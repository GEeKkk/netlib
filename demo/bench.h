#ifndef BENCH_H
#define BENCH_H

#include <functional>
#include <chrono>
#include <stdio.h>
#include <string_view>

#define FuncName(func) #func

namespace Bench 
{
    using BenchCallback = std::function<void()>;

    void Run(const BenchCallback& cb, int times, std::string_view name) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < times; ++i) {
            cb();
        }
        auto end = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();
        printf("*(%s) run %d times, cost %fms\n", name.data(), times, elapsedMs);
    }
}

#endif // BENCH_H