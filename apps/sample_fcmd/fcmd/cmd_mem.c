#include "fcmd.h"

#include "cmd_mem.h"

/*
 * 显示内存内容
 * buff:需要dump的内存
 * addr:被dump的内存地址
 * elem_cnt:被dump多少个元素
 * elem_size:一个元素有几个字节宽
 */
void md(int addr, int elem_cnt, int elem_size)
{
	int i, j;
	const uint8_t *pcbuf = (uint8_t *)addr;
	const uint8_t *bp;
	const uint16_t *sp;
	const uint32_t *lp;

	int line_elem;//一行多少个元素
	int line_count;//有多少行
	int line_remain;//最后一行剩下多少个元素

	if ((elem_size == 1) || (elem_size == 2) || (elem_size == 4))
		;
	else
	{
		return ;
	}

	line_elem = 16 / elem_size;
	line_count = (elem_cnt + line_elem - 1) / line_elem;//向上取整，不够一行算一行
	line_remain = elem_cnt % line_elem;

	PUTC('\n');

	for (j = 0; j < line_count; j++)
	{
		if (j == (line_count - 1))      /* last line */
		{
			if (line_remain != 0)
			{
				line_elem = line_remain;
			}
		}

		//dump a line, 16 bytes a line
		PRINTF("%08X:", addr);		/* address */

		switch (elem_size)
		{
		case 1://1
			bp = pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %02X", bp[i]);
			}
			break;
		case 2://2
			sp = (uint16_t *)pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %04X", *sp++);
			}
			break;
		case 4://4
			lp = (uint32_t *)pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %08X", lp[i]);
			}
			break;
		}

		PUTC(' ');
		PUTC(' ');
		bp = pcbuf;
		for (i = 0; i < line_elem * elem_size; i++) /* ASCII dump */
		{
			PUTC((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		}

		PUTC('\n');

		addr += line_elem * elem_size;
		pcbuf += line_elem * elem_size;
	}
}

/*
 * 比较俩块内存是否相等
 * mem1:第一块内存地址
 * mem2:第二块
 * elem_cnt:比较多少个元素
 * elem_size:一个元素是几个字节
 */
int cmp(void *mem1, void *mem2, int elem_cnt, int elem_size)
{
	uint32_t	ngood = 0;
	int     	rcode = 0;
	uint8_t		*addr1 = (uint8_t *)mem1, *addr2 = (uint8_t *)mem2;

	while (elem_cnt-- > 0)
	{
		if (elem_size == 4)
		{
			uint32_t word1 = *(uint32_t *)addr1;
			uint32_t word2 = *(uint32_t *)addr2;

			if (word1 != word2)
			{
				PRINTF("word at 0x%08x (0x%08x) "
				       "!= word at 0x%08x (0x%08x)\n",
				       (int)addr1, word1, (int)addr2, word2);
				rcode = 1;
				break;
			}
		}
		else if (elem_size == 2)
		{
			uint16_t hword1 = *(uint16_t *)addr1;
			uint16_t hword2 = *(uint16_t *)addr2;

			if (hword1 != hword2)
			{
				PRINTF("halfword at 0x%08x (0x%04x) "
				       "!= halfword at 0x%08x (0x%04x)\n",
				       (int)addr1, hword1, (int)addr2, hword2);
				rcode = 1;
				break;
			}
		}
		else if (elem_size == 1)
		{
			uint8_t byte1 = *(uint8_t *)addr1;
			uint8_t byte2 = *(uint8_t *)addr2;

			if (byte1 != byte2)
			{
				PRINTF("byte at 0x%08x (0x%02x) "
				       "!= byte at 0x%08x (0x%02x)\n",
				       (int)addr1, byte1, (int)addr2, byte2);
				rcode = 1;
				break;
			}
		}

		ngood++;

		addr1 += elem_size;
		addr2 += elem_size;
	}
	PRINTF("Total of %d %s%s were the same\n",
	       ngood,
	       (elem_size == 4) ? ("word") : ((elem_size == 2) ? "halfword" : "byte"),
	       ngood == 1 ? "" : "s");

	return rcode;
}
#if 0
/*****************************************************************************
 * spi flash的两个命令操作
 */
