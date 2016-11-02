#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <array>
#include <regex>
#include <vector>
#include <cassert>

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

using Address = std::size_t;

enum class Endianness { Big, Little };

class Process
{
    public:
        Process();
        Process(const std::string &programFilename);

        void open(const std::string &programFilename);
        void setEndianness(Endianness endianness) noexcept;

        // returns true if the current area is readable
        bool validArea(Address address);

        std::vector<char> readData(Address address, std::size_t length);

        // reads a null terminated string
        std::string readString(Address address);

        void writeData(Address address, std::vector<char> data);

        template<typename T> T readValue(Address address)
        {
            assert(std::is_arithmetic<T>::value);
            auto data = readData(address, sizeof(T));
            if(m_endianness == Endianness::Big)
                std::reverse(data.begin(), data.end());
            return *reinterpret_cast<T*>(data.data());
        }

        template<typename T> void writeValue(Address address, T value)
        {
            assert(std::is_arithmetic<T>::value);
            std::vector<char> data(sizeof(T));
            std::copy_n(reinterpret_cast<char*>(&value), data.size(), data.data());
            if(m_endianness == Endianness::Big)
                std::reverse(data.begin(), data.end());
            writeData(address, data);
        }

        // tries to find a string in the process memory
        // starts at 'address'
        // set 'backwards' to true if the string is located before 'address'
        // if the string is not found, the value 'npos' is returned
        Address findString(const std::string &str, Address address = 0, bool backwards = false) noexcept;

        // tries to find a regular expression in a chunk of size 'lenght' in the process memory
        // returns true if the regex has been found
        bool searchRegex(const std::regex &str, Address address = 0, std::size_t length = npos) noexcept;

        // returns the location of the specified process name
        static std::string getProcessLocation(const std::string &processName) noexcept;

        std::string getCurrentModuleName() noexcept;

        // a return value which indicates a failure
        static const Address npos { std::numeric_limits<std::size_t>::max() };

    private:
        std::vector<char> readDataNoExcept(Address address, std::size_t length) noexcept;

        HANDLE m_processHandle {nullptr};
        Endianness m_endianness { Endianness::Little };
};

#endif // PROCESS_H
