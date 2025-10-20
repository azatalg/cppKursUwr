#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "defines.h"

#define CASE_EX(n) case n: this->CAT(Ex_,n()); break;

#define SWITCH_EX_1()  CASE_EX(1)
#define SWITCH_EX_2()  SWITCH_EX_1()  CASE_EX(2)
#define SWITCH_EX_3()  SWITCH_EX_2()  CASE_EX(3)
#define SWITCH_EX_4()  SWITCH_EX_3()  CASE_EX(4)
#define SWITCH_EX_5()  SWITCH_EX_4()  CASE_EX(5)
#define SWITCH_EX_6()  SWITCH_EX_5()  CASE_EX(6)
#define SWITCH_EX_7()  SWITCH_EX_6()  CASE_EX(7)
#define SWITCH_EX_8()  SWITCH_EX_7()  CASE_EX(8)
#define SWITCH_EX_9()  SWITCH_EX_8()  CASE_EX(9)
#define SWITCH_EX_10() SWITCH_EX_9()  CASE_EX(10)

#define DISPATCH_SWITCH_EX(N)  CAT(SWITCH_EX_, N)()

#define REGISTER_TASKS(N)                                              \
    do {                                                               \
        tasks.reserve(N);                                              \
        for (int i = 1; i <= (N); ++i) {                               \
            tasks.emplace_back([this, i]() {                           \
                switch (i) {                                           \
                    DISPATCH_SWITCH_EX(N)                              \
                    default: break;                                    \
                }                                                      \
            });                                                        \
        }                                                              \
    } while (0)

#define REGISTER_LIST(ListType)                                        \
    ListBase& CAT(Create, ListType()) { static ListType inst; return inst; }

#define DECLARE_LIST(ListType)  ListBase& Create##ListType();

class ListBase {
protected:
    vec<fn<void()>> tasks;

public:
    virtual ~ListBase() = default;
    NO_DISCARD virtual std::string_view Name() const {
        return "Lista nie ma nazwy";
    };

    void RunAll() {
        if (tasks.empty()) UNLIKELY {
            std::cout << "Brak zada� w " << Name() << "\n";
            return;
        }
        std::cout << "== " << Name() << " ==\n";
        for (usize i = 0; i < tasks.size(); ++i) {
            std::cout << "-- Zad" << (i + 1) << " --\n";
            tasks[i]();
            std::cout << endl;
        }
    }

    void RunTask(int id) {
        if (id <= 0 || id > (int)tasks.size()) UNLIKELY {
            std::cout << "Brak takiego zadania: " << id << "\n";
            return;
        }
        std::cout << "-- Zad" << id << " --\n";
        tasks[id - 1]();
    }

    NO_DISCARD int TaskCount() const { return (int)tasks.size(); }
};