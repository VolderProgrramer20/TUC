// process.cpp в основном благодаря ии.

// "Пофикшены некоторые ошибки и добавлены новые функции и переменные.
// Всё, теперь крадём код для NRC. Делаем бизнес 🤑🤑🤑" - SpaceOC
#include "tuc/process.h"
#include <atomic>
#include <cstring>
#include <ctime>
#include <chrono>

namespace ProcessManager {

    static std::array<PCB, MAX_PROCS> pcb_table;                 // Таблица процессов
    static std::deque<uint32_t> ready_queue(MAX_PROCS);          // кто ждёт своего шанса
    static std::deque<uint32_t> blocked_queue(MAX_PROCS);        // кто ждёт события
    static std::atomic<uint32_t> next_pid{1};                    // для выдачи pid’(ор)ов
    static std::mutex pm_mutex;                                  // эмуляция атомарности
    static uint32_t current_idx = 0;                             // Текущий индекс в таблице
    static time_t currentProcessTime;                            // Текущее время выполнения процесса
    
    void init() {
        std::lock_guard lock(pm_mutex);
        // сбрасываем все записи
        for (auto &pcb : pcb_table) {
            pcb.state = ProcState::TERMINATED;
            pcb.stack_base = pcb.stack_ptr = nullptr;
        }
        ready_queue.clear();
        blocked_queue.clear();
        current_idx = 0;
    }

    std::optional<uint32_t> create_process(std::function<void(PCB*)> entry_point) {
        std::lock_guard lock(pm_mutex);

        if (ready_queue.size() + blocked_queue.size() >= MAX_PROCS) {
            return std::nullopt; // больше никак соре (что за cope? - SpaceOC)
        }

        uint32_t pid = next_pid.fetch_add(1);
        size_t idx = pid % MAX_PROCS;

        // ищем свободный слот
        for (size_t i = 0; i < MAX_PROCS; ++i) {
            size_t try_idx = (idx + i) % MAX_PROCS;
            if (pcb_table[try_idx].state == ProcState::TERMINATED) {
                idx = try_idx;
                break;
            }
        }
        if (pcb_table[idx].state != ProcState::TERMINATED) {
            return std::nullopt;
        }

        // выделяем память под стек
        uint8_t* stack = new (std::nothrow) uint8_t[STACK_SIZE];
        if (!stack) {
            return std::nullopt;
        }

        pcb_table[idx] = {
            pid,
            ProcState::NEW,
            stack,
            stack + STACK_SIZE,
            0,
            std::move(entry_point)
        };

        // сразу в очередь готовых
        pcb_table[idx].state = ProcState::READY;
        ready_queue.push_back(idx);

        return pid;
    }

    bool stack_memory_is_full(PCB* process) {
        return (process->stack_ptr <= process->stack_base);
    }

    void exit_process() {
        std::lock_guard lock(pm_mutex);
        auto &pcb = pcb_table[current_idx];
        pcb.state = ProcState::TERMINATED;
        delete[] pcb.stack_base;
        pcb.stack_base = pcb.stack_ptr = nullptr;
        std::cout << "[proc#" << pcb.pid << "] end\n";
    }

    void block_current() {
        std::lock_guard lock(pm_mutex);
        auto &pcb = pcb_table[current_idx];
        pcb.state = ProcState::BLOCKED;
        blocked_queue.push_back(current_idx);
        std::cout << "[proc#" << pcb.pid << "] freeze...\n";
    }

    bool unblock(uint32_t pid) {
        std::lock_guard lock(pm_mutex);
        for (auto it = blocked_queue.begin(); it != blocked_queue.end(); ++it) {
            if (pcb_table[*it].pid == pid) {
                pcb_table[*it].state = ProcState::READY;
                ready_queue.push_back(*it);
                blocked_queue.erase(it);
                std::cout << "[proc#" << pid << "] unfreezed.\n";
                return true;
            }
        }
        return false;
    }

    [[noreturn]] void scheduler_loop() {
        while (true) {
            bool ready_quene_is_empty;
            uint32_t next_idx = 0;
            {
                std::lock_guard lock(pm_mutex);
                ready_quene_is_empty = ready_queue.empty();
                if (!ready_quene_is_empty) {
                    next_idx = ready_queue.front();
                    ready_queue.pop_front();
                }
                // Если вовремя не разблочить, то при попытке запустить следующий процесс случится неопределённое поведение
                // Возможно, причиной такой проблемы является беспричинный повторный запуск функции scheduler_loop в exit_process
                // TODO: Закомментировать эту строчку снизу и проверить это
                pm_mutex.unlock();
            }

            if (!ready_quene_is_empty) {
                current_idx = next_idx;
                auto &pcb = pcb_table[current_idx];
                pcb.state = ProcState::RUNNING;
                uint32_t pid = pcb.pid;
                std::cout << "[sched] start proc#" << pid << "\n";

                // TODO: Если тот или иной процесс будет слишком долго (зависать) выполнять функцию, то начать попытки его блокировки/выключения.
                // Для этого потребуется создать отдельный поток где будет считаться время выполнения процесса .-.
                // Ой б...

                auto start = std::chrono::steady_clock::now();

                // 'debug'
                //std::cout << "proc#" << pcb.pid << " adr: " << &pcb.stack_base << '\n';
                //std::cout << "proc#" << pcb.pid << " end adr: " << &pcb.stack_ptr << '\n';
                //std::cout << "proc#" << pcb.pid << " memory used before entry: " << pcb.memory_used << '\n';

                pcb.entry(&pcb);

                //std::cout << "proc#" << pcb.pid << " memory used after entry: " << pcb.memory_used << '\n';
                //std::cout << "proc#" << pcb.pid << " end adr: " << &pcb.stack_ptr << '\n';

                exit_process();
                auto end = std::chrono::steady_clock::now();
                long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                std::cout << "[sched] proc#" << pid << " time: " << duration << "ms\n";
            }
        }
    }

    void checking_process_time_loop() {
        // TODO: Сделать проверку на то, что процесс завис и его можно уже пытаться досрочно завершить
    }

    bool trying_force_stop_process() {
        // TODO: Сделать остановку процесса.
        return true;
    }
}