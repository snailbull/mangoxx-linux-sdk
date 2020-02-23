// jpge.h - C++ class for JPEG compression.
// Public domain, Rich Geldreich <richgel99@gmail.com>
// Alex Evans: Added RGBA support, linear memory allocator.
#ifndef JPEG_ENCODER_H
#define JPEG_ENCODER_H

#include <stdint.h>
#include <stdbool.h>

// JPEG chroma subsampling factors. Y_ONLY (grayscale images) and H2V2 (color images) are the most common.
enum { Y_ONLY = 0, H1V1 = 1, H2V1 = 2, H2V2 = 3 };
enum { JPGE_OUT_BUF_SIZE = 512 };
typedef bool (*jpg_out_cb)(const void* data, int len);

// Lower level jpeg_encoder class - useful if more control is needed than the above helper functions.
struct jpeg_encoder {
	jpg_out_cb m_pPut_buf;

	// Quality: 1-100, higher is better. Typical values are around 50-95.
	int m_quality;

	// 0 = Y (grayscale) only
	// 1 = H1V1 subsampling (YCbCr 1x1x1, 3 blocks per MCU)
	// 2 = H2V1 subsampling (YCbCr 2x1x1, 4 blocks per MCU)
	// 3 = H2V2 subsampling (YCbCr 4x1x1, 6 blocks per MCU-- very common)
	int m_subsampling;

	uint8_t m_num_components;
	uint8_t m_comp_h_samp[3], m_comp_v_samp[3];
	int m_image_x, m_image_y, m_image_bpp, m_image_bpl;
	int m_image_x_mcu, m_image_y_mcu;
	int m_image_bpl_xlt, m_image_bpl_mcu;
	int m_mcus_per_row;
	int m_mcu_x, m_mcu_y;
	uint8_t *m_mcu_lines[16];
	uint8_t m_mcu_y_ofs;
	int32_t m_sample_array[64];
	int16_t m_coefficient_array[64];

	int m_last_dc_val[3];
	uint8_t m_out_buf[JPGE_OUT_BUF_SIZE];
	uint8_t *m_pOut_buf;
	uint32_t m_out_buf_left;
	uint32_t m_bit_buffer;
	uint32_t m_bits_in;
	uint8_t m_pass_num;
	bool m_all_stream_writes_succeeded;
};

// Initializes the compressor.
// pStream: The stream object to use for writing compressed data.
// params - Compression parameters structure, defined above.
// width, height  - Image dimensions.
// channels - May be 1, or 3. 1 indicates grayscale, 3 indicates RGB source data.
// Returns false on out of memory or if a stream write fails.
bool jpge_init(struct jpeg_encoder *jpge, jpg_out_cb pPut_buf, int width, int height, int src_channels, int quality, int subsampling);

// Call this method with each source scanline.
// width * src_channels bytes per scanline is expected (RGB or Y format).
// You must call with NULL after all scanlines are processed to finish compression.
// Returns false on out of memory or if a stream write fails.
bool jpge_process_scanline(struct jpeg_encoder *jpge, const void* pScanline);

// Deinitializes the compressor, freeing any allocated memory. May be called at any time.
void jpge_deinit(struct jpeg_encoder *jpge);

#endif // JPEG_ENCODER
