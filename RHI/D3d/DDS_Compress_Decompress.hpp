/*
* This part mainly contains compress and decompress algorithm on DDS textures
* for now, heres only realization on BC6H decompress
* 
* 
*/

#pragma once

#include <stdint.h>
#include <assert.h>

namespace My {
	namespace DDS_Algorithm {
		
		// BC6H decompress
		// BC6H Block format
		//-------------------------------------------------------------------------------------------------------//
		//| Mode	Partition Indices	Partition			Color Endpoints						Mode Bits		|//
		//|   1				46bits			5bits		75 bits(10.555, 10.555, 10.555)			2 bits (00)		|//
		//|   2				46bits			5bits		75 bits (7666, 7666, 7666)				2 bits (01)		|//
		//|	  3				46bits			5bits		72 bits (11.555, 11.444, 11.444)		5 bits (00010)	|//
		//|   4				46bits			5bits		72 bits (11.444, 11.555, 11.444)		5 bits (00110)	|//
		//|   5				46bits			5bits		72 bits (11.444, 11.444, 11.555)		5 bits (01010)	|//
		//|   6				46bits			5bits		72 bits (9555, 9555, 9555)				5 bits (01110)	|//
		//|   7				46bits			5bits		72 bits (8666, 8555, 8555)				5 bits (10010)	|//
		//|   8				46bits			5bits		72 bits (8555, 8666, 8555)				5 bits (10110)	|//
		//|   9				46bits			5bits		72 bits (8555, 8555, 8666)				5 bits (11010)	|//
		//|   10			46bits			5bits		72 bits (6666, 6666, 6666)				5 bits (11110)	|//
		//|   11			63bits			5bits		60 bits (10.10, 10.10, 10.10)			5 bits (00011)	|//
		//|   12			63bits			5bits		60 bits (11.9, 11.9, 11.9)				5 bits (00111)	|//
		//|   13			63bits			5bits		60 bits (12.8, 12.8, 12.8)				5 bits (01011)	|//
		//|   14			63bits			5bits		60 bits (16.4, 16.4, 16.4)				5 bits (01111)	|//
		//|------------------------------------------------------------------------------------------------------//
		      
		const size_t block_size = 16;	//each block size is 16 bytes or 128 bits

		typedef enum {
			BC6H_MODE_REGION_TYPE_UNKNOWN = 0,
			BC6H_MODE_REGION_TYPE_ONE,
			BC6H_MODE_REGION_TYPE_TWO
		} BC6H_MODE_REGION_TYPE;

		typedef enum {
			BC6H_MODE_TYPE_UNKNOWN = -1,
			BC6H_MODE_TYPE_1  = 0b00,
			BC6H_MODE_TYPE_2  = 0b01,
			BC6H_MODE_TYPE_3  = 0b00010,
			BC6H_MODE_TYPE_4  = 0b00110,
			BC6H_MODE_TYPE_5  = 0b01010,
			BC6H_MODE_TYPE_6  = 0b01110,
			BC6H_MODE_TYPE_7  = 0b10010,
			BC6H_MODE_TYPE_8  = 0b10110,
			BC6H_MODE_TYPE_9  = 0b11010,
			BC6H_MODE_TYPE_10 = 0b11110,
			BC6H_MODE_TYPE_11 = 0b00011,
			BC6H_MODE_TYPE_12 = 0b00111,
			BC6H_MODE_TYPE_13 = 0b01011,
			BC6H_MODE_TYPE_14 = 0b01111,
		} BC6H_MODE_TYPE;

		typedef struct BC6H_Block {
			BC6H_MODE_REGION_TYPE region_type;	// one-region tile or two-region tiles
			BC6H_MODE_TYPE mode_type;			//14 types
			uint32_t partition_index;
			uint8_t Endpt0[3];					// for BC6H only restore rgb and alpha channel is always 1.0, extract three channels is enough
			uint8_t Endpt1[3];
		} BC6H_Block;

		void extract_mode(const uint8_t* block_data, BC6H_Block& block_info)
		{
			uint8_t type_value;
			uint8_t mode_bit;		//mode type defined by the last five or two bits 
			mode_bit = *(block_data + (block_size - 1));
			mode_bit = mode_bit & 0b00011111;
			if ((mode_bit & 0b00011101) == 0b00000001) {
				type_value = mode_bit & 0b00000001;
			}
			else if ((mode_bit & 0b00011110) == 0b00000010) {
				type_value = mode_bit & 0b00000010;
			}
			else {
				type_value = mode_bit;
			}

			BC6H_MODE_TYPE mode_type = BC6H_MODE_TYPE_UNKNOWN;
			BC6H_MODE_REGION_TYPE region_type = BC6H_MODE_REGION_TYPE_UNKNOWN;
			switch (type_value)
			{
			case 0b00:
				mode_type = BC6H_MODE_TYPE_1;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b01:
				mode_type = BC6H_MODE_TYPE_2;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b00010:
				mode_type = BC6H_MODE_TYPE_3;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b00110:
				mode_type = BC6H_MODE_TYPE_4;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b01010:
				mode_type = BC6H_MODE_TYPE_5;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b01110:
				mode_type = BC6H_MODE_TYPE_6;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b10010:
				mode_type = BC6H_MODE_TYPE_7;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b10110:
				mode_type = BC6H_MODE_TYPE_8;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b11010:
				mode_type = BC6H_MODE_TYPE_9;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b11110:
				mode_type = BC6H_MODE_TYPE_10;
				region_type = BC6H_MODE_REGION_TYPE_TWO;
				break;
			case 0b00011:
				mode_type = BC6H_MODE_TYPE_11;
				region_type = BC6H_MODE_REGION_TYPE_ONE;
				break;
			case 0b00111:
				mode_type = BC6H_MODE_TYPE_12;
				region_type = BC6H_MODE_REGION_TYPE_ONE;
				break;
			case 0b01011:
				mode_type = BC6H_MODE_TYPE_13;
				region_type = BC6H_MODE_REGION_TYPE_ONE;
				break;
			case 0b01111:
				mode_type = BC6H_MODE_TYPE_14;
				region_type = BC6H_MODE_REGION_TYPE_ONE;
				break;
			default:
				mode_type = BC6H_MODE_TYPE_UNKNOWN;
				region_type = BC6H_MODE_REGION_TYPE_UNKNOWN;
				assert(false);
				break;
			}
			block_info.mode_type = mode_type;
			block_info.region_type = region_type;
		}

		void extract_endpoints(const void* block_data, BC6H_Block& block_info)
		{
			
		}

		void BC6H_Decompress(const void* pData, size_t datasize, size_t rowsize, size_t width, size_t height)
		{
			const uint8_t* block_data = new uint8_t[block_size];
			size_t offset = 0;
			while (offset < datasize) {
				block_data = reinterpret_cast<const uint8_t*>(pData) + offset;

				BC6H_Block block_info;
				extract_mode(block_data, block_info);

				extract_endpoints(block_data, block_info);
			}
		}

	}
}