#include <string.h>
#include <stdio.h>

#include <cobra_timer.h>
#include <cobra_console.h>
#include <cobra_cmd.h>

/*********************************************************************************/
/* author:  cobra                                                                */
/* version: 2.0.1                                                                */
/* date:    2019.04.09                                                           */
/*********************************************************************************/
#if (LOG_TIMER_LEVEL > LOG_LEVEL_NOT)
#define TIMER_INFO(fm, ...) { \
	console_cmdline_clean(); \
	console("TIMER   : " fm, ##__VA_ARGS__) \
	console_cmdline_restore(); \
}
#else
#define TIMER_INFO(fm, ...)
#endif

#if (LOG_TIMER_LEVEL > LOG_LEVEL_INFO)
#define TIMER_DEBUG(fm, ...) { \
	console_cmdline_clean(); \
	console("TIMER   : " fm, ##__VA_ARGS__) \
	console_cmdline_restore(); \
}
#else
#define TIMER_DEBUG(fm, ...)
#endif

#define TIMER_LOG(level, fm, ...) TIMER_##level(fm, ##__VA_ARGS__)

#if (CBA_PLATFORM == PLATFORM_STM8)
#include <stm8s_timer.h>
#include <stm8s_tim1.h>

#elif (CBA_PLATFORM == PLATFORM_STM32)
#if defined (__STM32F30X)
#include <stm32f30x_misc.h>
#elif defined (__STM32F0XX)
#include <stm32f0xx_tim.h>
#endif

#elif (CBA_PLATFORM == PLATFORM_LINUX)
#include <time.h>
#include <sys/time.h>
#include <signal.h>
static struct itimerval oldtv;

#endif

static TIMER_TASK_S timer_task_head;
static uint8_t	timer_head_lock;


static void _timer_touch_process(TIMER_TASK_S *task)
{
	CALLBACK_F callback = task->callback;
	void *cb_data = task->cb_data;

	if(task->info.type == TMR_ONCE) {
		TIMER_LOG(DEBUG, "task_touch[%d] : %s\n", task->info.type, task->name);
	}

	switch(task->info.type) {
	case TMR_ONCE:
		timer_task_release(task);
		break;

	case TMR_CYCLICITY:
		task->info.queue = 1;
		task->info.touch = CBA_FALSE;
		task->delay = task->reload;
		break;

	default:
		timer_task_release(task);
		return;
	}

	if(callback) {
		callback(cb_data);
	}
}

void timer_task_handle(void)
{
	TIMER_TASK_S *pos, *tmp;

	list_for_each_entry_safe(pos, tmp, &timer_task_head.list, TIMER_TASK_S, list) {
		if(pos->info.touch) {
			if(pos->info.queue) {
				pos->info.queue = 0;
				continue;
			}
			_timer_touch_process(pos);
			break;
		}
	}
}

static void timer_task_dump(void *cmd)
{
	TIMER_TASK_S *pos;
	int i = 0;

	TIMER_LOG(INFO, "============================================================\n");
	list_for_each_entry(pos, &timer_task_head.list, TIMER_TASK_S, list) {
		TIMER_LOG(INFO, "task_list[%02d]: [0x%08x][%d] %s\n", i++, &(pos->list), pos->info.type, pos->name);
	}
	TIMER_LOG(INFO, "============================================================\n");
}
CMD_CREATE(timer, task_dump, timer_task_dump);

void timer_task_create(TIMER_TASK_S *task, TASK_TYPE_E type,
							uint32_t delay, uint32_t reload, CALLBACK_F cb)
{
	task->info.type = type;
	task->info.touch = CBA_FALSE;
	task->delay = delay ? delay : 1;
	task->reload = reload;
	task->callback = cb;

	timer_head_lock = 1;
	list_add_tail(&(task->list), &timer_task_head.list);
	timer_head_lock = 0;

	if(type != TMR_DELAY) {
		TIMER_LOG(DEBUG, "task_create[%d]: %s\n", type, task->name);
	}

	task->info.active = CBA_TRUE;
}

void timer_task_release(TIMER_TASK_S *task)
{
	if(task && task->list.prev && task->list.next && !list_empty_careful(&(task->list))) {
		timer_head_lock = 1;
		list_del(&(task->list));
		timer_head_lock = 0;
		task->info.active = CBA_FALSE;

		if(task->info.type != TMR_DELAY) {
			TIMER_LOG(DEBUG, "task_release[%d]: %s\n", task->info.type, task->name);
		}
	}
}

void delay_ms(uint32_t ms)
{
    TIMER_TASK_S task;

	task.name = "delay_ms";
	task.cb_data = CBA_NULL;
	timer_task_create(&task, TMR_DELAY, ms, 0, NULL);

    while(!task.info.touch);

    timer_task_release(&task);
}

void timer_init(void) /* 1ms timer */
{
#if (CBA_PLATFORM == PLATFORM_STM8)
	TIM1_DeInit();
    TIM1_TimeBaseInit(16,TIM1_COUNTERMODE_DOWN, 948 ,0);
    TIM1_ITConfig(TIM1_IT_UPDATE,ENABLE);
    TIM1_SetCounter(948);
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
    TIM1_Cmd(ENABLE);

#elif (CBA_PLATFORM == PLATFORM_STM32)
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
#if defined (__STM32F30X)
    SysTick_Config(72000 - 1);
#elif defined (__STM32F0XX)
	SysTick_Config(48000 - 1);
#endif

#elif (CBA_PLATFORM == PLATFORM_LINUX)
	signal(SIGALRM, timer_itc);

	struct itimerval itv;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 1000;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);

#endif
	INIT_LIST_HEAD(&timer_task_head.list);

	cmd_register(&cmd_timer_task_dump);

	TIMER_LOG(INFO, "%s ... OK\n", __func__);
}

void timer_itc(int sig)
{
	TIMER_TASK_S *pos;

	if(!timer_head_lock) {
		list_for_each_entry(pos, &timer_task_head.list, TIMER_TASK_S, list) {
			if(pos->info.active && pos->delay) {
				pos->delay--;
				if(!pos->delay) {
					pos->info.touch = CBA_TRUE;
				}
			}
		}
	}
}


