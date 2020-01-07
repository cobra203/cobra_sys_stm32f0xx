#ifndef _COBRA_CONSOLE_H_
#define _COBRA_CONSOLE_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>
#include <cobra_define.h>
#include <cobra_list.h>
#include <cobra_timer.h>
#include <cobra_cmd.h>
#include <cobra_event.h>

#define LOG_LEVEL_NOT	0
#define LOG_LEVEL_INFO	1
#define LOG_LEVEL_DEBUG	2

/* ============= define console log of module =============*/
#define LOG_CONSOLE_LEVEL	LOG_LEVEL_INFO
#define LOG_CMD_LEVEL		LOG_LEVEL_INFO
#define LOG_TIMER_LEVEL		LOG_LEVEL_INFO
#define LOG_EVENT_LEVEL		LOG_LEVEL_INFO
#define LOG_SYS_LEVEL		LOG_LEVEL_INFO
#define LOG_BUTTON_LEVEL	LOG_LEVEL_NOT
#define LOG_POWER_LEVEL		LOG_LEVEL_INFO
/* ========================================================*/

#define PRINTF_REDIRCT_EN
#define USE_VIRTUAL_UARTx

#define CONSOLE_TAG			"[cobra]:/# "
#define CONSOLE_EVENT_MAX	2

#if CBA_PLATFORM == PLATFORM_STM32
#define CONSOLE_UART_COM	USART2
#define CONSOLE_UART_BAUD	115200
#endif

#define CONSOLE_TX_PORT		GPIO_PORT(CONSOLE_TX_PORTx)
#define CONSOLE_TX_PIN		GPIO_PIN(CONSOLE_TX_PINx)
#define CONSOLE_TX_PS		GPIO_PS(CONSOLE_TX_PINx)
#define CONSOLE_TX_AF		GPIO_AF(CONSOLE_TX_AFx)

#define CONSOLE_RX_PORT		GPIO_PORT(CONSOLE_RX_PORTx)
#define CONSOLE_RX_PIN		GPIO_PIN(CONSOLE_RX_PINx)
#define CONSOLE_RX_PS		GPIO_PS(CONSOLE_RX_PINx)
#define CONSOLE_RX_AF		GPIO_AF(CONSOLE_RX_AFx)

typedef struct console_event_s
{
	uint8_t		touch;
	EVENT_S		event;
	char		cmdline[_CMDLINE_MAX_SIZE_];
} CONSOLE_EVENT_S;

typedef struct console_s
{
	char				cmdline[_CMDLINE_MAX_SIZE_];
	char				cmdline_size;
	CONSOLE_EVENT_S		event_cache[CONSOLE_EVENT_MAX];
	TIMER_TASK_S		handle;

	void	(*cmdline_touch)	(void);
} CONSOLE_S;

extern CONSOLE_S gl_console;

void console_send_byte(char byte);
void console_send(char *data, uint8_t size);
#define console_puts(str) console_send((str), sizeof(str) - 1)

void console_cmdline_clean(void);
void console_cmdline_restore(void);

void console_init_early(void);
void console_init(void);

/*===================================================================================*/
/* Define console function                                                           */
/*===================================================================================*/
#if (CBA_PLATFORM == PLATFORM_STM8) || (CBA_PLATFORM == PLATFORM_STM32)
#ifdef PRINTF_REDIRCT_EN
#define console(format, ...) { \
	printf(format, ##__VA_ARGS__); \
}

#else
#define	CONSOLE_BUFFER_MAX_SIZE 128
extern char _console_buffer_[CONSOLE_BUFFER_MAX_SIZE];
#define console(format, ...) { \
	snprintf(_console_buffer_, sizeof(_console_buffer_), format, ##__VA_ARGS__); \
	console_send(_console_buffer_, strlen(_console_buffer_)); \
}
#endif /* PRINTF_REDIRCT_EN */

#else
#define console(format, ...) printf(stdout, format, ##__VA_ARGS__)

#endif

/*===================================================================================*/
/*===================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_CONSOLE_H_ */

