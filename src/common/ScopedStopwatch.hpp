#pragma once
#include <string>
#include <chrono>
#include <print>
#include <mutex>

class ScopedStopwatch {
public:
    ScopedStopwatch() = delete;
    explicit ScopedStopwatch(std::string_view name)
        : name_(name),
        start_tp_(std::chrono::high_resolution_clock::now()) {
    }
    ~ScopedStopwatch() {
        Stop();
    }
private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_tp_;
    inline static std::mutex output_mutex_;

    void Stop() {
        using namespace std::chrono;
        auto end_tp = high_resolution_clock::now();
        auto start = duration_cast<microseconds>(start_tp_.time_since_epoch()).count();
        auto end = duration_cast<microseconds>(end_tp.time_since_epoch()).count();
        auto diff_us = end - start;

        std::lock_guard<std::mutex> lock(output_mutex_);
        std::print("Stopwatch {} finished with duration {}.{}(ms).\n", name_, diff_us / 1000, diff_us % 1000);
    }
};