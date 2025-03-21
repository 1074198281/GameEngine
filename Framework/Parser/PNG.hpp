#pragma once
#include <cstdio>
#include <iostream>
#include <string>
#include <cassert>
#include <queue>
#include <algorithm>
//#include "config.h"
#include "IImageParser.hpp"
#include "portable.hpp"
#include "zlib.h"
#include "utility.hpp"

namespace My {
#pragma pack(push, 1)
    struct PNG_FILEHEADER {
        uint64_t Signature;
    };

    ENUM(PNG_CHUNK_TYPE) {
        IHDR = "IHDR"_u32,
        PLTE = "PLTE"_u32,
        IDAT = "IDAT"_u32,
        IEND = "IEND"_u32
    };

    static std::ostream& operator<<(std::ostream& out, PNG_CHUNK_TYPE type)
    {
        int32_t n = static_cast<int32_t>(type);
        n = endian_net_unsigned_int<int32_t>(n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof(int32_t); i++) {
            out << *c++;
        }

        return out;
    }

    struct PNG_CHUNK_HEADER {
        uint32_t        Length;
        PNG_CHUNK_TYPE  Type;
    };

    struct PNG_IHDR_HEADER : PNG_CHUNK_HEADER {
        uint32_t        Width;
        uint32_t        Height;
        uint8_t         BitDepth;
        uint8_t         ColorType;
        uint8_t         CompressionMethod;
        uint8_t         FilterMethod;
        uint8_t         InterlaceMethod;
    };

    struct PNG_PLTE_HEADER : PNG_CHUNK_HEADER {
        Vector3Type<uint8_t>* pEntries;
    };
#pragma pack(pop)

    /* report a zlib or i/o error */
    static void zerr(int ret)
    {
        fputs("zpipe: ", stderr);
        switch (ret) {
        case Z_ERRNO:
            if (ferror(stdin))
                fputs("error reading stdin\n", stderr);
            if (ferror(stdout))
                fputs("error writing stdout\n", stderr);
            break;
        case Z_STREAM_ERROR:
            fputs("invalid compression level\n", stderr);
            break;
        case Z_DATA_ERROR:
            fputs("invalid or incomplete deflate data\n", stderr);
            break;
        case Z_MEM_ERROR:
            fputs("out of memory\n", stderr);
            break;
        case Z_VERSION_ERROR:
            fputs("zlib version mismatch!\n", stderr);
        }
    }

    class PngParser : __implements ImageParser
    {
    protected:
        uint16_t m_Width;
        uint16_t m_Height;
        uint8_t  m_BitDepth;
        uint8_t  m_ColorType;
        uint8_t  m_CompressionMethod;
        uint8_t  m_FilterMethod;
        uint8_t  m_InterlaceMethod;
        size_t   m_ScanLineSize;
        uint8_t  m_BytesPerPixel;

