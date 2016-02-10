#include "Process.hpp"

using std::cout;
using std::endl;
using std::flush;

std::string wstrToStr(const std::wstring &ws)
{
    std::string str(ws.begin(), ws.end());
    return str;
}

Process::Process()
{
}

Process::Process(const std::string &programFilename)
{
    open(programFilename);
}

void Process::open(const std::string &programFilename)
{
    // Getting process name from program filename
    auto processName = programFilename;
    std::replace(processName.begin(), processName.end(), '/', '\\');
    processName = processName.substr(processName.find_last_of('\\') + 1);

    cout << "Opening process " << processName << "... " << endl;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    m_processHandle = nullptr;

    if(Process32First(snapshot, &entry))
    {
        while(Process32Next(snapshot, &entry))
            if(processName == wstrToStr(entry.szExeFile))
            {
                m_processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);

                std::wstring moduleFilename(MAX_PATH, 0);
                auto strSize = GetModuleFileNameEx(m_processHandle, nullptr, &moduleFilename[0], MAX_PATH);
                moduleFilename.erase(strSize);

                auto strToUpper = [](const std::string &str)
                {
                    std::string newStr;
                    std::transform(str.begin(), str.end(), std::back_inserter(newStr), ::toupper);
                    return newStr;
                };

                if(strToUpper(wstrToStr(moduleFilename)) != strToUpper(programFilename))
                    m_processHandle = nullptr;

                else break;
            }
    }

    if(m_processHandle == nullptr)
    {
        cout << "Failure!" << endl;
        throw std::runtime_error("Failed to open process \"" + processName + "\"\nfrom \"" + programFilename + "\".");
    }
    else
        cout << "Success! (Process ID: " << std::hex << std::showbase << entry.th32ProcessID << ")" << endl;
}

void Process::setEndianness(Endianness endianness) noexcept
{
    m_endianness = endianness;
}

bool Process::validArea(Address address)
{
    char byte;
    return ReadProcessMemory(m_processHandle, reinterpret_cast<void*>(address),
        &byte, sizeof(byte), nullptr);
}

std::vector<char> Process::readDataNoExcept(Address address, std::size_t length) noexcept
{
    std::vector<char> buffer(length);
    if(!ReadProcessMemory(m_processHandle, reinterpret_cast<void*>(address),
        buffer.data(), buffer.size(), nullptr))
            return std::vector<char>();

    return buffer;
}

std::vector<char> Process::readData(Address address, std::size_t length)
{
    auto buffer = readDataNoExcept(address, length);
    if(length != buffer.size())
    {
        cout << "Failure!" << endl;

        std::ostringstream os;
        os << "Failed to read memory from process \"" << getCurrentModuleName() << "\" (at " << std::showbase << std::hex << address << ").";

        throw std::runtime_error(os.str());
    }

    return buffer;
}

std::string Process::readString(Address address)
{
    std::string str = readData(address, MAX_PATH).data();
    return str;
}

void Process::writeData(Address address, std::vector<char> data)
{
    if(!WriteProcessMemory(m_processHandle, reinterpret_cast<void*>(address), data.data(), data.size(), nullptr))
    {
        cout << "Failure!" << endl;

        std::ostringstream os;
        os << "Failed to write memory into process \"" << getCurrentModuleName() << "\" (at " << std::showbase << std::hex << address << ").";

        throw std::runtime_error(os.str());
    }
}

Address Process::findString(const std::string &str, Address address, bool backwards) noexcept
{
    const std::size_t chunkLength {0x1000};
    const auto startAddress = address;
    address = startAddress - (backwards ? chunkLength : 0);

    for(;backwards ? address > 0 : npos - address >= chunkLength;
        address += (backwards ? -1 : 1) * chunkLength - address % chunkLength)
    {
        if(!validArea(address))
            continue;

        auto buffer = readDataNoExcept(address, chunkLength);
        if(buffer.empty())
            continue;

        auto it = std::search(buffer.begin(), buffer.end(), str.begin(), str.end());
        if(it != buffer.end())
            return address + std::distance(buffer.begin(), it);
    }

    cout << endl << "Can't find string \"" << str << "\" in process memory! (From address " << std::showbase << std::hex << startAddress << ")" << endl;
    return npos;
}

bool Process::searchRegex(const std::regex &reg, Address address, std::size_t length) noexcept
{
    auto buffer = readDataNoExcept(address, length);

    return std::regex_search(buffer.begin(), buffer.end(), reg);
}

std::string Process::getProcessLocation(const std::string &processName) noexcept
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    std::wstring processLocation {};
    if(Process32First(snapshot, &entry))
    {
        while(Process32Next(snapshot, &entry))
            if(processName == wstrToStr(entry.szExeFile))
            {
                auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);

                processLocation.assign(MAX_PATH, 0);
                GetModuleFileNameEx(handle, nullptr, &processLocation[0], MAX_PATH);

                auto index = processLocation.find_last_of('\\');
                processLocation.erase(index + 1);

                break;
            }
    }

    return wstrToStr(processLocation);
}

std::string Process::getCurrentModuleName() noexcept
{
    std::wstring moduleName(MAX_PATH, 0);
    auto strSize = GetModuleBaseName(m_processHandle, nullptr, &moduleName[0], MAX_PATH);
    moduleName.erase(strSize);

    return wstrToStr(moduleName);
}
