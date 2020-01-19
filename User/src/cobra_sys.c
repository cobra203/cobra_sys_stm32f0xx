#include <cobra_timer.h>
#include <cobra_event.h>
#include <cobra_cmd.h>
#include <cobra_console.h>
#include <mod_power.h>
#include <cobra_sys.h>

#if (LOG_SYS_LEVEL > LOG_LEVEL_NOT)
#define SYS_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("SYS     : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define SYS_INFO(fm, ...)
#endif

#if (LOG_SYS_LEVEL > LOG_LEVEL_INFO)
#define SYS_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("SYS     : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define SYS_DEBUG(fm, ...)
#endif

#define SYS_LOG(level, fm, ...) SYS_##level(fm, ##__VA_ARGS__)

COBRA_SYS_S gl_sys;

int	cobra_event_process(EVENT_S *event)
{
	CMD_S parse;

	switch(event->id) {
	case EV_CON_CMD:
		assert_param((char *)event->data);
		if(CBA_SUCCESS != cmd_parse((char *)event->data, &parse)) {
			SYS_LOG(INFO, "Format error\r\n");
			break;
		}
		parse.status = event->status.state;
		if(CBA_SUCCESS != cmd_process(&parse)) {
			SYS_LOG(INFO, "Invalid command\r\n");
		}
		event->status.state = parse.status;
		break;

	case EV_POWER_SWITCH:
		if(gl_sys.mod_power) {
			if(gl_sys.status.power->enable) {
				SYS_LOG(INFO, "Power On\r\n");
			}
			else {
				SYS_LOG(INFO, "Power Off\r\n");
			}
		}
		break;

	default:
		break;
	}

	return 0;
}

static void _cobra_cmd_test_resp(void *data)
{
	CONSOLE_EVENT_S *event = &gl_sys.cmd_test_resp;

	if(CBA_FALSE == event_is_active(&event->event)) {
		snprintf(event->cmdline, _CMDLINE_MAX_SIZE_, "sys_test %d", gl_sys.status.flag);
		event_commit(&event->event, EV_CON_CMD, 3, EV_STATE_RESPONSE, &event->cmdline);
		timer_task_release(&gl_sys.cmd_test_resp_timeout);
	}
}

static void _cobra_cmd_test_timeout(void *data)
{
	CONSOLE_EVENT_S *event = &gl_sys.cmd_test_resp;

	if(CBA_FALSE == event_is_active(&event->event)) {
		snprintf(event->cmdline, _CMDLINE_MAX_SIZE_, "sys_test %d", gl_sys.status.flag);
		event_commit(&event->event, EV_CON_CMD, 3, EV_STATE_TIMEOUT, &event->cmdline);
	}
}

static void cobra_sys_test(void *cmd)
{
	CMD_S *pcmd = (CMD_S *)cmd;
	uint32_t delay = 0;

	if(EV_STATE_NORMAL == pcmd->status && 0 != strlen(pcmd->arg)) {
		if(0 == strncmp("delay", pcmd->arg, strlen("delay"))) {
			pcmd->status = EV_STATE_REQUEST;
			sscanf(&pcmd->arg[sizeof("delay")], "%d", &delay);

			timer_task_create(&gl_sys.cmd_test_resp_task, TMR_ONCE, delay, 0, _cobra_cmd_test_resp);
			timer_task_create(&gl_sys.cmd_test_resp_timeout, TMR_ONCE, 2000, 0, _cobra_cmd_test_timeout);
		}
		else if(0 == strncmp("sleep", pcmd->arg, strlen("sleep"))) {
			sscanf(&pcmd->arg[sizeof("sleep")], "%d", &delay);
			delay_ms(delay);
			SYS_LOG(INFO, "test: work=%d\r\n", gl_sys.status.flag);
		}
		else {
			SYS_LOG(INFO, "Invalid Arguments\r\n");
			SYS_LOG(INFO, "Usage: sys_test [delay|sleep] [ms]\r\n");
		}
		return;
	}

	SYS_LOG(INFO, "============================================================\r\n");
	switch(pcmd->status) {
	case EV_STATE_NORMAL:
		SYS_LOG(INFO, "test: work=%d\r\n", gl_sys.status.flag);
		break;
	case EV_STATE_RESPONSE:
		SYS_LOG(INFO, "test: work=%s\r\n", pcmd->arg);
		break;
	default:
		SYS_LOG(INFO, "%s_%s: timeout\r\n", pcmd->prefix, pcmd->subcmd);
		break;
	}
	SYS_LOG(INFO, "============================================================\r\n");
}
CMD_CREATE(sys, test, cobra_sys_test);

static void cobra_sys_status_dump(void)
{
#if 1
	SYS_LOG(INFO, "============================================================\r\n");
	if(gl_sys.mod_power) {
		SYS_LOG(INFO, "| power     | enable    : %d                                |\r\n",
						gl_sys.status.power->enable);
	}
	SYS_LOG(INFO, "============================================================\r\n");
#endif
}

