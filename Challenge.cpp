#include "Challenge.hpp"

using std::cout;
using std::endl;

Challenge::Challenge()
{
}

Challenge::Challenge(const std::string programFilename)
{
    openProcess(programFilename);
}

void Challenge::openProcess(const std::string programFilename)
{
    cout << endl;
    m_process.open(programFilename);
}

void Challenge::load()
{
    cout << endl << "Loading running challenge:" << endl;
    findAddresses();
    readRules();
}

void Challenge::findAddresses()
{
    Address address {0};
    bool isDojo;

    cout << "Searching first address in process memory... " << endl;

    while(true)
    {
        address = m_process.findString("countdown", address);
        if(address == Process::npos)
            throw std::runtime_error("Failed to load challenge! (Challenge seed not found.)");

        auto countdownFilename = m_process.readString(address);

        std::string regexStr;
        bool found = false;
        if(countdownFilename == "countdown.act")
        {
            // 01 00 00 00   XX XX XX XX   00 00 00 00   00 00 00 00 'str'
            // (assuming XX is one byte of the seed and 'str' is the string that has been found)

            regexStr = R"(\x01\x00{3}[\s\S]{4}\x00{8})";
            isDojo = false;
            found = true;

        }
        else if(countdownFilename == "countdown_shaolin.act")
        {
            // XX XX XX XX   ?? ?? ?? ??   ?? ?? ?? ??   ?? ?? ?? ??
            // ?? ?? ?? ??   ?? ?? ?? ??   ?? ?? ?? ??   ?? ?? ?? ??
            // ?? ?? ?? ??   ?? ?? ?? ??   ?? ?? ?? ??   ?? ?? ?? ??
            // 02 00 00 00   02 00 00 00   ?? ?? ?? ??   ?? ?? ?? ??
            // 00 00 00 00   01 00 00 00   ?? ?? ?? ??   ?? ?? ?? ??
            // ?? ?? ?? ??   00 00 00 00   ?? ?? ?? ??   00 00 00 00
            // 00 00 00 00   ?? ?? ?? ??   ?? ?? ?? ??   00 00 00 00
            // ?? ?? ?? ??   'str'
            // (assuming XX is one byte of the seed, ?? is an unknown byte and 'str' is the string that has been found)

            regexStr = R"(\x02\x00{3}\x02\x00{3}[\s\S]{8}\x00{4}\x01\x00{3}[\s\S]{12}\x00{4}[\s\S]{4}\x00{8}[\s\S]{8}\x00{4})";
            isDojo = true;
            found = true;
        }

        if(found)
        {
            const auto bufferSize = (isDojo ? 0x44 : 0x10);
            if(m_process.searchRegex(std::regex(regexStr), address - bufferSize, bufferSize))
                break;
        }
        ++address;
    }

    address -= (isDojo ? 0x74 : 0x0C);

    m_seedAddress = address;

    m_process.setEndianness(Endianness::Big);
    m_seed = m_process.readValue<unsigned>(address);
    cout << "> Seed: " << std::showbase << std::hex << m_seed <<  " (address: " << m_seedAddress << ")" << endl;

    if(m_seed == 0x0)
        throw std::runtime_error("Can't continue process with challenge seed: 00 00 00 00\nSeed might have been edited outside the game.");

    address -= (isDojo ? 0x12C : 0x5F8);
    auto tempSeed = m_process.readValue<unsigned>(address);
    if(tempSeed != m_seed)
    {
        std::ostringstream os;
        os << "Challenge seed might be corrupted: " << std::showbase << std::hex << m_seed << " != " << tempSeed << " (at " << address - 0x34 << ").";
        throw std::runtime_error(os.str());
    }

    cout << "Success! (address: " << address << ")" << endl;
    m_addresses[1] = address;

    auto isg = m_process.readString(address + 0x34);
    cout << "> ISG filename: " << isg << endl;

    address = 0x1000'0000;

    cout << "Searching second address in process memory... " << endl;

    while(true)
    {
        address = m_process.findString(isg, address, true);
        if(address == Process::npos)
            throw std::runtime_error("Failed to load challenge! (ISG filename not found.)");

        if(m_process.readValue<unsigned>(address - 0x34) == m_seed)
            break;

        --address;
    }

    address -= 0x34;

    cout << "Success! (address: " << address << ")" << endl;
    m_addresses[0] = address;
}

