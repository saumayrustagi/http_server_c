# Web Server in C

Project to learn how multithreading, threadpools, function pointers and manually-managed memory works in C.

## Implemented Components

* **Task Queue:** A thread-safe queue to store incoming tasks (client connections) awaiting processing.

* **Thread Pool:** A pool of worker threads that retrieve tasks from the queue and execute them in parallel.

* **Web Server:** The core server component responsible for listening for incoming connections, accepting them, and enqueuing them as tasks for the thread pool.

## Building
### Prerequisites

* `gcc`
* `make`
* `pthread` library

### Debug Mode (Sanitizers)

```bash
make            # Builds the project in debug mode
./main.out      # Runs the server with sanitizers enabled
```

### Release Mode (-O2)

```bash
make fast       # Builds the project in release mode
./fast_main.out # Runs the server with optimizations
```

## Demo

https://github.com/user-attachments/assets/71a56a53-f65a-4560-8892-f3080f6e52c5




