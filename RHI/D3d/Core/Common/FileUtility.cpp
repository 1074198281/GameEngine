#include "FileUtility.h"
#include "../D3dCommonDef.h"
#include "../Utility.h"
#include <fstream>
#include <mutex>
#include <memory>
#include <zlib.h> // From NuGet package 

using namespace Utility;

namespace Utility
{
    ByteArray NullFile = std::make_shared<std::vector<unsigned char> > (std::vector<unsigned char>() );
}

ByteArray DecompressZippedFile(std::wstring& fileName );

ByteArray ReadFileHelper(const std::wstring& fileName)
{
    struct _stat64 fileStat;
    int fileExists = _wstat64(fileName.c_str(), &fileStat);
    if (fileExists == -1)
        return NullFile;

    std::ifstream file( fileName, std::ios::in | std::ios::binary );
    if (!file)
        return NullFile;

    Utility::ByteArray byteArray = std::make_shared<std::vector<unsigned char> >( fileStat.st_size );
    file.read( (char*)byteArray->data(), byteArray->size() );
    file.close();

    return byteArray;
}

ByteArray ReadFileHelperEx(std::shared_ptr<std::wstring> fileName)
{
    std::wstring zippedFileName = *fileName + L".gz";
    ByteArray firstTry = DecompressZippedFile(zippedFileName);
    if (firstTry != NullFile)
        return firstTry;

    return ReadFileHelper(*fileName);
}

ByteArray Inflate(ByteArray CompressedSource, int& err, uint32_t ChunkSize = 0x100000 ) 
{
    // Create a dynamic buffer to hold compressed blocks
    std::vector<std::unique_ptr<unsigned char> > blocks;

    z_stream strm  = {};
    strm.data_type = Z_BINARY;
    strm.total_in  = strm.avail_in  = (uInt)CompressedSource->size();
    strm.next_in   = CompressedSource->data();

    err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib

    while (err == Z_OK || err == Z_BUF_ERROR)
    {
        strm.avail_out = ChunkSize;
        strm.next_out = (unsigned char*)malloc(ChunkSize);
        blocks.emplace_back(strm.next_out);
        err = inflate(&strm, Z_NO_FLUSH);
    }

    if (err != Z_STREAM_END) 
    {
        inflateEnd(&strm);
        return NullFile;
    }

    ASSERT(strm.total_out > 0, "Nothing to decompress");

    Utility::ByteArray byteArray = std::make_shared<std::vector<unsigned char> >( strm.total_out );

    // Allocate actual memory for this.
    // copy the bits into that RAM.
    // Free everything else up!!
    void* curDest = byteArray->data();
    size_t remaining = byteArray->size();

    for (size_t i = 0; i < blocks.size(); ++i)
    {
        ASSERT(remaining > 0);

        size_t CopySize = remaining < ChunkSize ? remaining : ChunkSize;

        memcpy(curDest, blocks[i].get(), CopySize);
        curDest = (unsigned char*)curDest + CopySize;
        remaining -= CopySize;
    }

    inflateEnd(&strm);

    return byteArray;
}

ByteArray DecompressZippedFile(std::wstring& fileName )
{
    ByteArray CompressedFile = ReadFileHelper(fileName);
    if (CompressedFile == NullFile)
        return NullFile;

    int error;
    ByteArray DecompressedFile = Inflate(CompressedFile, error);
    if (DecompressedFile->size() == 0)
    {
        Utility::Printf(L"Couldn't unzip file %s:  Error = %d\n", fileName.c_str(), error);
        return NullFile;
    }

    return DecompressedFile;
}

ByteArray Utility::ReadFileSync( const std::wstring& fileName)
{
    return ReadFileHelperEx(std::make_shared<std::wstring>(fileName));
}

concurrency::task<ByteArray> Utility::ReadFileAsync(const std::wstring& fileName)
{
    std::shared_ptr<std::wstring> SharedPtr = std::make_shared<std::wstring>(fileName);
    return concurrency::create_task( [=] { return ReadFileHelperEx(SharedPtr); } );
}
