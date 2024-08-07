#include "AssetLoader.hpp"

int My::AssetLoader::Initialize()
{
    return 0;
}

void My::AssetLoader::Finalize()
{
    m_strSearchPath.clear();
}

void My::AssetLoader::Tick()
{

}

bool My::AssetLoader::AddSearchPath(const char *path)
{
    std::vector<std::string>::iterator src = m_strSearchPath.begin();

    while (src != m_strSearchPath.end()) {
        if (!(*src).compare(path))
            return true;
        src++;
    }

    m_strSearchPath.push_back(path);
    return true;
}

bool My::AssetLoader::RemoveSearchPath(const char *path)
{
    std::vector<std::string>::iterator src = m_strSearchPath.begin();

    while (src != m_strSearchPath.end()) {
        if (!(*src).compare(path)) {
            m_strSearchPath.erase(src);
            return true;
        }
        src++;
    }

    return true;
}

bool My::AssetLoader::FileExists(const char *filePath)
{
    AssetFilePtr fp = OpenFile(filePath, MY_OPEN_BINARY);
    if (fp != nullptr) {
        CloseFile(fp);
        return true;
    }
    return false;
}

My::AssetFilePtr My::AssetLoader::OpenFile(const char* name, AssetOpenMode mode)
{
    FILE *fp = nullptr;
    // loop N times up the hierarchy, testing at each level
    std::string upPath;
    std::string fullPath;
    for (int32_t i = 0; i < 10; i++) {
        std::vector<std::string>::iterator src = m_strSearchPath.begin();
        bool looping = true;
        while (looping) {
            fullPath.assign(upPath);  // reset to current upPath.
            if (src != m_strSearchPath.end()) {
                fullPath.append(*src);
                fullPath.append("/Asset/");
                src++;
            }
            else {
                fullPath.append("Asset/");
                looping = false;
            }
            fullPath.append(name);
#ifdef DEBUG
            fprintf(stderr, "Trying to open %s\n", fullPath.c_str());
#endif
            switch(mode) {
                case MY_OPEN_TEXT:
                fp = fopen(fullPath.c_str(), "r");
                break;
                case MY_OPEN_BINARY:
                fp = fopen(fullPath.c_str(), "rb");
                break;
            }

            if (fp) {
                if (!m_AssetPath.size()) {
                    size_t off = fullPath.find_last_of('/');
                    m_AssetPath = fullPath.substr(0, off + 1);
                }
                return (AssetFilePtr)fp;
            }
                
        }

        upPath.append("../");
    }

    return nullptr;
}

My::Buffer My::AssetLoader::SyncOpenAndReadText(const char *filePath)
{
    AssetFilePtr fp = OpenFile(filePath, MY_OPEN_TEXT);
    Buffer buff;

    if (fp) {
        size_t length = GetSize(fp);

        uint8_t* data = new uint8_t[length + 1];
        length = fread(data, 1, length, static_cast<FILE*>(fp));
#ifdef DEBUG
        fprintf(stderr, "Read file '%s', %zu bytes\n", filePath, length);
#endif

        data[length] = '\0';
        buff.SetData(data, length + 1);

        CloseFile(fp);
    }
    else {
        fprintf(stderr, "Error opening file '%s'\n", filePath);
    }

    return buff;
}

My::Buffer My::AssetLoader::SyncOpenAndReadBinary(const char *filePath)
{
    AssetFilePtr fp = OpenFile(filePath, MY_OPEN_BINARY);
    Buffer buff;

    if (fp) {
        size_t length = GetSize(fp);

        uint8_t* data = new uint8_t[length];
        fread(data, length, 1, static_cast<FILE*>(fp));
#ifdef DEBUG
        fprintf(stderr, "Read file '%s', %zu bytes\n", filePath, length);
#endif
        buff.SetData(data, length);

        CloseFile(fp);
    }
    else {
        fprintf(stderr, "Error opening file '%s'\n", filePath);
    }

    return buff;
}

void My::AssetLoader::CloseFile(AssetFilePtr& fp)
{
    fclose((FILE*)fp);
    fp = nullptr;
}

size_t My::AssetLoader::GetSize(const AssetFilePtr& fp)
{
    FILE* _fp = static_cast<FILE*>(fp);

    long pos = ftell(_fp);
    fseek(_fp, 0, SEEK_END);
    size_t length = ftell(_fp);
    fseek(_fp, pos, SEEK_SET);

    return length;
}

size_t My::AssetLoader::SyncRead(const AssetFilePtr& fp, Buffer& buf)
{
    size_t sz;

    if (!fp) {
        fprintf(stderr, "null file discriptor\n");
        return 0;
    }

    sz = fread(buf.m_pData, buf.m_szSize, 1, static_cast<FILE*>(fp));


    return sz;
}

int32_t My::AssetLoader::Seek(AssetFilePtr fp, long offset, AssetSeekBase where)
{
    return fseek(static_cast<FILE*>(fp), offset, static_cast<int>(where));
}
