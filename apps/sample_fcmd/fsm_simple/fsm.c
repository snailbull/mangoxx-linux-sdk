/*******************************************************************************
--�¼�����״̬��
--0.01
--zrpeng
--2015-6-13
--
--note:

--history:
	0.01	2015-6-6
	��stm32��������������֧�ֵ͹���__wfeģʽ,��������������CC2540��osal��д�ģ�
	����������״̬����ת������

	2015-11-5  ������esp8266��������.

	����os_send_message, �����ԣ�������stm�м�
*******************************************************************************/
#include "fsm.h"


extern void hal_active(uint8_t e);
extern void task1_active(uint8_t e);
extern void task2_active(uint8_t e);


//״̬��,����ƫ�ƾ���״̬����id��,��������״̬��
static stm_handler tasks_pool[TASKS_CNT] =
{
	hal_active,
	task1_active,
	task2_active
};
static queue_t tasks_queue[TASKS_CNT]; /* ÿ��״̬����Ӧ����Ϣ���� */
static uint8_t stm_ret_value;          /* ״̬������ֵ */
static stm_handler stm_state_temp;     /* ��ʱ״̬������ */
//static uint8_t message_nesting_cnt;    /* ��ϢǶ�׼��� */

//���Ӻ���, ����ϵͳ��һ�ν����¼�ѭ��
static void (*dispatch_hook)(void);
static uint8_t next_dispatch;	/* ����������һ���¼�ѭ�� */

//��ʱ��
static timer_t timer_pool[TIMER_CNT];
//static uint32_t tick_previous;
//static uint32_t sys_tick;

//��Դ
#ifdef POWER_SAVING
static uint32_t power_task_state;		/* 32��λ��֧��32��fsm,�������� */
#endif

/*
 * static function
 */
static void os_power_sleep(void);

/******************************************************************************
*       �¼�ѭ��
*/
void ICACHE_FLASH_ATTR os_dispatch(void)
{
	uint8_t i;

	next_dispatch = 0;
	
	//��ѯ�¼�������
	for (i = 0; i < TASKS_CNT; i++)
	{
		if (tasks_queue[i].used)
		{
			uint8_t e;
			
			PORT_SR_ALLOC();
			PORT_CPU_DISABLE();
			tasks_queue[i].used--;
			e = tasks_queue[i].buf[ tasks_queue[i].head ];
			tasks_queue[i].head++;
			if (tasks_queue[i].head >= QUEUE_SIZE)
			{
				tasks_queue[i].head = 0;
			}
			PORT_CPU_ENABLE();

			(tasks_pool[i])(e);
			if (stm_ret_value == STM_TRAN)
			{
				stm_ret_value = 0;
				(tasks_pool[i])(STM_EXIT_SIG);
				stm_state_temp(STM_ENTRY_SIG);
				tasks_pool[i] = stm_state_temp;
			}
		}
	}
#ifdef POWER_SAVING
	// ��������һ��os_dispatch�ˣ����Կ��ǹ�����
	if (!next_dispatch)
	{
		os_power_sleep();
	}
#endif
}

void ICACHE_FLASH_ATTR os_init_tasks(void)
{
	uint8_t i;
	for (i = 0; i < TASKS_CNT; i++)
	{
		os_post_message(i, STM_ENTRY_SIG);
	}
	for (i = 0; i < TIMER_CNT; i++)
	{
		timer_pool[i].task_id = INVALID_TASK_ID;
	}
}

void ICACHE_FLASH_ATTR os_register_hook(void (*func)(void))
{
	dispatch_hook = func;
}

/******************************************************************************
*   �¼�
*/
uint8_t ICACHE_FLASH_ATTR os_post_message(uint8_t task_id, uint8_t e)
{
	PORT_SR_ALLOC();
	PORT_CPU_DISABLE();
	if (tasks_queue[task_id].used >= QUEUE_SIZE)
	{
		PORT_CPU_ENABLE();
		return QUEUE_FULLED;
	}
	tasks_queue[task_id].buf[ tasks_queue[task_id].tail ] = e;
	tasks_queue[task_id].tail++;
	if (tasks_queue[task_id].tail >= QUEUE_SIZE)
	{
		tasks_queue[task_id].tail = 0;
	}
	tasks_queue[task_id].used++;

	if (!next_dispatch)
	{
		next_dispatch = 1;
		if (dispatch_hook)
		{
			(*dispatch_hook)();
		}
	}
	PORT_CPU_ENABLE();

	return RET_SUCCESS;
}
#if 0
/*
 * notes: ��windows��PostMessage,SendMessageһ����һ�����첽ִ�У�һ����ͬ��ִ��
 *        os_post_message�ǰ���Ϣ�ŵ��������棬����һ����Ϣѭ��ʱȡ����Ϣ��ִ�У�
 *        ��os_send_message��ֱ�ӵ�����Ϣ������������Ϣ����Ϣ��û�о�����Ϣ���е�
 *        os_send_message���ܻ�����ݹ飬���������趨�����ݹ����
 *        ǧ��Ҫ��STM_ENTRY_SIG��STM_EXIT_SIG��ʹ��os_send_message
 */
