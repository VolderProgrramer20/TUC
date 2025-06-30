// сделано эклером в 2024 и исправлено через claude 4 :) я даже не знаю РАБОТАЕТ ли это. 
// основано на коде процессов из ныне закрытого Slim Core.
#pragma once
#include <cstdint>
#include <array>
#include <deque>
#include <functional>
#include <optional>
#include <mutex>

// задаём макс. кол‑во процессов и размер стека каждого
constexpr size_t MAX_PROCS = 64;
constexpr size_t STACK_SIZE = 8 * 1024; // 8КБ

// состояния процесса
enum class ProcState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// вот что хранит ядро про каждый процесс
struct PCB {
    uint32_t pid;                    // уникальный ID
    ProcState state;                 // текущее состояние
    uint8_t* stack_base;             // где наш стек
    uint8_t* stack_ptr;              // текущий SP
    std::function<void()> entry;     // что запускать
};

namespace ProcessManager {
    // запустить эту хрень
    void init();

    // создать новый процесс возвращает pid или нулл
    std::optional<uint32_t> create_process(std::function<void()> entry_point);

    // внутри процесса вызываем чтобы завершить
    [[noreturn]] void exit_process();

    // блокируем и запрашиваем переключение
    void block_current();

    //когда айо готово — возвращает true если нашли
    bool unblock(uint32_t pid);

    // бессрочный цикл планировщика
    [[noreturn]] void scheduler_loop();
}
