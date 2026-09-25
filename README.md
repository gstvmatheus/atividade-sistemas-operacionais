# Central de processamento | produtor-consumidor em C++

Simulação do problema clássico **Produtor-consumidor** com múltiplas threads produtoras e consumidoras, implementada em C++ usando `std::thread`, `std::mutex` e `std::condition_variable`. O projeto também trata o problema de **alocação de múltiplos recursos** (deadlock avoidance) ao processar cada tarefa.

Atividade da disciplina de **sistemas operacionais**.

## Como funciona

- **3 threads produtoras** geram 5 tarefas cada, totalizando 15 tarefas, e as inserem em uma fila compartilhada.
- **5 threads consumidoras** retiram tarefas da fila e as processam.
- A fila tem capacidade máxima de 5 itens: quando está cheia, as produtoras aguardam; quando está vazia, as consumidoras aguardam.
- Cada consumidora precisa de **2 recursos compartilhados** (de um total de 5, protegidos por mutexes) para processar uma tarefa. Os recursos são sempre bloqueados em **ordem crescente de índice**, o que evita deadlocks (previne a formação de espera circular).
- Quando todas as produtoras terminam, a flag `fim_producao` é sinalizada e as consumidoras são liberadas para finalizar assim que a fila estiver vazia.
- Um mutex adicional (`mtx_cout`) garante que as mensagens de log impressas no console por diferentes threads não se misturem.

### Sincronização utilizada

| Mecanismo | Propósito |
|---|---|
| `mutex mtx_fila` + `condition_variable cv_produtora` / `cv_consumidora` | Protege a fila e implementa a espera condicional entre produtoras e consumidoras |
| `mutex recursos[5]` | Protege o acesso aos 5 recursos compartilhados usados no processamento das tarefas |
| `mutex mtx_cout` | Evita concorrência na saída padrão (`std::cout`) |
| Bloqueio de recursos em ordem crescente | Estratégia de prevenção de deadlock (ordenação de recursos) |

## Requisitos

- Compilador C++ com suporte a C++11 ou superior (g++, clang++, MSVC etc.)
- Suporte à biblioteca de threads da plataforma (no Linux, link com `-pthread`)

## Compilando e executando

```bash
g++ -std=c++17 -pthread novaCentral.cpp -o central
./central
```

No Windows (MinGW):

```bash
g++ -std=c++17 novaCentral.cpp -o central.exe
central.exe
```

## Saída esperada

O programa imprime no console, em tempo real, mensagens como:

```
=== CENTRAL DE PROCESSAMENTO ===
Produtora 1 criou a tarefa 101
Consumidora 0 pegou a tarefa 101
Consumidora 0 processando tarefa 101 com recursos 0 e 1
...
Todas as tarefas foram processadas.
```

A ordem exata das mensagens pode variar a cada execução, já que depende do escalonamento das threads pelo sistema operacional.