/*
 * esp8266的spi flash内存显示, 用于串口调试查看flash内容
 * @readaddr flash地址，4字节对齐
 * @elem_cnt  要读取多少元素, 1024
 * @elem_size 一个元素多宽(1,2,4字节)
 *
 * note  sfmd(0, 1024, 1)
 */
int sfmd(uint32_t addr, int elem_cnt, int elem_size)
{
	uint32_t size = (elem_cnt * elem_size) & 0xFFFFFFFC;	// 4的倍数
	uint8_t *pbuf;
	uint8_t status;

	pbuf = (uint8_t *)malloc(size + 4);
	if (pbuf == NULL)
	{
		PRINTF("memory err\n");
		return -1;
	}

	if (HAL_Flash_Open(PRJCONF_IMG_FLASH, 5000) != HAL_OK)
	{
		PRINTF("open %d fail\n", PRJCONF_IMG_FLASH);
		free(pbuf);
		pbuf = 0;
		return -2;
	}
	//read
	status = HAL_Flash_Read(PRJCONF_IMG_FLASH, addr, pbuf, size);
	HAL_Flash_Close(PRJCONF_IMG_FLASH);
	if (status != HAL_OK)
	{
		PRINTF("HAL_Flash_Read err! (#%08x %d)\n", addr, size);
		free(pbuf);
		pbuf = 0;
		return -3;
	}

	//display spi flash memory
	int i, j;
	const uint8_t *pcbuf = (uint8_t *)pbuf;	//读取到的flash数据
	const uint8_t *bp;
	const uint16_t *sp;
	const uint32_t *lp;

	int line_elem;//一行多少个元素
	int line_count;//有多少行
	int line_remain;//最后一行剩下多少个元素

	if ((elem_size == 1) || (elem_size == 2) || (elem_size == 4))
		;
	else
	{
		free(pbuf);
		pbuf = 0;
		return -4;
	}

	line_elem = 16 / elem_size;
	line_count = (elem_cnt + line_elem - 1) / line_elem;//向上取整，不够一行算一行
	line_remain = elem_cnt % line_elem;

//	PUTC('\n');

	for (j = 0; j < line_count; j++)
	{
		if (j == (line_count - 1))      /* last line */
		{
			if (line_remain != 0)
			{
				line_elem = line_remain;
			}
		}

		//dump a line, 16 bytes a line
		PRINTF("%08X:", addr);		/* address */

		switch (elem_size)
		{
		case 1://1
			bp = pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %02X", bp[i]);
			}
			break;
		case 2://2
			sp = (uint16_t *)pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %04X", *sp++);
			}
			break;
		case 4://4
			lp = (uint32_t *)pcbuf;
			for (i = 0; i < line_elem; i++)		/* Hexdecimal dump */
			{
				PRINTF(" %08X", lp[i]);
			}
			break;
		}

		PUTC(' ');
		PUTC(' ');
		bp = pcbuf;
		for (i = 0; i < line_elem * elem_size; i++) /* ASCII dump */
		{
			PUTC((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		}

		PUTC('\n');

		addr += line_elem * elem_size;
		pcbuf += line_elem * elem_size;
	}

	free(pbuf);
	pbuf = 0;

	return 0;
}

/*
 * spi flash内存写入
 * @writeaddr  写入flash的地址, 地址随意，无需四字节对齐
 * @*pbuf   写入的内容
 * @num  写入大小, 大小随意无需四字节对齐

 * bug:pbuf如果没有4字节对齐的话，也会触发异常, 全部采用4096整扇区读写
 *     不再判断要不要擦除
 * 比如spi_flash_write(0x00300000, pbuf, 512)的pbuf没有4字节对齐将会出错
 */
