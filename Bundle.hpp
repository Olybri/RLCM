#ifndef BUNDLE_H
#define BUNDLE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <iterator>

#include <QFile>

using Long = long int;
using LongLong = long long int;

struct FileInfo
{
    std::string filename;
    Long size {0};
    Long cmpSize {0};
    LongLong offset {0};
};

class Bundle
{
    public:
        Bundle();
        Bundle(const std::string& gameFolder, const std::string& bundleName);

        void open(const std::string& gameFolder, const std::string& bundleName);
        std::vector<FileInfo>::iterator getFileInfo(const std::string& filename);
        void installTrainingRoom(bool install);
        bool checkTrainingRoom();

    private:
        template<typename T> T readValue() noexcept;
        template<typename T> T readValue(LongLong address) noexcept;

        // loads a resource file that has the same name as 'filename', the path is excluded
        // set 'mod' to true if you want to load the modded one
        std::vector<char> loadResource(const std::string &filename, bool mod) noexcept;

        // when reading a string, the first 4 bytes hold the length of the string
        std::string readString() noexcept;
        std::string readString(LongLong address) noexcept;

        std::fstream m_file;
        std::vector<FileInfo> m_fileList;

        std::string m_bundleName;

        const std::vector<std::string> fileList{
            "cache/itf_cooked/pc/enginedata/inputs/menu/input_menu_x360.isg.ckd",
            "cache/itf_cooked/pc/world/home/brick/challenge/challenge_endless.isc.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_levels/textures/challenge/challenge_1.tga.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_levels/textures/challenge/challenge_2.tga.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_levels/textures/challenge/challenge_3.tga.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_levels/textures/challenge/challenge_4.tga.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_levels/textures/challenge/challenge_5.tga.ckd",
            "cache/itf_cooked/pc/world/home/paintings_and_notifications/painting_challengeendless/animation/painting_challengeendless_a1.tga.ckd",
            "cache/itf_cooked/pc/world/common/ui/suitcase/animation/suitcase_a1.tga.ckd"};
};

#endif // BUNDLE_H
