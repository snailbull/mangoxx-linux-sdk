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
