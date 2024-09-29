#pragma once
#include <string>
#include <chrono>
#include <format>
#include <iostream>

class DebugTimer {
public:
	DebugTimer() = delete;
	explicit DebugTimer(std::string_view name)
			: name_(name),
			  start_tp_(std::chrono::high_resolution_clock::now()) {
	}

	~DebugTimer() {
		if (!stopped_)
			stop();
	}

	void stop() {
		auto end_tp = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::duration_cast<std::chrono::milliseconds>(start_tp_.time_since_epoch()).count();
		auto end = std::chrono::duration_cast<std::chrono::milliseconds>(end_tp.time_since_epoch()).count();
		std::cout << std::format("Timer {} finished with duration {}(ms).\n", name_, end - start);
		stopped_ = true;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_tp_;
	std::string name_;
	bool stopped_ = false;
};
