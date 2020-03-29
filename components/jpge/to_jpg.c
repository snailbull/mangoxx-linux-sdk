// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "img_converters.h"
#include "jpge.h"
#include "yuv.h"
#include "jpge_debug.h"


static void convert_line_format(uint8_t * src, pixformat_t format, uint8_t * dst, size_t width, size_t in_channels, size_t line)
{
    int i=0, o=0, l=0;
    if(format == PIXFORMAT_GRAYSCALE) {
        memcpy(dst, src + line * width, width);
    } else if(format == PIXFORMAT_RGB888) {
        l = width * 3;
        src += l * line;
        for(i=0; i<l; i+=3) {
            dst[o++] = src[i+2];
            dst[o++] = src[i+1];
            dst[o++] = src[i];
        }
    } else if(format == PIXFORMAT_RGB565) {
        l = width * 2;
        src += l * line;
        for(i=0; i<l; i+=2) {
            dst[o++] = src[i+1] & 0xF8;
            dst[o++] = (src[i+1] & 0x07) << 5 | (src[i] & 0xE0) >> 3;
            dst[o++] = (src[i] & 0x1F) << 3;
        }
    } else if(format == PIXFORMAT_YUV422) {
        uint8_t y0, y1, u, v;
        uint8_t r, g, b;
        l = width * 2;
        src += l * line;
        for(i=0; i<l; i+=4) {
            y0 = src[i];
            u = src[i+1];
            y1 = src[i+2];
            v = src[i+3];

            yuv2rgb(y0, u, v, &r, &g, &b);
            dst[o++] = r;
            dst[o++] = g;
            dst[o++] = b;

            yuv2rgb(y1, u, v, &r, &g, &b);
            dst[o++] = r;
            dst[o++] = g;
            dst[o++] = b;
        }
    }
}

bool convert_image(uint8_t *src, uint16_t width, uint16_t height, pixformat_t format, uint8_t quality, jpg_out_cb put_buf)
{
    int num_channels = 3;
    int subsampling = H2V2;

    if(format == PIXFORMAT_GRAYSCALE) {
        num_channels = 1;
        subsampling = Y_ONLY;
    }

    if(!quality) {
        quality = 1;
    } else if(quality > 100) {
        quality = 100;
    }

    struct jpeg_encoder dst_image;
	
	printf("sizeof(jpeg_encoder):%d\n", sizeof(struct jpeg_encoder));

    if (!jpge_init(&dst_image, put_buf, width, height, num_channels, quality, subsampling)) {
        ESP_LOG("JPG encoder init failed");
        return false;
    }

    uint8_t* line = (uint8_t*)malloc(width * num_channels);
    if(!line) {
        ESP_LOG("Scan line malloc failed");
        return false;
    }

    int i;
    for (i = 0; i < height; i++) {
        convert_line_format(src, format, line, width, num_channels, i);
        if (!jpge_process_scanline(&dst_image, line)) {
            ESP_LOG("JPG process line %u failed", i);
            free(line);
            return false;
        }
    }
    free(line);

    if (!jpge_process_scanline(&dst_image, NULL)) {
        ESP_LOG("JPG image finish failed");
        return false;
    }
    jpge_deinit(&dst_image);
    return true;
}

struct jpg_buf{
	uint8_t *buf;
	int size;
	int cnt;
};
static struct jpg_buf s_jpg_buf;
bool put_buf(const void* pBuf, int len)
{
	if (!pBuf) {
		//end of image
		return true;
	}
	if (len > (s_jpg_buf.size - s_jpg_buf.cnt)) {
		ESP_LOG("JPG output overflow: %d bytes", len - (s_jpg_buf.size - s_jpg_buf.cnt));
		len = s_jpg_buf.size - s_jpg_buf.cnt;
	}
	if (len) {
		memcpy(s_jpg_buf.buf + s_jpg_buf.cnt, pBuf, len);
		s_jpg_buf.cnt += len;
	}
	return true;
}

bool fmt2jpg(uint8_t *src, size_t src_len, uint16_t width, uint16_t height, pixformat_t format, uint8_t quality, uint8_t ** out, size_t * out_len)
{
	s_jpg_buf.size = 1024*1024;
	s_jpg_buf.cnt = 0;
    s_jpg_buf.buf = (uint8_t *)malloc(s_jpg_buf.size);
    if(s_jpg_buf.buf == NULL) {
        ESP_LOG("JPG buffer malloc failed");
        return false;
    }
    
    if(!convert_image(src, width, height, format, quality, put_buf)) {
        free(s_jpg_buf.buf);
		s_jpg_buf.buf = 0;
        return false;
    }

    *out = s_jpg_buf.buf;
    *out_len = s_jpg_buf.cnt;
    return true;
}

bool frame2jpg(camera_fb_t * fb, uint8_t quality, uint8_t ** out, size_t * out_len)
{
    return fmt2jpg(fb->buf, fb->len, fb->width, fb->height, fb->format, quality, out, out_len);
}