void Challenge::readRules() noexcept
{
    cout << "Getting challenge informations in process memory... " << endl;

    auto isg = m_process.readString(m_addresses[0] + 0x34);

    auto isgLevel = isg.substr(0, isg.find_last_of('_'));
    auto isgEvent = isgLevel.substr(isgLevel.find_last_of('_') + 1);
    isgLevel = isgLevel.substr(0, isgLevel.find_last_of('_'));
    auto isgDifficulty = isg.substr(isg.find_last_of('_') + 1, 6);


    if(isgLevel == "challenge_spikyroad")
        m_level = Level::Pit;

    else if(isgLevel == "challenge_run")
        m_level = Level::LotLD;

    else if(isgLevel == "challenge_goingup")
        m_level = Level::Tower;

    else if(isgLevel == "challenge_drc_castle")
        m_level = Level::Murfy;

    else if(isgLevel == "challenge_shaolin")
        m_level = Level::Dojo;

    cout << "> Level: " << getLevelName() << endl;


    if(isgDifficulty == "normal")
        m_difficulty = Difficulty::Normal;

    else if(isgDifficulty == "expert")
        m_difficulty = Difficulty::Expert;

    cout << "> Difficulty: " << getDifficultyName() << endl;


    m_process.setEndianness(Endianness::Little);

    m_goal = m_process.readValue<float>(m_addresses[0] + 0x0C);
    cout << "> Goal: " << m_goal << endl;

    m_limit = m_process.readValue<float>(m_addresses[0] + 0x10);
    cout << "> Score limit: " << m_limit << endl;

    if(isgEvent == "default")
    {
        if(m_level == Level::Dojo)
        {
            if(m_goal == 60 && m_limit == 5)
                m_event = Event::LumsTime;
            else
                m_event = Event::Lums;
        }

        else if(m_goal != -1)
            m_event = Event::Time;

        else
            m_event = Event::Distance;
    }

    else if(isgEvent == "lumsattack")
        m_event = Event::Lums;

    else if(isgEvent == "timeattack")
        m_event = Event::Time;

    else if(isgEvent == "asmanylumsasyoucan")
        m_event = Event::LumsDistance;

    cout << "> Event: " << getEventName() << endl;

    cout << "Success!" << endl;
}

std::string Challenge::getLevelName() const noexcept
{
    switch(m_level)
    {
        case Level::Dojo:
            return "The Dojo";

        case Level::LotLD:
            return "Land of the Livid Dead";

        case Level::Murfy:
            return "Murfy's Dungeon";

        case Level::Pit:
            return "The Neverending Pit";

        case Level::Tower:
            return "The Infinite Tower";

        case Level::Unknown:
            return "Unknown level";
    }

    return "N/A";
}

std::string Challenge::getEventName() const noexcept
{
    switch(m_event)
    {
        case Event::Distance:
            return "As far as you can!";

        case Event::Lums:
            return "Grab them quickly!";

        case Event::LumsDistance:
            return "As many as you can!";

        case Event::LumsTime:
            return "As many as you can in 60\"!";

        case Event::Time:
            return "Get there quickly!";

        case Event::Unknown:
            return "Unknown event";
    }

    return "N/A";
}

std::string Challenge::getDifficultyName() const noexcept
{
    switch(m_difficulty)
    {
        case Difficulty::Expert:
            return "Expert";

        case Difficulty::Normal:
            return "Normal";

        case Difficulty::Unknown:
            return "Unknown difficulty";
    }

    return "N/A";
}

unsigned Challenge::getSeed() const noexcept
{
    return m_seed;
}

float Challenge::getGoal() const noexcept
{
    return m_goal;
}

float Challenge::getLimit() const noexcept
{
    return m_limit;
}

std::string Challenge::getGoalType() const noexcept
{
    switch(m_event)
    {
        case Event::Distance:
        case Event::LumsDistance:
            return "";

        case Event::Lums:
            return "lums";

        case Event::LumsTime:
            return "sec";

        case Event::Time:
            return "meters";

        case Event::Unknown:
            return "";
    }

    return "";
}

std::string Challenge::getLimitType() const noexcept
{
    switch(m_event)
    {
        case Event::Distance:
            return "meters";

        case Event::LumsDistance:
        case Event::LumsTime:
            return "lums";

        case Event::Lums:
        case Event::Time:
            return "sec";

        case Event::Unknown:
            return "";
    }

    return "";
}

void Challenge::updateRules(unsigned seed, float goal, float limit)
{
    auto updateValue = [this](Address address, auto value)
    {
        m_process.writeValue<decltype(value)>(address, value);
        cout << "At " << std::hex << std::showbase << address << ": sucess!" << endl;
    };

    cout << endl;

    if(seed != m_seed)
    {
        cout << "Updating seed..." << endl;
        m_process.setEndianness(Endianness::Big);
        updateValue(m_seedAddress, seed);
        updateValue(m_addresses[0], seed);
        updateValue(m_addresses[1], seed);

        m_seed = seed;
        cout << "Seed has been updated successfully!" << endl;
    }

    if(goal != m_goal && m_event != Event::Distance && m_event != Event::LumsDistance)
    {
        cout << "Updating goal..." << endl;
        m_process.setEndianness(Endianness::Little);
        updateValue(m_addresses[0] + 0x0C, goal);
        updateValue(m_addresses[1] + 0x0C, goal);

        m_goal = goal;
        cout << "Goal has been updated successfully!" << endl;
    }

    if(limit != m_limit)
    {
        cout << "Updating score limit..." << endl;
        m_process.setEndianness(Endianness::Little);
        updateValue(m_addresses[0] + 0x10, limit);
        updateValue(m_addresses[1] + 0x10, limit);

        m_limit = limit;
        cout << "Score limit has been updated successfully!" << endl;
    }
}
