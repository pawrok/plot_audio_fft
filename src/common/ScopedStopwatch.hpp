#pragma once
#include <string>
#include <chrono>
#include <print>
#include <mutex>

class ScopedStopwatch {
public:
    ScopedStopwatch() = delete;
    ScopedStopwatch(const ScopedStopwatch&)            = delete;
    ScopedStopwatch& operator=(const ScopedStopwatch&) = delete;
    ScopedStopwatch(ScopedStopwatch&&)                 = delete;
    ScopedStopwatch& operator=(ScopedStopwatch&&)      = delete;

    explicit ScopedStopwatch(std::string_view name)
        : m_name(name), m_start(clock_t::now()) {}

    ~ScopedStopwatch() { stop(); }
    void stop() {
        if (m_stopped) return;
        const auto diff = std::chrono::duration_cast<microseconds>(clock_t::now() - m_start);

        std::lock_guard lock(m_mutex);
#if defined(__cpp_lib_print)
        std::print("Stopwatch {}: {:.3f} ms\n", m_name, diff.count() / 1000.0);
#else
        std::printf("Stopwatch %s: %.3f ms\n", m_name.c_str(), diff.count() / 1000.0);
#endif
        m_stopped = true;
    }

private:
    using clock_t   = std::chrono::steady_clock;
    using timepoint = std::chrono::time_point<clock_t>;

    const std::string m_name;
    timepoint m_start;
    bool m_stopped = false;
    
    inline static std::mutex m_mutex;
};
