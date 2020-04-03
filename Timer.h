#pragma once
#include <chrono>
class Timer
{
public:
	Timer() noexcept;
	double Mark() noexcept;
	double Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point m_last;
};

