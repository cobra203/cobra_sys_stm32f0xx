#include <cobra_common.h>
#include <cobra_led.h>
#include <cobra_timer.h>

static void _led_bright(LED_S *led)
{
    //GPIO_ResetBits(led->port, led->pin);
	GPIO_SetBits(led->port, led->pin);
    led->status = CBA_TRUE;
}

static void _led_dark(LED_S *led)
{
    //GPIO_SetBits(led->port, led->pin);
	GPIO_ResetBits(led->port, led->pin);
    led->status = CBA_FALSE;
}

static void _led_flash(void *args)
{
	LED_S *led = (LED_S *)args;

    if(!led->status) {
        _led_bright(led);
        timer_task_create(&led->flash_task, TMR_ONCE, led->flash_ms.bright, 0, _led_flash);
    }
    else {
        _led_dark(led);
		timer_task_create(&led->flash_task, TMR_ONCE, led->flash_ms.dark, 0, _led_flash);
    }
}

static void _led_doing(LED_S *led, uint32_t bright, uint32_t dark)
{
	timer_task_release(&led->flash_task);

	led->flash_ms.bright = bright;
	led->flash_ms.dark = dark;

    if(!bright) {
        _led_dark(led);
    }
    else if(!dark) {
        _led_bright(led);
    }
    else {
		_led_bright(led);
		timer_task_create(&led->flash_task, TMR_ONCE, bright, 0, _led_flash);
    }
}

void led_init(LED_S *led, GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_InitTypeDef    gpio_info;

	led->pin	= pin;
	led->port	= port;

    gpio_info.GPIO_Speed  = GPIO_Speed_2MHz;
    gpio_info.GPIO_Mode   = GPIO_Mode_OUT;
    gpio_info.GPIO_PuPd   = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
    gpio_info.GPIO_OType  = GPIO_OType_PP;//GPIO_OType_OD;
    gpio_info.GPIO_Pin    = led->pin;
    GPIO_Init(led->port, &gpio_info);

    led->doing  = _led_doing;

    led->status = CBA_FALSE;

	led->flash_task.name = "led_flash";
	led->flash_task.cb_data = led;
}



