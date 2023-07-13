#pragma once

#include <vector>
#include <string>
#include <ppl.h>
#include <ppltasks.h>

namespace Utility
{

    typedef std::shared_ptr<std::vector<unsigned char> > ByteArray;
    extern ByteArray NullFile;

    // Reads the entire contents of a binary file.  If the file with the same name except with an additional
    // ".gz" suffix exists, it will be loaded and decompressed instead.
    // This operation blocks until the entire file is read.
    ByteArray ReadFileSync(const std::wstring& fileName);

    // Same as previous except that it does not block but instead returns a task.
    concurrency::task<ByteArray> ReadFileAsync(const std::wstring& fileName);

} // namespace Utility
