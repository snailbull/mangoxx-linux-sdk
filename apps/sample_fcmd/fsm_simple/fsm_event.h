#ifndef _FSM_EVENT_h_
#define _FSM_EVENT_h_


/******************************************************************************
 * �¼�����
 */
enum
{
	STM_ENTRY_SIG = 0,
	STM_EXIT_SIG,

	/*
	 * hal�������¼�
	 */
	HAL_DIGITLED_EVT,
	HAL_BUZZER_EVT,
	HAL_LED_EVT,
	HAL_KEY_EVT,
	HAL_TIMEOUT,

	/*
	 * task1�¼�
	 */
	A1_EVT,//7
	A2_EVT,
	A3_EVT,
	A4_EVT,

	/*
	 * task2�¼�
	 */
	B1_EVT,//11
	B2_EVT,//12
	B3_EVT,//13
	B4_EVT,
	B5_EVT,
	B6_EVT,//16
};

/*
 * ����ID����
 */
enum
{
	HAL_ID = 0,
	TASK1_ID,
	TASK2_ID,
};

#endif
