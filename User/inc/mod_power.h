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

#define POWER_LED_PORT			GPIO_PORT(POWER_LED_PORTx)
#define POWER_LED_PIN			GPIO_PIN(POWER_LED_PINx)

struct cobra_sys_s;

typedef struct key_power_info_s
{
	uint8_t		power	:1;
	uint8_t				:7;
} KEY_POWER_INFO_S;

typedef struct key_power_event_s
{
	uint8_t		touch;
	EVENT_S		event;
} KEY_POWER_EVENT_S;

typedef struct mod_power_s
{
    COBRA_SYS_S			*sys;
	KEY_POWER_INFO_S	info;
	LED_S				power_led;
    BUTTON_S        	key_power;
	KEY_POWER_EVENT_S	key_power_event;
	TIMER_TASK_S		handle;

    void		(*key_power_touch)   (void);
} MOD_POWER_S;

extern MOD_POWER_S gl_mod_power;

void mod_power_init(COBRA_SYS_S *sys);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_H_ */
