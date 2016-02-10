#ifndef CLOCK_H
#define CLOCK_H

#include <chrono>
#include <memory>

using hrClock = std::chrono::high_resolution_clock;

class Clock
{
    public:
        Clock();
        float reset() noexcept;
        float elapsed() const noexcept;

    private:
        hrClock::time_point m_timePoint;
};

#endif // CLOCK_H