uint8_t ICACHE_FLASH_ATTR os_send_message(uint8_t task_id, uint8_t e)
{
	message_nesting_cnt++;	/* ++��--Ҫ�ɶԳ��� */
	if (message_nesting_cnt > MESSAGE_NESTING_MAX)
	{
		message_nesting_cnt--;
		return NESTING_FULLED;
	}

	(tasks_pool[task_id])(e);	/* �����﷢���ݹ� */
	if (stm_ret_value == STM_TRAN)
	{
		stm_ret_value = 0;
		(tasks_pool[task_id])(STM_EXIT_SIG);
		stm_state_temp(STM_ENTRY_SIG);
		tasks_pool[task_id] = stm_state_temp;
	}

	message_nesting_cnt--;
	return RET_SUCCESS;
}
#endif


/******************************************************************************
*  ��ʱ��
*/
void ICACHE_FLASH_ATTR os_timer_update(void)
{
	uint8_t i;

	for (i = 0; i < TIMER_CNT; i++)
	{
		if (timer_pool[i].task_id != INVALID_TASK_ID)
		{
			if (timer_pool[i].timeout > 0)
			{
				timer_pool[i].timeout--;
			}
			else
			{
				os_post_message(timer_pool[i].task_id, timer_pool[i].sig);
				if (timer_pool[i].flag & TIMER_RELOAD_FLAG)
				{
					timer_pool[i].timeout = timer_pool[i].reload;
				}
				else
				{
					timer_pool[i].task_id = INVALID_TASK_ID;
				}
			}
		}
	}
}

void ICACHE_FLASH_ATTR os_timer_set(uint8_t task_id, uint8_t sig, uint16_t timeout)
{
	uint8_t i;

	PORT_SR_ALLOC();
	PORT_CPU_DISABLE();
	//���붨ʱ��
	for (i = 0; i < TIMER_CNT; i++)
	{
		if (timer_pool[i].task_id == task_id &&
		        timer_pool[i].sig == sig)		//�Ѵ��ڶ�ʱ��,�޸ĳ�ʱֵ
		{
			timer_pool[i].timeout = timeout;
			break;
		}
		else if (timer_pool[i].task_id == INVALID_TASK_ID)
		{
			timer_pool[i].sig = sig;
			timer_pool[i].timeout = timeout;
			timer_pool[i].task_id = task_id;
			timer_pool[i].flag = 0;
			break;
		}
	}
	PORT_CPU_ENABLE();
}

void ICACHE_FLASH_ATTR os_reload_timer_set(uint8_t task_id, uint8_t sig, uint16_t timeout)
{
	uint8_t i;

	PORT_SR_ALLOC();
	PORT_CPU_DISABLE();
	//���붨ʱ��
	for (i = 0; i < TIMER_CNT; i++)
	{
		if (timer_pool[i].task_id == task_id &&
		        timer_pool[i].sig == sig)		//�Ѵ��ڶ�ʱ��,ֱ���޸��ظ���ʱֵ,��һ����Ч
		{
			timer_pool[i].reload = timeout;
			break;
		}
		else if (timer_pool[i].task_id == INVALID_TASK_ID)
		{
			timer_pool[i].sig = sig;
			timer_pool[i].timeout = timeout;
			timer_pool[i].reload = timeout;
			timer_pool[i].task_id = task_id;
			timer_pool[i].flag |= TIMER_RELOAD_FLAG;
			break;
		}
	}
	PORT_CPU_ENABLE();
}

void ICACHE_FLASH_ATTR os_timer_del(uint8_t task_id, uint8_t sig)
{
	uint8_t i;

	PORT_SR_ALLOC();
	PORT_CPU_DISABLE();
	//ɾ����ʱ��
	for (i = 0; i < TIMER_CNT; i++)
	{
		if (timer_pool[i].task_id == task_id &&
		        timer_pool[i].sig == sig)		//ɾ�����ڶ�ʱ��
		{
			timer_pool[i].task_id = INVALID_TASK_ID;
			timer_pool[i].flag = 0;
			break;
		}
	}
	PORT_CPU_ENABLE();
}

#ifdef POWER_SAVING
/******************************************************************************
*   ��Դ�͹���
*/
static void ICACHE_FLASH_ATTR os_power_sleep(void)
{
	if (power_task_state == 0)
	{
		;
	}
}
void ICACHE_FLASH_ATTR os_power_set(uint8_t task_id, uint8_t state)
{
	if (state == POWER_HOLD)
	{
		power_task_state |= 1 << task_id;
	}
	else if (state == POWER_SLEEP)
	{
		power_task_state &= ~(1 << task_id);
	}
}
#endif

/******************************************************************************
*  ����״̬����ת
*/
void ICACHE_FLASH_ATTR os_stm_tran(stm_handler state)
{
	stm_ret_value = STM_TRAN;
	stm_state_temp = state;
}

