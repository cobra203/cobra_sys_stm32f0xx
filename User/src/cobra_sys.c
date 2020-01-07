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
	LED_S *power_led = &gl_sys.mod_power->power_led;

	switch(event->info.id) {
	case EV_CON_CMD:
		assert_param((char *)event->data);
		if(CBA_SUCCESS != cmd_parse((char *)event->data, &parse)) {
			SYS_LOG(INFO, "Format error\n");
			break;
		}
		parse.status = event->info.status;
		if(CBA_SUCCESS != cmd_process(&parse)) {
			SYS_LOG(INFO, "Invalid command\n");
		}
		event->info.status = parse.status;
		break;

	case EV_POWER_SWITCH:
		if(*((uint8_t *)event->data)) {
			SYS_LOG(INFO, "Power On\n");
			power_led->doing(power_led, 100, 1900);
		}
		else {
			SYS_LOG(INFO, "Power Off\n");
			power_led->doing(power_led, 0, 1);
		}
		break;


	default:
		break;
	}

	return 0;
}

static void _cobra_cmd_status_resp(void *data)
{
	if(CBA_FALSE == gl_sys.cmd_status_resp.event.info.active) {
		gl_sys.cmd_status_resp.event.info.id = EV_CON_CMD;
		gl_sys.cmd_status_resp.event.info.priority = 3;
		gl_sys.cmd_status_resp.event.info.status = EV_STATUS_RESPONSE;
		gl_sys.cmd_status_resp.event.info.active = CBA_TRUE;
		snprintf(gl_sys.cmd_status_resp.cmdline, _CMDLINE_MAX_SIZE_, "sys_status %d", gl_sys.status.work);
		event_commit(&gl_sys.cmd_status_resp.event);
		if(gl_sys.cmd_status_resp_timeout.info.active) {
		timer_task_release(&gl_sys.cmd_status_resp_timeout);
		}
	}
}

static void _cobra_cmd_status_timeout(void *data)
{
	if(CBA_FALSE == gl_sys.cmd_status_resp.event.info.active) {
		gl_sys.cmd_status_resp.event.info.id = EV_CON_CMD;
		gl_sys.cmd_status_resp.event.info.priority = 3;
		gl_sys.cmd_status_resp.event.info.status = EV_STATUS_TIMEOUT;
		gl_sys.cmd_status_resp.event.info.active = CBA_TRUE;
		snprintf(gl_sys.cmd_status_resp.cmdline, _CMDLINE_MAX_SIZE_, "sys_status %d", gl_sys.status.work);
		event_commit(&gl_sys.cmd_status_resp.event);
	}
}

static void cobra_sys_status(void *cmd)
{
	CMD_S *pcmd = (CMD_S *)cmd;
	uint32_t delay = 0;

	if(EV_STATUS_NORMAL == pcmd->status && strcmp("delay", pcmd->arg) < 0) {
		pcmd->status = EV_STATUS_REQUEST;
		sscanf(&pcmd->arg[sizeof("delay")], "%d", &delay);

		timer_task_create(&gl_sys.cmd_status_resp_task, TMR_ONCE, delay, 0, _cobra_cmd_status_resp);
		timer_task_create(&gl_sys.cmd_status_resp_timeout, TMR_ONCE, 2000, 0, _cobra_cmd_status_timeout);
		return;
	}

	SYS_LOG(INFO, "============================================================\n");
	switch(pcmd->status) {
	case EV_STATUS_NORMAL:
		SYS_LOG(INFO, "STATUS: work=%d\n", gl_sys.status.work);
		break;
	case EV_STATUS_RESPONSE:
		SYS_LOG(INFO, "STATUS: work=%s\n", pcmd->arg);
		break;
	default:
		SYS_LOG(INFO, "%s_%s: timeout\n", pcmd->prefix, pcmd->subcmd);
		break;
	}
	SYS_LOG(INFO, "============================================================\n");
}
CMD_CREATE(sys, status, cobra_sys_status);

static void cobra_sys_reboot(void *cmd)
{
	console_puts("cobra system reboot ...\n");
	NVIC_SystemReset();
}
CMD_CREATE_SIMPLE(reboot, cobra_sys_reboot);

static void cobra_sys_handle(void *args)
{
	//static int times = 0;
	//COBRA_SYS_S *sys = (COBRA_SYS_S *)args;

	//SYS_LOG("%s:%d\n", __func__, times++);
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
	gl_sys.event_process = cobra_event_process;
	gl_sys.sys_handle = cobra_sys_handle;

	gl_sys.cmd_status_resp.event.data = gl_sys.cmd_status_resp.cmdline;
	gl_sys.cmd_status_resp_task.name = "cobra_cmd_status_resp";
	gl_sys.cmd_status_resp_task.cb_data = CBA_NULL;
	gl_sys.cmd_status_resp_timeout.name = "cobra_cmd_status_timeout";
	gl_sys.cmd_status_resp_timeout.cb_data = CBA_NULL;

	cmd_register(&cmd_sys_status);
	cmd_register(&cmd_reboot);

	cobra_sys_register();

	mod_power_init(&gl_sys);

	SYS_LOG(INFO, "%s ... OK\n", __func__);
}
