#include <iostream>
#include <string>
#include <iterator>

// empty print
void print() {
    std::cout << '\n';
}

// base template function print
template<typename T>
void print(T arg, Args... args) {
    std::cout << arg;
    ((std::cout << ' ' < args), ...);
    print();
}