#include <stdarg.h>
#include <stdio.h>
#include <cobra_console.h>
#include <cobra_sys.h>

#if (LOG_CONSOLE_LEVEL > LOG_LEVEL_NOT)
#define CONSOLE_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("CONSOLE : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define CONSOLE_INFO(fm, ...)
#endif

#if (LOG_CONSOLE_LEVEL > LOG_LEVEL_INFO)
#define CONSOLE_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("CONSOLE : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define CONSOLE_DEBUG(fm, ...)
#endif

#define CONSOLE_LOG(level, fm, ...) CONSOLE_##level(fm, ##__VA_ARGS__)

CONSOLE_S gl_console;

#ifdef PRINTF_REDIRCT_EN
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
	/* Loop until transmit data register is empty */
	//while(USART_GetFlagStatus(CONSOLE_UART_COM, USART_FLAG_TXE) == RESET) {}
	while((CONSOLE_UART_COM->ISR & USART_FLAG_TXE) == (uint16_t)RESET) {}

	//USART_SendData(CONSOLE_UART_COM, ch);
	CONSOLE_UART_COM->TDR = (ch & (uint16_t)0x01FF);

	/* Loop until transmit data is complete  */
	//while(USART_GetFlagStatus(CONSOLE_UART_COM, USART_FLAG_TC) == RESET) {}
	while((CONSOLE_UART_COM->ISR & USART_FLAG_TC) == (uint16_t)RESET) {}
	return ch;
}
#else
char _console_buffer_[CONSOLE_BUFFER_MAX_SIZE];

#endif /* PRINTF_REDIRCT_EN */

void console_send_byte(char byte)
{
	// Loop until transmit data register is empty
	//while(USART_GetFlagStatus(CONSOLE_UART_COM, USART_FLAG_TXE) == RESET) {}
	while((CONSOLE_UART_COM->ISR & USART_FLAG_TXE) == (uint16_t)RESET) {}

	//USART_SendData(CONSOLE_UART_COM, ch);
	CONSOLE_UART_COM->TDR = (byte & (uint16_t)0x01FF);

	// Loop until transmit data is complete
	//while(USART_GetFlagStatus(CONSOLE_UART_COM, USART_FLAG_TC) == RESET) {}
	while((CONSOLE_UART_COM->ISR & USART_FLAG_TC) == (uint16_t)RESET) {}
}

void console_send(char *data, uint8_t size)
{
	while(size--) {
		console_send_byte(*(data++));
	}
}

/***************************************************************/
/* for console log *********************************************/
void console_cmdline_clean(void)
{
	int i = gl_console.cmdline_size + sizeof(CONSOLE_TAG);
	console_send_byte('\r');
	while(i--) {
		console_send_byte(' ');
	}
	console_send_byte('\r');
}

void console_cmdline_restore(void)
{
	console_puts(CONSOLE_TAG);
	gl_console.cmdline[gl_console.cmdline_size] = 0;
	console_send(gl_console.cmdline, gl_console.cmdline_size);
}
/***************************************************************/
/***************************************************************/

static void console_cmdline_touch(void)
{
	CONSOLE_S *con = &gl_console;
	CONSOLE_EVENT_S *con_event = CBA_NULL;
	uint8_t cmd_size = con->cmdline_size;
	uint8_t i = 0;

	if(0 == cmd_size) {
		return;
	}

	gl_console.cmdline_size = 0;

	for(i = 0; i < CONSOLE_EVENT_MAX; i++) {
		if(CBA_FALSE == con->event_cache[i].event.info.active) {
			con_event = &con->event_cache[i];
			memcpy(con_event->cmdline, con->cmdline, cmd_size + 1);

			con_event->event.info.active = CBA_TRUE;
			con_event->event.info.priority = 3;
			con_event->event.info.id = EV_CON_CMD;
			con_event->event.info.status = EV_STATUS_NORMAL;
			con_event->event.data = &con_event->cmdline;

			con_event->touch = CBA_TRUE;
			return;
		}
	}
	CONSOLE_LOG(INFO, "console event cache is full\n");
}

