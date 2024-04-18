#include "WAV.hpp"
#include "geommath.hpp"
#include <random>

using namespace My;

int main()
{
	const uint32_t SAMPLE_RATE = 11025U;
	uint8_t buf[SAMPLE_RATE];

	const uint16_t channels = 1;
	const uint16_t BitsPerSample = 8;		//每个采样存储的bit数，8/16/32

	WAV_CHUNK_HEADER chunk_header = {
		{'R', 'I', 'F', 'F'},
		0,
		{'W', 'A', 'V', 'E'}
	};

	FORMAT_SUB_CHUNK format_chunk = {
		{{'f', 'm', 't', ' '},
		sizeof(FORMAT_SUB_CHUNK) - sizeof(SUB_CHUNK_HEADER)},
		1, // audio format type, 1 means PCM
		channels, // 1 for single channel, 2 for two channels
		SAMPLE_RATE,
		SAMPLE_RATE * channels * BitsPerSample / 8,
		channels * BitsPerSample / (uint16_t)8,
		BitsPerSample
	};

	DATA_SUB_CHUNK data_chunk = {
		{'d', 'a', 't', 'a',
		0}
	};

	FILE* fp;
	/*
	fp = std::fopen("wave_noise.wav", "wb");

	if (fp) {
		// skip file header
		std::fseek(fp, sizeof(chunk_header), SEEK_SET);

		// write format chunk
		std::fwrite(&format_chunk, sizeof(FORMAT_SUB_CHUNK), 1, fp);

		// now generate the wave
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0, 255);
		for (int i = 0; i < SAMPLE_RATE; i++) {
			buf[i] = distribution(generator);  // generates number in the range 0..255
		}

		data_chunk.subchunkSize = SAMPLE_RATE;

		std::fwrite(&data_chunk, sizeof(DATA_SUB_CHUNK), 1, fp);
		std::fwrite(buf, sizeof(uint8_t), SAMPLE_RATE, fp);

		// Now fill the RIFF header with correct chunk size
		chunk_header.chunkSize = std::ftell(fp) - My::RIFF_CHUNK_HEADER_SIZE;
		std::fseek(fp, 0, SEEK_SET);
		// write file header
		std::fwrite(&chunk_header, sizeof(WAV_CHUNK_HEADER), 1, fp);

		// Now close the wave file
		std::fclose(fp);
	}
	

	fp = std::fopen("sine_wave.wav", "wb");

	if (fp) {
		// skip file header
		std::fseek(fp, sizeof(chunk_header), SEEK_SET);

		// write format chunk
		std::fwrite(&format_chunk, sizeof(FORMAT_SUB_CHUNK), 1, fp);

		for (int i = 0; i < SAMPLE_RATE; i++) {
			buf[i] = 0xFF * std::sin(261.6f * 2.0f * PI * i / SAMPLE_RATE);
		}

		data_chunk.subchunkSize = SAMPLE_RATE;

		std::fwrite(&data_chunk, sizeof(DATA_SUB_CHUNK), 1, fp);
		std::fwrite(buf, sizeof(uint8_t), SAMPLE_RATE, fp);

		// Now fill the RIFF header with correct chunk size
		chunk_header.chunkSize = std::ftell(fp) - My::RIFF_CHUNK_HEADER_SIZE;
		std::fseek(fp, 0, SEEK_SET);
		// write file header
		std::fwrite(&chunk_header, sizeof(WAV_CHUNK_HEADER), 1, fp);

		// Now close the wave file
		std::fclose(fp);
	}
	*/

	fp = std::fopen("octave_wave.wav", "wb");

	if (fp) {
		// skip file header
		std::fseek(fp, sizeof(chunk_header), SEEK_SET);

		// write format chunk
		std::fwrite(&format_chunk, sizeof(FORMAT_SUB_CHUNK), 1, fp);

		data_chunk.subchunkSize = SAMPLE_RATE;
		const float oct_frequency[] = { 261.6f, 293.6f, 329.6f, 349.2f, 392.0f, 440.0f, 493.8f, 523.2f };
		for (int oct_index = 0; oct_index < 8; oct_index++) {
			for (int i = 0; i < SAMPLE_RATE; i++) {
				buf[i] = 0xFF * std::sin(oct_frequency[oct_index] * 2.0f * PI * i / SAMPLE_RATE);
			}
			std::fwrite(&data_chunk, sizeof(DATA_SUB_CHUNK), 1, fp);
			std::fwrite(buf, sizeof(uint8_t), SAMPLE_RATE, fp);
		}

		// Now fill the RIFF header with correct chunk size
		chunk_header.chunkSize = std::ftell(fp) - My::RIFF_CHUNK_HEADER_SIZE;
		std::fseek(fp, 0, SEEK_SET);
		// write file header
		std::fwrite(&chunk_header, sizeof(WAV_CHUNK_HEADER), 1, fp);

		// Now close the wave file
		std::fclose(fp);
	}
}