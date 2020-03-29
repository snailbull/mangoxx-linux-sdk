#include "bitmap.h"
#include <string.h>
#include <stdlib.h>

BITMAPINFO *bmp_create_header(int w, int h, int bitperpix)
{
	BITMAPINFO *pbitmap  = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFO));
	
	memset(pbitmap, sizeof(BITMAPINFO), 0);
	pbitmap->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//信息头大小
    pbitmap->bmiHeader.biWidth = w;     //bmp的宽度
    pbitmap->bmiHeader.biHeight = h;    //bmp的高度
    pbitmap->bmiHeader.biPlanes = 1;    //恒为1
	pbitmap->bmiHeader.biBitCount = bitperpix;
	if (bitperpix == 16)
	{
		// 16bit
		pbitmap->bmiHeader.biCompression = BI_BITFIELDS;//每个象素的比特由指定的掩码决定。
		pbitmap->bmiHeader.biSizeImage = w*h*2+2;//bmp数据区大小
	}
	else
	{
		// 24bit
    	pbitmap->bmiHeader.biCompression = BI_RGB;
		pbitmap->bmiHeader.biSizeImage = 0;//bmp数据区大小
	}
    pbitmap->bmiHeader.biXPelsPerMeter = 0x130b;	//2835, 72 DPI
    pbitmap->bmiHeader.biYPelsPerMeter = 0x130b;	//2835, 72 DPI
    pbitmap->bmiHeader.biClrUsed = 0;
    pbitmap->bmiHeader.biClrImportant = 0;

    pbitmap->bmfHeader.bfType = ((uint16_t)'M'<<8)|'B';//BM格式标志
	if (bitperpix == 16)
	{
		// 16bit, RGB565
		pbitmap->RGB_MASK[0] = 0x0000F800;//红色掩码
		pbitmap->RGB_MASK[1] = 0x000007E0;//绿色掩码
		pbitmap->RGB_MASK[2] = 0x0000001F;//蓝色掩码
		pbitmap->RGB_MASK[3] = 0x00000000;//alpha

		pbitmap->bmfHeader.bfSize = sizeof(BITMAPINFO) + pbitmap->bmiHeader.biSizeImage;
		pbitmap->bmfHeader.bfOffBits = sizeof(BITMAPINFO); //到数据区的偏移
	}
	else
	{
		// 24bit
		pbitmap->bmfHeader.bfSize = sizeof(BITMAPINFO) - 16 + w*h*3;
		pbitmap->bmfHeader.bfOffBits = sizeof(BITMAPINFO) - 16; //到数据区的偏移
	}
	
	return pbitmap;
}