int sfmw(uint32_t writeaddr, uint8_t *pbuf, uint32_t num)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;
	uint16_t i;
	uint8_t *pmen, *pmenalign;
	int ret = 0;
	int status;

	secpos = writeaddr / 4096; //扇区地址 0~511 for w25x16
	secoff = writeaddr % 4096; //偏移地址
	secremain = 4096 - secoff; //扇区剩余空间大小

	if (num <= secremain)
	{
		secremain = num;    //不大于4096个字节
	}
	pmen = (uint8_t *)malloc(4096 + 8);
	if (pmen == NULL)
	{
		return -1;			//内存错误
	}
	pmenalign = (uint8_t *)((((uint32_t)pmen) + 3) & (~3)); //四字节对齐的内存缓冲

	if (HAL_Flash_Open(PRJCONF_IMG_FLASH, 5000) != HAL_OK)
	{
		PRINTF("open %d fail\n", PRJCONF_IMG_FLASH);
		free(pmen);
		pmen = 0;
		return -2;
	}
	while (1)
	{
		status = HAL_Flash_Read(PRJCONF_IMG_FLASH, secpos * 4096, pmenalign, 4096);
		if (status != HAL_OK)
		{
			PRINTF("HAL_Flash_Read err! (#%08x %d)\n", secpos * 4096, 4096);
			free(pbuf);
			pbuf = 0;
			ret = status;
			break;
		}

		status = HAL_Flash_Erase(PRJCONF_IMG_FLASH, FLASH_ERASE_4KB, secpos * 4096, 1);//擦除这个扇区
		if (status != HAL_OK)
		{
			ret = status;
			break;
		}
		for (i = 0; i < secremain; i++)	 //复制用户内容
		{
			pmenalign[i + secoff] = pbuf[i];
		}
		//写入整个扇区
		status = HAL_Flash_Write(PRJCONF_IMG_FLASH, secpos * 4096, pmenalign, 4096);
		if (status != HAL_OK)
		{
			ret = status;
			break;
		}

		if (num == secremain)
		{
			break;    //写入结束
		}
		else                        //写入未结束
		{
			secpos++;               //扇区地址增1
			secoff = 0;             //偏移位置为0
			pbuf += secremain;      //pbuf指针偏移
			writeaddr += secremain; //写地址偏移
			num -= secremain;		//字节数递减
			if (num > 4096)
			{
				secremain = 4096;    //下一个扇区还是写不完
			}
			else
			{
				secremain = num;    //下一个扇区可以写完了
			}
		}
	}
	HAL_Flash_Close(PRJCONF_IMG_FLASH);

	free(pmen);
	pmen = 0;
	return ret;
}

/**
 * spi flash clear清空(擦除)
 * @erase_addr  flash地址(4kbyte align)
 * @erase_mode  4096,32768,65536
 * @num  擦除多少个单元
 *
 */
