/*
Reference: https://blog.csdn.net/moonlinux20704/article/details/110739835
*/

#pragma once

#include <iostream>

namespace My {

	const uint32_t RIFF_CHUNK_HEADER_SIZE = 8;
#pragma pack (push, 1)
	struct WAV_CHUNK_HEADER {
		uint8_t chunkID[4];
		uint32_t chunkSize;
		uint8_t format[4];
	};

	struct SUB_CHUNK_HEADER {
		uint8_t subchunkID[4];
		uint32_t subchunkSize;
	};

	struct FORMAT_SUB_CHUNK : SUB_CHUNK_HEADER {
		uint16_t audio_format;
		uint16_t channels;
		uint32_t sample_rate;
		uint32_t data_rate;				// Bytes Per Second
		uint16_t block_align;			// Bytes Per Sample
		uint16_t bits_per_sample;		// Per Sample Data Bits
	};

	struct DATA_SUB_CHUNK : SUB_CHUNK_HEADER {
		
	};
#pragma pack(pop)
}