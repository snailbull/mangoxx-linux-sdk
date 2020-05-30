#include "fsm.h"


static stm_handler state[TASKS_CNT];
static stm_handler temp[TASKS_CNT];
static uint8_t stm_ret_value[TASKS_CNT];
static queue_t tasks_queue[TASKS_CNT][8];
static uint8_t message_nesting_cnt[TASKS_CNT];

//���Ӻ���, ����ϵͳ��һ�ν����¼�ѭ��
static void (*dispatch_hook)(void);
static uint8_t next_dispatch;	/* ����������һ���¼�ѭ�� */

//��ʱ��
static timer_t timer_pool[TIMER_CNT];
//static uint32_t tick_previous;
//static uint32_t sys_tick;

void os_dispatch(void)
{
	uint8_t i;
	uint8_t e;
	next_dispatch = 0;
	for (i = 0; i < TASKS_CNT; i++)
	{
		if (tasks_queue[i].used)
		{
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

			(state[i])(e);
			if (stm_ret_value == STM_TRAN)
			{
				stm_ret_value = 0;
				(state[i])(STM_EXIT_SIG);
				temp(STM_ENTRY_SIG);
				state[i] = temp;
			}
		}
	}
#ifdef POWER_SAVING
	if (!next_dispatch)
	{
		os_power_sleep();
	}
#endif
}

void os_init_tasks(void)
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

void os_register_hook(void (*func)(void))
{
	dispatch_hook = func;
}

/******************************************************************************
*   �¼�
*/
uint8_t os_post_message(uint8_t task_id, uint8_t e)
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

/*
 * notes: ��windows��PostMessage,SendMessageһ����һ�����첽ִ�У�һ����ͬ��ִ��
 *        os_post_message�ǰ���Ϣ�ŵ��������棬����һ����Ϣѭ��ʱȡ����Ϣ��ִ�У�
 *        ��os_send_message��ֱ�ӵ�����Ϣ������������Ϣ����Ϣ��û�о�����Ϣ���е�
 *        os_send_message���ܻ�����ݹ飬���������趨�����ݹ����
 *        ǧ��Ҫ��STM_ENTRY_SIG��STM_EXIT_SIG��ʹ��os_send_message
 */
uint8_t os_send_message(uint8_t task_id, uint8_t e)
{
	message_nesting_cnt++;	/* ++��--Ҫ�ɶԳ��� */
	if (message_nesting_cnt > MESSAGE_NESTING_MAX)
	{
		message_nesting_cnt--;
		return NESTING_FULLED;
	}

	(state[task_id])(e);	/* �����﷢���ݹ� */
	if (stm_ret_value == STM_TRAN)
	{
		stm_ret_value = 0;
		(state[task_id])(STM_EXIT_SIG);
		temp(STM_ENTRY_SIG);
		state[task_id] = temp;
	}

	message_nesting_cnt--;
	return RET_SUCCESS;
}

void os_timer_update(void)
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

void os_timer_set(uint8_t task_id, uint8_t sig, uint16_t timeout)
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

void os_reload_timer_set(uint8_t task_id, uint8_t sig, uint16_t timeout)
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

void os_timer_del(uint8_t task_id, uint8_t sig)
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

void os_stm_tran(stm_handler state)
{
	stm_ret_value = STM_TRAN;
	temp = state;
}
