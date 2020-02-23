/******************************************************************************
--文件名  : bmp.h
--版本    : V1.0
--作者    : 周瑞鹏
--创建时间: 2013-12-30
--注明    : bmp文件头14字节，bmp信息头40字节，调色板(2色，16色有，24位真彩色没有调色板)
		    实际的位图数据:24色 b00000000 00000000 00000000  3字节表示一个像素 BGR排列的
			              16色  b0000 0000 4位表示一个像素 左高位
						        像素0 像素1
		    比如16色位图的调色板为
				B  G  R
				00 00 00 00 黑
				00 00 80 00 
				00 80 00 00
				00 80 80 00
				80 00 00 00
				80 00 80 00
				80 80 00 00
				80 80 80 00 灰
				C0 C0 C0 00
				00 00 FF 00 红
				00 FF 00 00 绿
				00 FF FF 00
				FF 00 00 00 蓝
				FF 00 FF 00
				FF FF 00 00
				FF FF FF 00 白
            
--注意：
    BITMAPINFOHEADER biSizeImage=w*h*2+2   尾部额外增加2byte!(00 00)
	gcc不能做到1byte align!!
*******************************************************************************
--修改历史：
******************************************************************************/
#ifndef _BITMAP_H_
#define _BITMAP_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct//bmp文件头 14字节
{
    uint16_t  bfType ;     //文件类型.(0x424D'BM')
    uint32_t  bfSize ;     //文件大小,(4byte)
    uint16_t  bfReserved1 ;//保留
    uint16_t  bfReserved2 ;//保留
    uint32_t  bfOffBits ;  //实际位图数据开始的偏移量
}__attribute__((packed,aligned(1))) BITMAPFILEHEADER;

typedef struct//bmp信息头(40字节)
{
    uint32_t biSize ;            //BITMAPINFOHEADER结构所需要的字数。(28 00 00 00)
    long biWidth ;          //图象的宽度(78 00 00 00)120 (必须为4的整数倍，向上补齐,121->124)
    long biHeight ;         //图象的高度(a0 00 00 00)160
    uint16_t  biPlanes ;         //为目标设备说明位面数，总是被设为1
    uint16_t  biBitCount ;       //多少位的颜色，其值为1、4、8、16、24、或32
    uint32_t biCompression ;     //图象数据压缩类型。其值是下述值之一：
    //BI_RGB：没有压缩；
    //BI_RLE8：每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；
    //BI_RLE4：每个象素4比特的RLE压缩编码，压缩格式由2字节组成
    //BI_BITFIELDS：每个象素的比特由指定的掩码决定。
    uint32_t biSizeImage ;       //位图数据的占用总字节数。BI_RGB时，为0.
    long  biXPelsPerMeter ; //设备水平分辨率，用象素/米表示
    long  biYPelsPerMeter ; //设备垂直分辨率，用象素/米表示
    uint32_t biClrUsed ;         //位图实际使用的颜色数。为0表示用到的颜色数为2^biBitCount
    uint32_t biClrImportant ;    //指定本图象中重要的颜色数，为0，表示都重要。
}__attribute__((packed, aligned(1))) BITMAPINFOHEADER;

typedef struct//调色板 sizeof(RGBQUAD)*biClrUsed  
{
    uint8_t rgbBlue ;    //蓝色分量 0~256
    uint8_t rgbGreen ;   //绿色分量
    uint8_t rgbRed ;     //红色分量
    uint8_t rgbReserved ;//保留，为0
}__attribute__((packed, aligned(1))) RGBQUAD;

typedef struct//位图信息头
{
    BITMAPFILEHEADER bmfHeader;//14
    BITMAPINFOHEADER bmiHeader;//40
//	union{
		uint32_t RGB_MASK[4];       //存放RGBA掩码. 注意：还有一个alpha通道掩码为0
//		RGBQUAD bmiColors[2^biBitCount];
//	};
}__attribute__((packed, aligned(1))) BITMAPINFO;
typedef RGBQUAD * LPRGBQUAD;//彩色表

//图象数据压缩的类型
#define BI_RGB          0  //没有压缩.RGB 5,5,5.
#define BI_RLE8         1  //每个象素8比特的RLE压缩编码，压缩格式由2字节组成(重复象素计数和颜色索引)；
#define BI_RLE4         2  //每个象素4比特的RLE压缩编码，压缩格式由2字节组成
#define BI_BITFIELDS    3  //每个象素的比特由指定的掩码决定。

BITMAPINFO *bmp_create_header(int w, int h, int bitperpix);


#ifdef __cplusplus
}
#endif

#endif
