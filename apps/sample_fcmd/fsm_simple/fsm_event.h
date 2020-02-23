#ifndef _FSM_EVENT_h_
#define _FSM_EVENT_h_


/******************************************************************************
 * 事件定义
 */
enum
{
	STM_ENTRY_SIG = 0,
	STM_EXIT_SIG,

	/*
	 * hal层任务事件
	 */
	HAL_DIGITLED_EVT,
	HAL_BUZZER_EVT,
	HAL_LED_EVT,
	HAL_KEY_EVT,
	HAL_TIMEOUT,

	/*
	 * task1事件
	 */
	A1_EVT,//7
	A2_EVT,
	A3_EVT,
	A4_EVT,

	/*
	 * task2事件
	 */
	B1_EVT,//11
	B2_EVT,//12
	B3_EVT,//13
	B4_EVT,
	B5_EVT,
	B6_EVT,//16
};

/*
 * 任务ID定义
 */
enum
{
	HAL_ID = 0,
	TASK1_ID,
	TASK2_ID,
};

#endif
