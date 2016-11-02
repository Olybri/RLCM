#include "Clock.hpp"

Clock::Clock() :
    m_timePoint(hrClock::now())
{
}

float Clock::reset() noexcept
{
    const auto seconds = elapsed();
    m_timePoint = hrClock::now();

    return seconds;
}

float Clock::elapsed() const noexcept
{
    const auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(hrClock::now() - m_timePoint);

    return static_cast<float>(elapsedTime.count()) / 1000;
}
