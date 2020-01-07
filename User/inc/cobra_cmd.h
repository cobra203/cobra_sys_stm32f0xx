#ifndef _COBRA_CMD_H_
#define _COBRA_CMD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_list.h>

#define _PREFIX_SIZE_		10 + 1	/* the 1 is '_' */
#define _SUBCMD_SIZE_		10 + 1	/* the 1 is ' ' */
#define _ARG_SIZE_			10 + 1	/* the 1 is '\0' */
#define _CMDLINE_MAX_SIZE_	(_PREFIX_SIZE_ + _SUBCMD_SIZE_ + _ARG_SIZE_)
/***********************************************/
/* cmdline max size is _CMDLINE_MAX_SIZE_=32   */
/* so a cmd format is:                         */
/* prefix_subcmd arg                           */
/* 9 +   1 +10+ 1+10 + '\0' = 32               */
/***********************************************/

typedef struct cmd_s
{
	char			prefix[_PREFIX_SIZE_];
	char			subcmd[_SUBCMD_SIZE_];
	char			arg[_ARG_SIZE_];
	uint8_t			status;
	void			(*process)		(void *);
	LIST_S			list;
} CMD_S;

#define CMD_CREATE(PREFIX, SUBCMD, PROCESS) \
struct cmd_s cmd_##PREFIX##_##SUBCMD = { \
	.prefix = #PREFIX, \
	.subcmd = #SUBCMD, \
	.process = PROCESS, \
}

#define CMD_CREATE_SIMPLE(PREFIX, PROCESS) \
struct cmd_s cmd_##PREFIX = { \
	.prefix = #PREFIX, \
	.process = PROCESS, \
}

CBA_BOOL cmd_parse(const char *cmdline, CMD_S *parse);
CBA_BOOL cmd_process(CMD_S *parse);
void cmd_register(CMD_S *cmd);
void cmd_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _COBRA_CMD_H_ */
