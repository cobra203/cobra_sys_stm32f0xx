#include <string.h>
#include <cobra_common.h>
#include <cobra_cmd.h>
#include <cobra_console.h>
#include <cobra_list.h>

#if (LOG_CMD_LEVEL > LOG_LEVEL_NOT)
#define CMD_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("CMD     : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define CMD_INFO(fm, ...)
#endif

#if (LOG_CMD_LEVEL > LOG_LEVEL_INFO)
#define CMD_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("CMD     : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define CMD_DEBUG(fm, ...)
#endif

#define CMD_LOG(level, fm, ...) CMD_##level(fm, ##__VA_ARGS__)

static CMD_S cmd_head;

CBA_BOOL cmd_parse(const char *cmdline, CMD_S *parse)
{
	uint8_t i = 0, space = 0;
	CBA_BOOL rm_space = CBA_FALSE;
	CBA_BOOL no_subcmd = CBA_FALSE;

	/* 1. parse prefix */
	for(i = 0; i < _PREFIX_SIZE_; i++, cmdline++) {
		if(' ' == *cmdline && CBA_FALSE == rm_space) {
			space++;
			continue;
		}
		if(CBA_FALSE == rm_space) {
			rm_space = CBA_TRUE;
		}
		if(' ' == *cmdline || '_' == *cmdline || '\0' == *cmdline) {
			if(i > 0) {
				parse->prefix[i - space] = '\0';
				break;
			}
		}
		parse->prefix[i - space] = *cmdline;
	}
	if(i >= _PREFIX_SIZE_) {
		return CBA_FAILURE;
	}
	if('\0' == *cmdline) {
		parse->subcmd[0] = '\0';
		parse->arg[0] = '\0';
		return CBA_SUCCESS;
	}
	if(' ' == *cmdline++) {
		no_subcmd = CBA_TRUE;
		parse->subcmd[0] = '\0';
	}

	if(CBA_FALSE == no_subcmd) {
		/* 2. parse subcmd */
		for(i = 0; i < _SUBCMD_SIZE_; i++, cmdline++) {
			if(' ' == *cmdline || '\0' == *cmdline) {
				parse->subcmd[i] = '\0';
				break;
			}
			parse->subcmd[i] = *cmdline;
		}
		if(i >= _SUBCMD_SIZE_) {
			return CBA_FAILURE;
		}
		if('\0' == *cmdline++) {
			parse->arg[0] = '\0';
			return CBA_SUCCESS;
		}
	}

	/* 3. parse arg */
	for(; ' ' == *cmdline; cmdline++);
	for(i = 0; i < _ARG_SIZE_; i++, cmdline++) {
		if('\0' == *cmdline) {
			parse->arg[i] = '\0';
			break;
		}
		parse->arg[i] = *cmdline;
	}
	if(i >= _ARG_SIZE_) {
		return CBA_FAILURE;
	}

	return CBA_SUCCESS;
}

CBA_BOOL cmd_process(CMD_S *parse)
{
	CMD_S *pos;

	list_for_each_entry(pos, &cmd_head.list, CMD_S, list) {
		if(!strcmp(parse->prefix, pos->prefix) &&
			!strcmp(parse->subcmd, pos->subcmd)) {
			memcpy(pos->arg, parse->arg, strlen(parse->arg) + 1);

			pos->status = parse->status;
			if(strlen(parse->subcmd)) {
				CMD_LOG(DEBUG, "%s_%s: process start status[%d]\n", pos->prefix, pos->subcmd, pos->status);
			}
			else {
				CMD_LOG(DEBUG, "%s: process start status[%d]\n", pos->prefix, pos->status);
			}
			pos->process(pos);
			parse->status = pos->status;

			if(strlen(parse->subcmd)) {
				CMD_LOG(DEBUG, "%s_%s: process end status[%d]\n", pos->prefix, pos->subcmd, pos->status);
			}
			else {
				CMD_LOG(DEBUG, "%s: process end status[%d]\n", pos->prefix, pos->status);
			}
			return CBA_SUCCESS;
		}
	}

	return CBA_FAILURE;
}

static void cmd_list(void *cmd)
{
	uint8_t i = 0;
	CMD_S *pos;

	CMD_LOG(INFO, "============================================================\n");
	list_for_each_entry(pos, &cmd_head.list, CMD_S, list) {
		if(strlen(pos->subcmd)) {
			CMD_LOG(INFO, "%-2d: %s_%s\n", i++, pos->prefix, pos->subcmd);
		}
		else {
			CMD_LOG(INFO, "%-2d: %s\n", i++, pos->prefix);
		}
	}
	CMD_LOG(INFO, "============================================================\n");
}
CMD_CREATE(cmd, list, cmd_list);

void cmd_register(CMD_S *cmd)
{
	list_add_tail(&cmd->list, &cmd_head.list);
	if(strlen(cmd->subcmd)) {
		CMD_LOG(INFO, "register: [%s_%s]\n", cmd->prefix, cmd->subcmd);
	}
	else {
		CMD_LOG(INFO, "register: [%s]\n", cmd->prefix);
	}
}

void cmd_init(void)
{
	INIT_LIST_HEAD(&cmd_head.list);
	cmd_register(&cmd_cmd_list);
	CMD_LOG(INFO, "%s ... OK\n", __func__);
}

