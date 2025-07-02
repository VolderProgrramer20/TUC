// сделано эклером в 2024 и исправлено через claude 4 :) я даже не знаю РАБОТАЕТ ли это. 
// основано на коде процессов из ныне закрытого Slim Core.

// "Пофикшены некоторые ошибки и добавлены новые функции и переменные.
// Всё, теперь крадём код для NRC. Делаем бизнес 🤑🤑🤑" - SpaceOC
#ifndef TUC_PROCESS_MANAGER_H_
#define TUC_PROCESS_MANAGER_H_

#include <cstdint>
#include <array>
#include <deque>
#include <functional>
#include <optional>
#include <mutex>
#include <iostream>

// задаём макс. кол‑во процессов и размер стека каждого
constexpr size_t MAX_PROCS = 64;
constexpr size_t STACK_SIZE = 8 * 1024; // 8КБ

// состояния процесса
enum class ProcState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED // Я ухожу, адьёс
};

// вот что хранит ядро про каждый процесс
struct PCB {
    uint32_t pid;                    // уникальный ID
    ProcState state;                 // текущее состояние
    uint8_t* stack_base;             // где наш стек
    uint8_t* stack_ptr;              // текущий SP
    int memory_used;                 // Сколько памяти занято
    std::function<void(PCB*)> entry; // что запускать
};

namespace ProcessManager {
    // запустить эту хрень
    void init();

    /**
     * Создаёт новый процесс
     * @param entry_point Функция
     * @return Возвращает PID процесса, если успешно создан. В противном случае - std::nullopt
     */
    std::optional<uint32_t> create_process(std::function<void(PCB*)> entry_point);

    /**
     * Проверяет есть ли свободная память в стеке процесса или нет
     * @param process Указатель на процесс
     * @return true - если память заполнена, иначе false
     */
    bool stack_memory_is_full(PCB* process);

    /**
     * Проверяет есть ли свободная память в стеке процесса для данного типа или нет
     * @param process Указатель на процесс
     * @return true - если память заполнена, иначе false
     */
    template<typename T>
    bool stack_memory_is_full_t(PCB* process) {
        return (process->stack_ptr - sizeof(T) < process->stack_base);
    }

    /**
     * Добавляет данные в стек процесса
     * Если память заполнена или процесс не готов/запущен, то данные не оказываются в стеке
     * @param process Указатель на процесс
     * @param value Данные, которые нужно добавить в стек
     * @return Данные, которые нужно добавить в стек.
     */
    template <typename T>
    T push_to_stack(PCB* process, T value) {
        if (!process || (process->state != ProcState::RUNNING && process->state != ProcState::READY)) {
            std::cout << "[ProcessManager] - ERROR: This process not initialized. Value NOT pushed in the process stack\n";
            return value;
        }
        // Проверяем, есть ли достаточно свободного места для добавления новых данных
        if (stack_memory_is_full_t<T>(process)) {
            std::cout << "[ProcessManager] - ERROR: Memory is full. Value NOT pushed in the process stack\n";
            return value;
        }

        process->stack_ptr -= sizeof(T);
        // TODO: Придумать другой способ вывода информации о занятости памяти в стеке.
        process->memory_used += sizeof(T);
        *reinterpret_cast<T*>(process->stack_ptr) = value;
        return value;
    }

    // НЕ ПРОТЕСТИРОВАНО
    template <typename T>
    T pop_from_stack(PCB* process) {
        if (process->state != ProcState::RUNNING && process->state != ProcState::READY) {
            std::cout << "[ProcessManager] - ERROR: This process not initialized. Value NOT pushed in the process stack\n";
            T value;
            return value;
        }

        T value = *reinterpret_cast<T*>(process->stack_ptr);
        process->stack_ptr += sizeof(T);
        return value;
    }

    // Завершаем текущий процесс
    void exit_process();

    // блокируем и запрашиваем переключение
    void block_current();

    //когда айо готово — возвращает true если нашли
    bool unblock(uint32_t pid);

    // бессрочный цикл планировщика
    [[noreturn]] void scheduler_loop();

    // цикличная проверка времени выполнения текущего процесса
    // и авто-включения возможности его завершения через функцию trying_stop_process, если процесс завис (?)
    void checking_process_time_loop();

    /**
     * Досрочно пытаемся завершить текущий процесс
     * @return true - если удалось завершить. В противном случае - false
     */
    bool trying_force_stop_process();
}

#endif // TUC_PROCESS_MANAGER_H_