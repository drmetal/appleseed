

#ifndef WAVE_H_
#define WAVE_H_

#include "stream_common.h"

//The sample data must end on an even byte boundary.
//All numeric data fields are in the Intel format of low-high byte ordering.
//8-bit samples are stored as unsigned bytes, ranging from 0 to 255.
//16-bit samples are stored as 2's-complement signed integers, ranging from -32768 to 32767.
//
//For multi-channel data, samples are interleaved between channels, like this:
//
//sample 0 for channel 0
//sample 0 for channel 1
//sample 1 for channel 0
//sample 1 for channel 1
//...
//
//For stereo audio, channel 0 is the left channel and channel 1 is the right.

#define CHUNK_TAG_LENGTH 4

#define RIFF_TAG_RIFF 			{'R','I','F','F'} // "RIFF"
#define RIFF_TAG_WAVE 			{'W','A','V','E'} // "WAVE"
#define RIFF_TAG_FMT  			{'f','m','t',' '} // "fmt "
#define RIFF_TAG_DATA 			{'d','a','t','a'} // "data"
#define RIFF_TAG_FACT 			{'f','a','c','t'} // "fact" wont be used in this system
//#define WAVE_FORMAT_PCM 		0x0001
//#define WAVE_FORMAT_IEEE_FLOAT 	0x0003
//#define WAVE_FORMAT_ALAW 		0x0006
//#define WAVE_FORMAT_MULAW 		0x0007
//#define WAVE_FORMAT_ADPCM		0x0011
//#define WAVE_FORMAT_EXTENSIBLE 	0xFFFE

#define TEMPLATE_FMT_LEN 		16				// length of fmt data (includes fact and any other chunks up to the data chunk)
#define TEMPLATE_RIFF_SIZE 		36   			// total size of file in bytes minus 8
												// this is the offset from the start of the file to the data, assuming no fact section exists in the file.
#define TEMPLATE_FORMAT			WAVE_FORMAT_PCM	// 0x0001 	WAVE_FORMAT_PCM 		PCM
#define TEMPLATE_CHANNELS		2				// 1 = mono, 2 = stereo, ...
#define TEMPLATE_SAMPLERATE		48000 			// 8000 = 8kHz, 44100 = 44.1kHz, ...
#define TEMPLATE_WORD_SIZE		16 				// 8 = 8bit, 16 = 16bit, ...
#define TEMPLATE_BLOCK_ALIGN	((TEMPLATE_CHANNELS*TEMPLATE_WORD_SIZE)/8) // fmt_num_channels*fmt_word_size/8 (bytes per sample over all channels)
#define TEMPLATE_BPS			(TEMPLATE_SAMPLERATE*TEMPLATE_BLOCK_ALIGN) // fmt_sample_rate*fmt_block_align  (bytes per second)
#define TEMPLATE_FACT_LEN 	    0 				// length of fact data is 0bytes
#define TEMPLATE_DATA_LEN 	    0 				// length of data block in bytes

#define WAVE_HEADER_TEMPLATE   	\
{								\
		RIFF_TAG_RIFF,			\
		TEMPLATE_RIFF_SIZE, 	\
		RIFF_TAG_WAVE,			\
		RIFF_TAG_FMT,			\
		TEMPLATE_FMT_LEN,		\
		TEMPLATE_FORMAT,		\
		TEMPLATE_CHANNELS,		\
		TEMPLATE_SAMPLERATE,	\
		TEMPLATE_BPS,			\
		TEMPLATE_BLOCK_ALIGN,	\
		TEMPLATE_WORD_SIZE,		\
		RIFF_TAG_FACT,			\
		TEMPLATE_FACT_LEN,		\
		RIFF_TAG_DATA,			\
		TEMPLATE_DATA_LEN		\
}

typedef enum {
	WAVE_FORMAT_PCM = 0x0001,
	WAVE_FORMAT_IEEE_FLOAT= 0x0003,
	WAVE_FORMAT_ALAW = 0x0006,
	WAVE_FORMAT_MULAW = 0x0007,
	WAVE_FORMAT_ADPCM = 0x0011,
	WAVE_FORMAT_EXTENSIBLE = 0xFFFE
} wave_fmt_t;

typedef struct _riff_header_t riff_header_t;

struct _riff_header_t{
	uint8_t riff[CHUNK_TAG_LENGTH];	// "RIFF"
	uint32_t size;					// total size of file in bytes minus 8
	uint8_t type[CHUNK_TAG_LENGTH];	// "WAVE"
	uint8_t fmt[CHUNK_TAG_LENGTH];	// "fmt "
	uint32_t fmt_len;				// length of fmt data (includes fact and any other chunks up to the data chunk)
	uint16_t fmt_tag;				// 0x0001 	WAVE_FORMAT_PCM 		PCM
									// 0x0003 	WAVE_FORMAT_IEEE_FLOAT 	IEEE float
									// 0x0006 	WAVE_FORMAT_ALAW 		8-bit ITU-T G.711 A-law
									// 0x0007 	WAVE_FORMAT_MULAW 		8-bit ITU-T G.711 �-law
									// 0x0011	WAVE_FORMAT_ADPCM		4-bit IMA ADPCM
									// 0xFFFE 	WAVE_FORMAT_EXTENSIBLE 	Determined by SubFormat
	uint16_t fmt_num_channels;		// 1 = mono, 2 = stereo, ...
	uint32_t fmt_sample_rate;		// 8000 = 8kHz, 44100 = 44.1kHz, ...
	uint32_t fmt_byte_rate;			// fmt_sample_rate*fmt_block_align  (bytes per second)
	uint16_t fmt_block_align;		// fmt_num_channels*fmt_word_size/8 (bytes per sample over all channels)
	uint16_t fmt_word_size;			// 8 = 8bit, 16 = 16bit, ...
	uint8_t fact[CHUNK_TAG_LENGTH];	// "fact"
	uint32_t fact_len;				// length of fact data
	uint8_t data[CHUNK_TAG_LENGTH];	// "data"
	uint32_t data_len;				// length of data block in bytes
};

typedef struct {
	int fdes;
	struct _riff_header_t header;
	logger_t log;
}wav_file_t;

int wav_file_open(wav_file_t* file, const char* filepath);
void wav_file_close(wav_file_t* file);
uint32_t wav_file_read_mix_to_buffer_channel(wav_file_t* file, uint32_t samplecount, int16_t* buffer, uint8_t bufferchannels, int32_t multiply, int32_t divide, int16_t* workarea, uint32_t workarealength);
uint16_t wav_file_get_channels(wav_file_t* file);
uint32_t wav_file_get_data_length(wav_file_t* file);
uint32_t wav_file_get_samplerate(wav_file_t* file);
uint32_t wav_file_get_wordsize_bits(wav_file_t* file);
uint32_t wav_file_get_wordsize_bytes(wav_file_t* file);
wave_fmt_t wav_file_get_format(wav_file_t* file);

#endif // WAVE_H_
