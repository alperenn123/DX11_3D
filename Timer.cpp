#include "Timer.h"
Timer::Timer() noexcept
{
	m_last = std::chrono::steady_clock::now();
}

double Timer::Mark() noexcept
{
	const auto old = m_last;
	m_last = std::chrono::steady_clock::now();
	const std::chrono::duration<double> frameTime = m_last - old;
	return frameTime.count();
}

double Timer::Peek() const noexcept
{
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_last).count();
}