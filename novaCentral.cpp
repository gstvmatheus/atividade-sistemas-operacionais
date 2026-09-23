#include <iostream>
#include <thread>
#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>

using namespace std;

queue<int> fila;
bool fim_producao = false;

mutex mtx_fila;
condition_variable cv_produtora;
condition_variable cv_consumidora;

mutex recursos[5];
mutex mtx_cout;

void log(const string& msg) {
    lock_guard<mutex> lock(mtx_cout);
    cout << msg << endl;
}

void produtora(int id) {
    for (int i = 1; i <= 5; i++) {
        int tarefa = id * 100 + i;

        unique_lock<mutex> lock(mtx_fila);
        cv_produtora.wait(lock, [] { return fila.size() < 5; });

        fila.push(tarefa);
        log("Produtora " + to_string(id) + " criou a tarefa " + to_string(tarefa));

        lock.unlock();
        cv_consumidora.notify_one();

        this_thread::sleep_for(chrono::milliseconds(300));
    }
    log("Produtora " + to_string(id) + " finalizou.");
}

void consumidora(int id) {
    int r1 = min(id, (id + 1) % 5);
    int r2 = max(id, (id + 1) % 5);

    while (true) {
        int tarefa;

        {
            unique_lock<mutex> lock(mtx_fila);
            cv_consumidora.wait(lock, [] { return !fila.empty() || fim_producao; });

            if (fila.empty() && fim_producao) {
                break;
            }

            tarefa = fila.front();
            fila.pop();
            cv_produtora.notify_one();
        }

        log("Consumidora " + to_string(id) + " pegou a tarefa " + to_string(tarefa));

        // Bloqueia em ordem crescente para evitar deadlock
        lock_guard<mutex> l1(recursos[r1]);
        lock_guard<mutex> l2(recursos[r2]);

        log("Consumidora " + to_string(id) + " processando tarefa " + to_string(tarefa) + 
            " com recursos " + to_string(r1) + " e " + to_string(r2));

        this_thread::sleep_for(chrono::milliseconds(600));

        log("Consumidora " + to_string(id) + " concluiu a tarefa " + to_string(tarefa));
    }

    log("Consumidora " + to_string(id) + " finalizou.");
}

int main() {
    log("=== CENTRAL DE PROCESSAMENTO ===");

    vector<thread> produtoras;
    vector<thread> consumidoras;

    for (int i = 1; i <= 3; i++) {
        produtoras.emplace_back(produtora, i);
    }

    for (int i = 0; i <= 4; i++) {
        consumidoras.emplace_back(consumidora, i);
    }

    for (auto& p : produtoras) {
        p.join();
    }

    {
        lock_guard<mutex> lock(mtx_fila);
        fim_producao = true;
    }
    cv_consumidora.notify_all();

    for (auto& c : consumidoras) {
        c.join();
    }

    log("\nTodas as tarefas foram processadas.");

    return 0;
}