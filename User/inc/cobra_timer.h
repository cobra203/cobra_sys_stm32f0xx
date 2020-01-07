#ifndef _COBRA_TIMER_H_
#define _COBRA_TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_list.h>

#define TASK_NAME_SIZE 32
typedef void (*CALLBACK_F) (void *);

typedef enum task_type_e
{
    TMR_ONCE,
    TMR_CYCLICITY,
    TMR_DELAY,
} TASK_TYPE_E;

typedef struct task_info_s
{
	uint8_t				touch	:1;
	uint8_t				active	:1;
	uint8_t				type	:2;
	uint8_t				queue	:1; /* for TMR_CYCLICITY task queue */
	uint8_t						:3;
} TASK_INFO_S;

typedef struct timer_task_s
{
	char					*name;
	volatile TASK_INFO_S	info;
	uint32_t				delay;
	uint32_t				reload;
	CALLBACK_F				callback;
	void					*cb_data;
	LIST_S					list;
} TIMER_TASK_S;

void timer_task_handle(void);

void timer_task_create(TIMER_TASK_S *task, TASK_TYPE_E type,
							uint32_t delay, uint32_t reload, CALLBACK_F cb);

void timer_task_release(TIMER_TASK_S *task);

void delay_ms(uint32_t time);

void timer_init(void);
void timer_itc(int sig);

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_TIMER_H_ */
