#ifndef _COBRA_LED_H_
#define _COBRA_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_timer.h>

typedef struct led_flash_ms_s
{
	uint32_t	bright	:16;
	uint32_t	dark	:16;
} LED_FLASH_MS_S;

typedef struct led_s
{
    GPIO_TypeDef    *port;
    uint16_t        pin;

	uint8_t         status;
	TIMER_TASK_S	flash_task;
    LED_FLASH_MS_S	flash_ms;

	void	(*doing)	(struct led_s *, uint32_t bright, uint32_t dark);
} LED_S;

void led_init(LED_S *led, GPIO_TypeDef *port, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_LED_H_ */