static void cobra_sys_status(void *cmd)
{
	CMD_S *pcmd = (CMD_S *)cmd;

	if(0 == strlen(pcmd->arg)) {
		cobra_sys_status_dump();
	}
	else {
		SYS_LOG(INFO, "Invalid Arguments\r\n");
		SYS_LOG(INFO, "Usage: status\r\n");
	}
}
CMD_CREATE_SIMPLE(status, cobra_sys_status);

static uint8_t _cobra_parse_flag(uint8_t rcc_flag)
{
#define RCC_FLAG_MASK	((uint8_t)0x1F)
	return (gl_sys.status.flag & ((uint32_t)1 << (rcc_flag & RCC_FLAG_MASK))) ? 1 : 0;
}

static void _cobra_sys_resetflag_dump(void)
{
#if 1
	SYS_LOG(INFO, "============================================================\r\n");
	SYS_LOG(INFO, "| LPWRRST: %d | WWDGRST: %d | IWDGRST: %d |            |\r\n",
		_cobra_parse_flag(RCC_FLAG_LPWRRST), _cobra_parse_flag(RCC_FLAG_WWDGRST), _cobra_parse_flag(RCC_FLAG_IWDGRST));
	SYS_LOG(INFO, "| SFTRST : %d | PORRST : %d | PINRST : %d | OBLRST : %d |\r\n",
		_cobra_parse_flag(RCC_FLAG_SFTRST), _cobra_parse_flag(RCC_FLAG_PORRST),
		_cobra_parse_flag(RCC_FLAG_PINRST), _cobra_parse_flag(RCC_FLAG_OBLRST));
	SYS_LOG(INFO, "============================================================\r\n");
#endif
#if 0
	SYS_LOG(INFO, "============================================================\r\n");
	SYS_LOG(INFO, "| LPWRRST: %d | WWDGRST: %d | IWDGRST: %d |            |\r\n",
		RCC_GetFlagStatus(RCC_FLAG_LPWRRST), RCC_GetFlagStatus(RCC_FLAG_WWDGRST), RCC_GetFlagStatus(RCC_FLAG_IWDGRST));
	SYS_LOG(INFO, "| SFTRST : %d | PORRST : %d | PINRST : %d | OBLRST : %d |\r\n",
		RCC_GetFlagStatus(RCC_FLAG_SFTRST), RCC_GetFlagStatus(RCC_FLAG_PORRST),
		RCC_GetFlagStatus(RCC_FLAG_PINRST), RCC_GetFlagStatus(RCC_FLAG_OBLRST));
	SYS_LOG(INFO, "============================================================\r\n");
#endif
}

static void _cobra_record_resetflag(void)
{
	gl_sys.status.flag = (uint32_t)RCC->CSR;
	RCC_ClearFlag();
	_cobra_sys_resetflag_dump();
}

static void cobra_sys_resetflag(void *cmd)
{
	CMD_S *pcmd = (CMD_S *)cmd;

	if(0 == strlen(pcmd->arg)) {
		_cobra_sys_resetflag_dump();
	}
	else {
		SYS_LOG(INFO, "Invalid Arguments\r\n");
		SYS_LOG(INFO, "Usage: flag\r\n");
	}
}
CMD_CREATE_SIMPLE(flag, cobra_sys_resetflag);

static void cobra_sys_reboot(void *cmd)
{
	console_puts("cobra system reboot ...\r\n");
	NVIC_SystemReset();
}
CMD_CREATE_SIMPLE(reboot, cobra_sys_reboot);

static void cobra_sys_handle(void *args)
{
	//static int times = 0;
	//COBRA_SYS_S *sys = (COBRA_SYS_S *)args;

	//SYS_LOG("%s:%d\r\n", __func__, times++);
	//delay_ms(5000);

	//timer_task_create(&(sys->task), TMR_ONCE, 1000, 0, cobra_sys_handle, (void *)sys);
}

static void cobra_sys_register(void)
{
	gl_sys.handle.name = "cobra_sys_handle";
	gl_sys.handle.cb_data = &gl_sys;
	timer_task_create(&gl_sys.handle, TMR_CYCLICITY, 10, 5000, cobra_sys_handle);
}

void cobra_sys_init(void)
{
	memset(&gl_sys, 0, sizeof(&gl_sys));
	_cobra_record_resetflag();

	gl_sys.event_process = cobra_event_process;
	gl_sys.sys_handle = cobra_sys_handle;

	gl_sys.cmd_test_resp_task.name = "cobra_cmd_test_resp";
	gl_sys.cmd_test_resp_task.cb_data = CBA_NULL;
	gl_sys.cmd_test_resp_timeout.name = "cobra_cmd_test_timeout";
	gl_sys.cmd_test_resp_timeout.cb_data = CBA_NULL;

	cmd_register(&cmd_sys_test);
	cmd_register(&cmd_reboot);
	cmd_register(&cmd_flag);
	cmd_register(&cmd_status);

	cobra_sys_register();

	mod_power_init(&gl_sys);

	SYS_LOG(INFO, "%s ... OK\r\n", __func__);
}