    public:
        virtual Image Parse(Buffer& buf)
        {
            Image img;
            img.data = nullptr;
            bool bEnd = false;

            uint8_t* pData = buf.GetData();
            uint8_t* pDataEnd = buf.GetData() + buf.GetDataSize();

            const PNG_FILEHEADER* pFileHeader = reinterpret_cast<const PNG_FILEHEADER*>(pData);
            pData += sizeof(PNG_FILEHEADER);
            if (pFileHeader->Signature == endian_net_unsigned_int((uint64_t)0x89504E470D0A1A0A)) {
                std::cout << "Asset is PNG file" << std::endl;
                while (pData < pDataEnd)
                {
                    const PNG_CHUNK_HEADER* pChunkHeader = reinterpret_cast<const PNG_CHUNK_HEADER*>(pData);
                    PNG_CHUNK_TYPE type = static_cast<PNG_CHUNK_TYPE>(endian_net_unsigned_int(static_cast<uint32_t>(pChunkHeader->Type)));

                    std::cout << "============================" << std::endl;

                    switch (type)
                    {
                    case PNG_CHUNK_TYPE::IHDR:
                    {
                        std::cout << "IHDR (Image Header)" << std::endl;
                        std::cout << "----------------------------" << std::endl;
                        const PNG_IHDR_HEADER* pIHDRHeader = reinterpret_cast<const PNG_IHDR_HEADER*>(pData);
                        m_Width = endian_net_unsigned_int(pIHDRHeader->Width);
                        m_Height = endian_net_unsigned_int(pIHDRHeader->Height);
                        m_BitDepth = pIHDRHeader->BitDepth;
                        m_ColorType = pIHDRHeader->ColorType;
                        m_CompressionMethod = pIHDRHeader->CompressionMethod;
                        m_FilterMethod = pIHDRHeader->FilterMethod;
                        m_InterlaceMethod = pIHDRHeader->InterlaceMethod;

                        switch (m_ColorType)
                        {
                        case 0:  // grayscale
                            m_BytesPerPixel = (m_BitDepth + 7) >> 3;
                            break;
                        case 2:  // rgb true color
                            m_BytesPerPixel = (m_BitDepth * 3) >> 3;
                            break;
                        case 3:  // indexed
                            m_BytesPerPixel = (m_BitDepth + 7) >> 3;
                            break;
                        case 4:  // grayscale with alpha
                            m_BytesPerPixel = (m_BitDepth * 2) >> 3;
                            break;
                        case 6:
                            m_BytesPerPixel = (m_BitDepth * 4) >> 3;
                            break;
                        default:
                            std::cout << "Unkown Color Type: " << m_ColorType << std::endl;
                            assert(0);
                        }

                        m_ScanLineSize = m_BytesPerPixel * m_Width;


                        img.Width = m_Width;
                        img.Height = m_Height;
                        img.bitcount = 32; // currently we fixed at RGBA for rendering
                        img.pitch = (img.Width * (img.bitcount >> 3) + 3) & ~3u; // for GPU address alignment
                        img.data_size = img.pitch * img.Height;
                        img.data = new uint8_t[img.data_size];

                        std::cout << "Width: " << m_Width << std::endl;
                        std::cout << "Height: " << m_Height << std::endl;
                        std::cout << "Bit Depth: " << (int)m_BitDepth << std::endl;
                        std::cout << "Color Type: " << (int)m_ColorType << std::endl;
                        std::cout << "Compression Method: " << (int)m_CompressionMethod << std::endl;
                        std::cout << "Filter Method: " << (int)m_FilterMethod << std::endl;
                        std::cout << "Interlace Method: " << (int)m_InterlaceMethod << std::endl;
                    }
                    break;
                    case PNG_CHUNK_TYPE::PLTE:
                    {
                        std::cout << "PLTE (Palette)" << std::endl;
                        std::cout << "----------------------------" << std::endl;
                        const PNG_PLTE_HEADER* pPLTEHeader = reinterpret_cast<const PNG_PLTE_HEADER*>(pData);
                        for (auto i = 0; i < endian_net_unsigned_int(pPLTEHeader->Length) / sizeof(*pPLTEHeader->pEntries); i++)
                        {
#if DUMP_DETAILS
                            std::cout << "Entry " << i << ": " << pPLTEHeader->pEntries[i] << std::endl;
#endif
                        }
                    }
                    break;
                    case PNG_CHUNK_TYPE::IDAT:
                    {
                        std::cout << "IDAT (Image Data Start)" << std::endl;
                        std::cout << "----------------------------" << std::endl;

                        uint32_t compressed_data_size = endian_net_unsigned_int(pChunkHeader->Length);

                        std::cout << "Compressed Data Length: " << compressed_data_size << std::endl;

                        const uint32_t kChunkSize = 256 * 1024;
                        z_stream strm;
                        strm.zalloc = Z_NULL;
                        strm.zfree = Z_NULL;
                        strm.opaque = Z_NULL;
                        strm.avail_in = 0;
                        strm.next_in = Z_NULL;
                        int ret = inflateInit(&strm);
                        if (ret != Z_OK)
                        {
                            std::cout << "[Error] Failed to init zlib" << std::endl;
                            break;
                        }

                        uint8_t* pIn = pData + sizeof(PNG_CHUNK_HEADER);  // point to the start of the input data buffer
                        uint8_t* pOut = reinterpret_cast<uint8_t*>(img.data);  // point to the start of the input data buffer
                        uint8_t* pDecompressedBuffer = new uint8_t[kChunkSize];
                        memset(pDecompressedBuffer, 0, kChunkSize);
                        uint8_t filter_type = 0;
                        int current_row = 0;
                        int current_col = -1;   // -1 means we need read filter type

                        do {
                            uint32_t next_in_size = (compressed_data_size > kChunkSize) ? kChunkSize : compressed_data_size;
                            if (next_in_size == 0) break;
                            compressed_data_size -= next_in_size;
                            strm.next_in = reinterpret_cast<Bytef*>(pIn);
                            strm.avail_in = next_in_size;
                            do {
                                strm.avail_out = kChunkSize; // 256K
                                strm.next_out = static_cast<Bytef*>(pDecompressedBuffer);
                                ret = inflate(&strm, Z_NO_FLUSH);
                                assert(ret != Z_STREAM_ERROR);
                                switch (ret) {
                                case Z_NEED_DICT:
                                case Z_DATA_ERROR:
                                case Z_MEM_ERROR:
                                    zerr(ret);
                                    (void)inflateEnd(&strm);
                                    strm.avail_out = 0;
                                    ret = Z_STREAM_END;
                                    assert(0);
                                    return img;
                                default:
                                    ;
                                }

                                // now we de-filter the data into image
                                uint8_t* p = pDecompressedBuffer;
                                while (p - pDecompressedBuffer < (kChunkSize - strm.avail_out))
                                {
                                    if (current_col == -1)
                                    {
                                        // we are at start of scan line, get the filter type and advance the pointer
                                        filter_type = *p;
                                    }
                                    else
                                    {
                                        //  prediction filter
                                        //  X is current value
                                        //
                                        //  C  B  D
                                        //  A  X

                                        uint8_t A, B, C;
                                        if (current_row == 0) {
                                            B = C = 0;
                                        }
                                        else
                                        {
                                            B = *(pOut + img.pitch * (current_row - 1) + current_col);
                                            C = (current_col < m_BytesPerPixel) ? 0 : *(pOut + img.pitch * (current_row - 1) + current_col - m_BytesPerPixel);
                                        }

                                        A = (current_col < m_BytesPerPixel) ? 0 : *(pOut + img.pitch * current_row + current_col - m_BytesPerPixel);

                                        switch (filter_type) {
                                        case 0:
                                            *(pOut + img.pitch * current_row + current_col) = *p;
                                            break;
                                        case 1:
                                            *(pOut + img.pitch * current_row + current_col) = *p + A;
                                            break;
                                        case 2:
                                            *(pOut + img.pitch * current_row + current_col) = *p + B;
                                            break;
                                        case 3:
                                            *(pOut + img.pitch * current_row + current_col) = *p + (A + B) / 2;
                                            break;
                                        case 4:
                                        {
                                            int _p = A + B - C;
                                            int pa = abs(_p - A);
                                            int pb = abs(_p - B);
                                            int pc = abs(_p - C);
                                            if (pa <= pb && pa <= pc) {
                                                *(pOut + img.pitch * current_row + current_col) = *p + A;
                                            }
                                            else if (pb <= pc) {
                                                *(pOut + img.pitch * current_row + current_col) = *p + B;
                                            }
                                            else {
                                                *(pOut + img.pitch * current_row + current_col) = *p + C;
                                            }
                                        }
                                        break;
                                        default:
                                            std::cout << "[Error] Unknown Filter Type!" << std::endl;
                                            assert(0);
                                        }

                                    }

                                    current_col++;
                                    if (current_col == m_ScanLineSize) {
                                        current_col = -1;
                                        current_row++;
                                    }

                                    p++;
                                }

                            } while (strm.avail_out == 0);

                            pIn += next_in_size; // move pIn to next chunk
                        } while (ret != Z_STREAM_END);

                        (void)inflateEnd(&strm);
                        delete[] pDecompressedBuffer;
                    }
                    break;
                    case PNG_CHUNK_TYPE::IEND:
                    {
                        bEnd = true;
                        std::cout << "IEND (Image Data End)" << std::endl;
                        std::cout << "----------------------------" << std::endl;
                    }
                    break;
                    default:
                    {
                        std::cout << "Ignor Unrecognized Chunk. Marker=" << type << std::endl;
                    }
                    break;
                    }
                    pData += endian_net_unsigned_int(pChunkHeader->Length) + sizeof(PNG_CHUNK_HEADER) + 4/* length of CRC */;
                }
            }
            else {
                std::cout << "File is not a PNG file!" << std::endl;
            }

            assert(bEnd);
            return img;
        }
    };
}


