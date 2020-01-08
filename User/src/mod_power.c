#include <cobra_define.h>
#include <cobra_console.h>
#include <cobra_timer.h>
#include <mod_power.h>

#if (LOG_POWER_LEVEL > LOG_LEVEL_NOT)
#define POWER_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("POWER   : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define POWER_INFO(fm, ...)
#endif

#if (LOG_POWER_LEVEL > LOG_LEVEL_INFO)
#define POWER_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("POWER   : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define POWER_DEBUG(fm, ...)
#endif

#define POWER_LOG(level, fm, ...) POWER_##level(fm, ##__VA_ARGS__)

MOD_POWER_S gl_mod_power;

static void _mod_power_ctrl(CBA_BOOL enable)
{
	if(enable) {
		gl_mod_power.power_ctrl.doing(&gl_mod_power.power_ctrl, 1, 0);
	}
	else {
		gl_mod_power.power_ctrl.doing(&gl_mod_power.power_ctrl, 0, 1);
	}
}

static void _mod_power_swtich(CBA_BOOL old_status)
{
	EVENT_S *event = &gl_mod_power.key_power_event;

	if(CBA_FALSE == event_is_active(event)) {
		gl_mod_power.status.enable = old_status ? 0 : 1;
		_mod_power_ctrl(gl_mod_power.status.enable);
		POWER_LOG(INFO, "power switch ->%d\n", gl_mod_power.status.enable);
		event_commit(event, EV_POWER_SWITCH, 3, EV_STATE_NORMAL, CBA_NULL);
	}
	else {
		POWER_LOG(INFO, "event EV_POWER_SWITCH is busy\n");
	}
}

static void _mod_power_key_power_process(BUTTON_S *button)
{
	uint8_t updata = CBA_FALSE;

	if(button->state.active) {

		switch(button->state.effective) {
		case ECT_LOOSE:
			if(button->cont_count < 3) {
				updata = CBA_TRUE;
			}
		case ECT_PRESSED:
			break;
		case ECT_FOCUSED:
			if(button->cont_count == 3) {
				updata = CBA_TRUE;
			}
			break;
		}
		button->state.active = 0;
    }
	if(updata) {
		_mod_power_swtich(gl_mod_power.status.enable);
	}
}

static void mod_power_key_power_touch(void)
{
	gl_mod_power.key_power.detect_count = 10;
}

static void _mod_power_monitor_handle(void *arg)
{
	BUTTON_S *button = (BUTTON_S *)arg;
	uint16_t gpio_vol;

	/* 1. get gpio_vol */
	if(button->detect_count) {
		gpio_vol = GPIO_ReadInputData(KEY_POWER_PORT);
		button->state.press = (~gpio_vol) & KEY_POWER_PIN ? 1 : 0;
		button->detect_count--;
	}
	/* 2. check active effective type */
	button->check_active(button);

	/* 3. logic process */
	_mod_power_key_power_process(button);
}

static void mod_power_cmd(void *cmd)
{
	CMD_S *pcmd = (CMD_S *)cmd;

	POWER_LOG(INFO, "============================================================\n");
	if(0 == strlen(pcmd->arg)) {
		POWER_LOG(INFO, "power: %d\n", gl_mod_power.status.enable);
	}
	else if(strncmp("on", pcmd->arg, strlen("on")) == 0 ||
		strncmp("1", pcmd->arg, strlen("1")) == 0) {
		_mod_power_swtich(CBA_FALSE);
	}
	else if(strncmp("off", pcmd->arg, strlen("off")) == 0 ||
		strncmp("0", pcmd->arg, strlen("0")) == 0) {
		_mod_power_swtich(CBA_TRUE);
	}
	else {
		POWER_LOG(INFO, "Invalid Arguments\n");
		POWER_LOG(INFO, "Usage: power [on|1]|[off|0]\n");
	}
	POWER_LOG(INFO, "============================================================\n");
}
CMD_CREATE_SIMPLE(power, mod_power_cmd);

void mod_power_init(COBRA_SYS_S *sys)
{
    GPIO_InitTypeDef    gpio_cfg = {0};
	NVIC_InitTypeDef	nvic_cfg = {0};
	EXTI_InitTypeDef	exti_cfg = {0};

	sys->mod_power			= &gl_mod_power;
	sys->status.power		= &gl_mod_power.status;
	gl_mod_power.sys_status = &sys->status;

	gl_mod_power.key_power_touch = mod_power_key_power_touch;
	gl_mod_power.key_power_touch();

	gl_mod_power.key_power.check_active 	 	= button_check_active;
	gl_mod_power.key_power.interval.dithering	= 3;
	gl_mod_power.key_power.interval.long_press  = 100;
	gl_mod_power.key_power.interval.continuous  = 100;

    gpio_cfg.GPIO_Mode   = GPIO_Mode_IN;
    gpio_cfg.GPIO_PuPd   = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP;
    gpio_cfg.GPIO_Speed  = GPIO_Speed_2MHz;
    gpio_cfg.GPIO_Pin    = KEY_POWER_PIN;
    GPIO_Init(KEY_POWER_PORT, &gpio_cfg);

	SYSCFG_EXTILineConfig(KEY_POWER_EXTI_PORT, KEY_POWER_EXIT_SOURCE);
	exti_cfg.EXTI_Line		= KEY_POWER_EXTI_LINE;
	exti_cfg.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti_cfg.EXTI_Trigger	= EXTI_Trigger_Rising_Falling;
	exti_cfg.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&exti_cfg);

	nvic_cfg.NVIC_IRQChannel = KEY_POWER_IRQ;
#if defined (STM32F303xE)
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 0xf;
	nvic_cfg.NVIC_IRQChannelSubPriority = 0xf;
#elif defined (STM32F051)
	nvic_cfg.NVIC_IRQChannelPriority = 3;
#endif
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);

	led_init(&gl_mod_power.power_ctrl, POWER_CTRL_PORT, POWER_CTRL_PIN);

	gl_mod_power.handle.name = "mod_power_monitor_handle";
	gl_mod_power.handle.cb_data = &gl_mod_power.key_power;
	timer_task_create(&gl_mod_power.handle, TMR_CYCLICITY, 10, 10, _mod_power_monitor_handle);

	cmd_register(&cmd_power);

	POWER_LOG(INFO, "%s ... OK\n", __func__);
}