void console_init_early(void)
{
	gl_console.cmdline_touch = console_cmdline_touch;

#if (CBA_PLATFORM == PLATFORM_STM32)
	GPIO_InitTypeDef		gpio_cfg;
	USART_InitTypeDef		usart_cfg;
	NVIC_InitTypeDef		nvic_cfg;
	TIM_TimeBaseInitTypeDef tim_basecfg;

	/* 1. Configuration USART gpio function */
	GPIO_PinAFConfig(CONSOLE_TX_PORT, CONSOLE_TX_PS, CONSOLE_TX_AF);
	GPIO_PinAFConfig(CONSOLE_RX_PORT, CONSOLE_RX_PS, CONSOLE_RX_AF);

	gpio_cfg.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_cfg.GPIO_Pin = CONSOLE_TX_PIN;
	gpio_cfg.GPIO_Mode = GPIO_Mode_AF;
	gpio_cfg.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CONSOLE_TX_PORT, &gpio_cfg);

	gpio_cfg.GPIO_Pin = CONSOLE_RX_PIN;
	gpio_cfg.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(CONSOLE_RX_PORT, &gpio_cfg);

	/* 2. Configuration USART interface */
	usart_cfg.USART_BaudRate = CONSOLE_UART_BAUD;
	usart_cfg.USART_WordLength = USART_WordLength_8b;
	usart_cfg.USART_StopBits = USART_StopBits_1;
	usart_cfg.USART_Parity = USART_Parity_No;
	usart_cfg.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_cfg.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(CONSOLE_UART_COM, &usart_cfg);

	/* 3. Enable the USART Interrupt */
	nvic_cfg.NVIC_IRQChannel = CONSOLE_RX_IRQ;
#if defined (STM32F303xE)
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_cfg.NVIC_IRQChannelSubPriority = 0;
#elif defined (STM32F051)
	nvic_cfg.NVIC_IRQChannelPriority = 0;
#endif
	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);

	/* 4. Enable USART */
	USART_ITConfig(CONSOLE_UART_COM, USART_IT_RXNE, ENABLE);
	USART_Cmd(CONSOLE_UART_COM, ENABLE);

	/* 5. Configuration TIM3 for RX buffer */
	// Probably not necessary for the console
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	nvic_cfg.NVIC_IRQChannel = TIM3_IRQn;
#if defined (STM32F303xE)
	nvic_cfg.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_cfg.NVIC_IRQChannelSubPriority = 0;
#elif defined (STM32F051)
	nvic_cfg.NVIC_IRQChannelPriority = 0;
#endif

	nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_cfg);

	// 115200 1bit=8.65ns
	tim_basecfg.TIM_Period = 80 - 1;		// timer = 80us
#if defined (STM32F303xE)
	tim_basecfg.TIM_Prescaler = (72 - 1);	// 1MHz, 1us
#elif defined (STM32F051)
	tim_basecfg.TIM_Prescaler = (48 - 1);	// 1MHz, 1us
#endif
	tim_basecfg.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_basecfg.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &tim_basecfg);

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

#endif /* CBA_PLATFORM == PLATFORM_STM32 */
	console_puts("\n+====================================================================+\n");
	console_puts("| system  : cobra system template                                    |\n");
	console_puts("| version : v0.1 - 19/12/16                                          |\n");
	console_puts("| author  : sm723@qq.com                                             |\n");
	console_puts("+====================================================================+\n");
	CONSOLE_LOG(INFO, "%s ... OK\n", __func__);
}

static void _console_monitor_handle(void *arg)
{
	CONSOLE_EVENT_S *event_cache = &gl_console.event_cache[0];
	uint8_t i = 0;

	for(i = 0; i < CONSOLE_EVENT_MAX; i++) {
		if(CBA_TRUE == event_cache[i].touch) {
			event_cache[i].touch = CBA_FALSE;
			event_commit(&event_cache[i].event);
		}
	}
}

void console_init(void)
{
	gl_console.handle.name = "console_monitor_handle";
	gl_console.handle.cb_data = CBA_NULL;
	timer_task_create(&gl_console.handle, TMR_CYCLICITY, 10, 100, _console_monitor_handle);

	CONSOLE_LOG(INFO, "%s ... OK\n", __func__);
}

