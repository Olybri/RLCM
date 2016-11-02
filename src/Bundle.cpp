#include "Bundle.hpp"

using std::cout;
using std::endl;
using std::flush;

Bundle::Bundle()
{
}

Bundle::Bundle(const std::string& gameFolder, const std::string& bundleName)
{
    open(gameFolder, bundleName);
}

void Bundle::open(const std::string& gameFolder, const std::string& bundleName)
{
    m_bundleName = bundleName;
    const std::string bundleFilename = gameFolder + bundleName;

    cout << endl << "Opening game data package " << m_bundleName << "... " << flush;

    m_file.open(bundleFilename, std::ios::in | std::ios::out | std::ios::binary);

    if(!m_file)
    {
        cout << "Failure!" << endl;
        throw std::runtime_error("Can't open file \"" + bundleFilename + "\"!");
    }

    // address 0x0C holds the size of the base offset
    const auto baseOffset = readValue<Long>(0x0C);

    // address 0x2C holds the number of files
    const auto fileCount = readValue<Long>(0x2C);
    m_fileList.resize(fileCount);

    // read every file informations
    for(auto &fileInfo : m_fileList)
    {
        auto dummy = readValue<Long>();

        fileInfo.size = readValue<Long>();
        fileInfo.cmpSize = readValue<Long>();

        readValue<LongLong>();

        fileInfo.offset = readValue<LongLong>() + baseOffset;

        if(dummy == 2)
            readValue<LongLong>();

        fileInfo.filename = readString();
        fileInfo.filename += readString();

        readValue<LongLong>();
    }

    if(!m_file)
        throw std::runtime_error("File \"" + bundleFilename + "\" is corrupted!");

    cout << "Success! (" << std::dec << fileCount << " files)" << endl;
}

std::vector<FileInfo>::iterator Bundle::getFileInfo(const std::string& filename)
{
    auto it = std::find_if(m_fileList.begin(), m_fileList.end(),
        [&filename](const auto &fileInfo){ return fileInfo.filename == filename; });

    if(it == m_fileList.end())
        throw std::runtime_error("Can't find file \"" + filename + "\" in " + m_bundleName + "!");

    return it;
}

void Bundle::installTrainingRoom(bool install)
{
    cout << (install ? "Installing" : "Uninstalling") << " the training room:" << endl;

    unsigned count = 1;
    for(const auto &fileName : fileList)
    {
        cout << "Writing file " << std::dec << count << "/" << fileList.size() << ": "
            << fileName.substr(fileName.find_last_of('/') + 1) << "..." << endl;

        ++count;

        const auto address = getFileInfo(fileName)->offset;
        m_file.seekp(address);

        const auto resource = loadResource(fileName, install);
        m_file.write(resource.data(), resource.size());

        if(!m_file)
            throw std::runtime_error("Failed to write into " + m_bundleName + "!");

        cout << "Success! (" << std::hex << std::showbase << resource.size()
            << " bytes written at address " << address << ")" << endl;
    }

    cout << "Training room has been " << (install ? "installed" : "uninstalled") << " successfully!" << endl;
}

bool Bundle::checkTrainingRoom()
{
    auto checkFile = [this](const std::string &filename)
    {
        const auto address = getFileInfo(filename)->offset;
        m_file.seekg(address);

        auto modData = loadResource(filename, true);
        decltype(modData) gameData(modData.size());
        m_file.read(gameData.data(), modData.size());

        if(!m_file)
            throw std::runtime_error("Failed to read in " + m_bundleName + "!");

        return gameData == modData;
    };

    return checkFile(fileList[0]) && checkFile(fileList[1]);
}

template<typename T> T Bundle::readValue() noexcept
{
    std::vector<char> buffer(sizeof(T));
    m_file.read(buffer.data(), buffer.size());
    std::reverse(buffer.begin(), buffer.end());

    return *reinterpret_cast<T*>(buffer.data());
}

template<typename T> T Bundle::readValue(LongLong address) noexcept
{
    m_file.seekg(address);
    return readValue<T>();
}

std::string Bundle::readString() noexcept
{
    std::string str(readValue<Long>(), 0);
    m_file.read(const_cast<char*>(str.data()), str.size());

    return str;
}

std::string Bundle::readString(LongLong address) noexcept
{
    m_file.seekg(address);
    return readString();
}

std::vector<char> Bundle::loadResource(const std::string &filename, bool mod) noexcept
{
    QFile resource(QString(mod ? ":/data/mod/" : ":/data/default/") + QString(filename.c_str()).split("/").last());

    if(!resource.open(QIODevice::ReadOnly))
    {
        resource.setFileName(resource.fileName() + "_COMPRESSED");
        resource.open(QIODevice::ReadOnly);
    }

    Q_ASSERT_X(resource.isOpen(), "Bundle::loadResource", QString("Missing resource file " + resource.fileName()).toUtf8().constData());

    std::vector<char> data(resource.size());
    resource.read(data.data(), data.size());
    resource.close();

    return data;
}
