#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "Process.hpp"

enum class Level
{
    Dojo,
    LotLD,
    Pit,
    Tower,
    Murfy,
    Unknown
};

enum class Event
{
    Time,
    Lums,
    LumsTime,
    Distance,
    LumsDistance,
    Unknown
};

enum class Difficulty
{
    Normal,
    Expert,
    Unknown
};

class Challenge
{
    public:
        Challenge();
        Challenge(const std::string programFilename);
        void openProcess(const std::string programFilename);
        void load();

        // These following functions converts the challenge informations
        // into readable strings.
        std::string getLevelName() const noexcept;
        std::string getEventName() const noexcept;
        std::string getDifficultyName() const noexcept;

        unsigned getSeed() const noexcept;
        float getGoal() const noexcept;
        float getLimit() const noexcept;

        std::string getGoalType() const noexcept;
        std::string getLimitType() const noexcept;

        void updateRules(unsigned seed, float goal, float limit);

    private:
        // We need 2 addresses to read/write the challenge rules. These
        // addresses are the locations of 2 occurrences of the challenge seed.
        // This function will try to find them in the process memory and store
        // them in 'm_addresses'.
        void findAddresses();

        // Each of these 2 addresses points to a structure which contains
        // informations about the challenge (seed, goal, score limit, level
        // difficulty, event). Both structures are the same, so using one
        // address is enough to read the informations.
        // This function will read the informations of the challenge and store
        // them in 'm_goal', 'm_limit', 'm_level', 'm_event' and 'm_difficulty'
        void readRules() noexcept;

        Process m_process;
        std::array<Address, 2> m_addresses;
        Address m_seedAddress;

        unsigned m_seed {0};
        float m_goal {0};
        float m_limit {0};
        Level m_level = Level::Unknown;
        Event m_event = Event::Unknown;
        Difficulty m_difficulty = Difficulty::Unknown;
};

#endif // CHALLENGE_H
