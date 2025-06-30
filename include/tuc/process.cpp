// process.cpp в основном благодаря ии.
#include "process.h"
#include <iostream>
#include <atomic>
#include <cstring>  

namespace ProcessManager {

    static std::array<PCB, MAX_PROCS> pcb_table;    // сама таблица процессов
    static std::deque<uint32_t> ready_queue;        // кто ждёт своего шанса
    static std::deque<uint32_t> blocked_queue;      // кто ждёт события
    static std::atomic<uint32_t> next_pid{1};       // для выдачи pid’(ор)ов
    static std::mutex pm_mutex;                     // эмуляция атомарности
    static uint32_t current_idx = 0;                // индекс в таблице текущ.
    
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

    std::optional<uint32_t> create_process(std::function<void()> entry_point) {
        std::lock_guard lock(pm_mutex);

        if (ready_queue.size() + blocked_queue.size() >= MAX_PROCS) {
            return std::nullopt; // больше никак соре
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
            std::move(entry_point)
        };

        // сразу в очередь готовых
        pcb_table[idx].state = ProcState::READY;
        ready_queue.push_back(idx);

        return pid;
    }

    [[noreturn]] void exit_process() {
        std::lock_guard lock(pm_mutex);
        auto &pcb = pcb_table[current_idx];
        pcb.state = ProcState::TERMINATED;
        delete[] pcb.stack_base;
        pcb.stack_base = pcb.stack_ptr = nullptr;
        std::cout << "[proc#" << pcb.pid << "] мы закончили\n";
        // переходим снова в планировщик
        scheduler_loop();
    }

    void block_current() {
        std::lock_guard lock(pm_mutex);
        auto &pcb = pcb_table[current_idx];
        pcb.state = ProcState::BLOCKED;
        blocked_queue.push_back(current_idx);
        std::cout << "[proc#" << pcb.pid << "] застопорились, ждём...\n";
        scheduler_loop();
    }

    bool unblock(uint32_t pid) {
        std::lock_guard lock(pm_mutex);
        for (auto it = blocked_queue.begin(); it != blocked_queue.end(); ++it) {
            if (pcb_table[*it].pid == pid) {
                pcb_table[*it].state = ProcState::READY;
                ready_queue.push_back(*it);
                blocked_queue.erase(it);
                std::cout << "[proc#" << pid << "] разбудили, ставим в очередь\n";
                return true;
            }
        }
        return false;
    }

    [[noreturn]] void scheduler_loop() {
        while (true) {
            uint32_t next_idx = 0;
            {
                std::lock_guard lock(pm_mutex);
                if (ready_queue.empty()) {
                    continue; // никого нет ыы
                }
                next_idx = ready_queue.front();
                ready_queue.pop_front();
            }
            current_idx = next_idx;
            auto &pcb = pcb_table[current_idx];
            pcb.state = ProcState::RUNNING;
            std::cout << "[sched] запускаем proc#" << pcb.pid << "\n";

            // выполняем user‑код
            pcb.entry();

            // если юзер вернулся — значит он завершился
            exit_process();
        }
    }

} 





