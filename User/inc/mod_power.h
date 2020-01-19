#ifndef _MOD_POWER_H_
#define _MOD_POWER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_sys.h>
#include <cobra_button.h>
#include <cobra_event.h>
#include <cobra_timer.h>
#include <cobra_led.h>

#define KEY_POWER_PORT			GPIO_PORT(KEY_POWER_PORTx)
#define KEY_POWER_PIN			GPIO_PIN(KEY_POWER_PINx)
#define KEY_POWER_EXTI_PORT		EXTI_PORT(KEY_POWER_PORTx)
#define KEY_POWER_EXIT_SOURCE	EXIT_PIN(KEY_POWER_PINx)
#define KEY_POWER_EXTI_LINE		EXTI_LINE(KEY_POWER_PINx)
#define KEY_POWER_IRQ			EXTI_IRQ(KEY_POWER_IRQ_RANGE)

#define POWER_CTRL_PORT			GPIO_PORT(POWER_CTRL_PORTx)
#define POWER_CTRL_PIN			GPIO_PIN(POWER_CTRL_PINx)

typedef struct power_status_s
{
	uint8_t		enable	:1;
	uint8_t				:7;
} POWER_STATUS_S;

typedef struct mod_power_s
{
	SYS_STATUS_S		*sys_status;
	POWER_STATUS_S		status;
	LED_S				power_ctrl;
    BUTTON_S        	key_power;
	EVENT_S				key_power_event;
	TIMER_TASK_S		handle;

    void		(*key_power_touch)   (void);
} MOD_POWER_S;

extern MOD_POWER_S gl_mod_power;

void mod_power_init(COBRA_SYS_S *sys);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_H_ */