int sfmc(uint32_t erase_addr, uint32_t erase_mode, uint32_t num)
{
	uint32_t i;
	int ret = 0, status = 0;
	uint32_t addr;

	if ((erase_mode != 4096UL) && (erase_mode != 32768UL) && (erase_mode != 65536UL))
	{
		PRINTF("erase_mode not support!\r\n");
		return -1;
	}
	if (HAL_Flash_Open(PRJCONF_IMG_FLASH, 5000) != HAL_OK)
	{
		PRINTF("open %d fail\n", PRJCONF_IMG_FLASH);
		return -1;
	}
	erase_addr &= ~(erase_mode - 1);	// align down
	for (i = 0; i < num; i++)
	{
		addr = erase_addr + i * erase_mode;
		status = HAL_Flash_Erase(PRJCONF_IMG_FLASH, erase_mode, addr, 1);
		if (status != HAL_OK)
		{
			ret = status;
			break;
		}
		PRINTF("erase sec %d, len %d sec.\r\n", addr / 4096, erase_mode / 4096);
	}
	HAL_Flash_Close(PRJCONF_IMG_FLASH);
	return ret;
}
/**
 * spi flash mem usage,flash扇区使用情况
 * @start_addr 起始地址
 * @sectors  扫描扇区数量, 需自行判断是否flash越界, 有的flash越界后地址会取余flash真实大小, 然后地址又从头开始
 *
 * 扫描flash的对应扇区占用情况4K一个单位  .空闲  X占用
 *                 sector offset(4k)
 * addr      0 1 2 3 4 5 6 7  8 9 A B C D E F
 * 00100000: . X X . X X . X  X . X X . . X X
 * 00110000: . X X . X X . X  X . X X . . X X
 * 00120000: . X X . X X . X  X . X X . . X X
 * 00130000: . X X . X X . X  X . X X . . X X
 * 00140000: . X X . X X . X  X . X X . . X X
 * 00150000: . X X . X X . X  X . X X . . X X
 * 00160000: . X X . X X . X  X . X X . . X X
 * 00170000: . X X . X X . X  X . X X . . X X
 * 00180000: . X X . X X . X  X . X X . . X X
 * 00190000: . X X . X X . X  X . X X . . X X
 * 001A0000: . X X . X X . X  X . X X . . X X
 * 001B0000: . X X . X X . X  X . X X . . X X
 * 001C0000: . X X . X X . X  X . X X . . X X
 * 001D0000: . X X . X X . X  X . X X . . X X
 * 001E0000: . X X . X X . X  X . X X . . X X
 * 001F0000: . X X . X X . X  X . X X . . X X
 */
int sfmu(uint32_t start_addr, uint32_t sectors)
{
	uint8_t *pmem, *pmemalign;
	uint32_t *lp;
	uint32_t secpos;
	uint32_t v;
	uint32_t indicate_addr;
	int status;
	int ret = 0, used = 0;
	int i, j;

	if (HAL_Flash_Open(PRJCONF_IMG_FLASH, 5000) != HAL_OK)
	{
		PRINTF("open %d fail\n", PRJCONF_IMG_FLASH);
		return -1;
	}

	pmem = (uint8_t *)malloc(4096 + 16);
	if (pmem == NULL)
	{
		HAL_Flash_Close(PRJCONF_IMG_FLASH);
		return -1;
	}
	pmemalign = (uint8_t *)((((uint32_t)pmem) + 3) & (~3)); //4byte align up
	start_addr &= ~4095UL;	// align down
	secpos = start_addr / 4096;
	indicate_addr = start_addr;

	PRINTF(	" .-idle X-used    sector offset(4k)\n"
	        "addr      0 1 2 3 4 5 6 7  8 9 A B C D E F");
	for (j = 0; j < sectors; ++j, ++secpos)
	{
		// indicate addr
		if (j == 0)
		{
			PRINTF("\n%08X:", indicate_addr);
		}
		else if ((((j / 8) % 2) == 1) && ((j % 8) == 0))
		{
			PRINTF(" ");
		}
		else if ((j % 16) == 0)
		{
			indicate_addr += 4096 * 16;
			PRINTF("\n%08X:", indicate_addr);
		}

		// read
		status = HAL_Flash_Read(PRJCONF_IMG_FLASH, secpos * 4096, pmemalign, 4096);
		if (status != HAL_OK)
		{
			PRINTF("\flash read err! (#%08x %d)\n", secpos * 4096, 4096);
			ret = status;
			goto EXIT;
		}

		// check 1 sector
		v = 0xFFFFFFFF;
		used = 0;
		lp = (uint32_t *)pmemalign;
		for (i = 0; i < 1024; i++)
		{
			v = v & lp[i];
			if (v != 0xFFFFFFFF)
			{
				used = 1;
				break;
			}
		}

		// output sector usage info
		if (used)
		{
			PRINTF(" X");
		}
		else
		{
			PRINTF(" .");
		}
	}
	PRINTF("\n");
EXIT:
	free(pmem);
	lp = 0;
	pmem = pmemalign = 0;
	HAL_Flash_Close(PRJCONF_IMG_FLASH);

	return ret;
}
#endif
