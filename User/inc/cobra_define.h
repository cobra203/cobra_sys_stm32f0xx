#ifndef _COBRA_DEFINE_H_
#define _COBRA_DEFINE_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <cobra_common.h>

#define _GPIO_PORT(X)			GPIO##X
#define GPIO_PORT(X)			_GPIO_PORT(X)

#define _GPIO_PIN(X)			GPIO_Pin_##X
#define GPIO_PIN(X)				_GPIO_PIN(X)

#define _GPIO_AF(X)				GPIO_AF_##X
#define GPIO_AF(X)				_GPIO_AF(X)

#define _GPIO_PS(X)				GPIO_PinSource##X
#define GPIO_PS(X)				_GPIO_PS(X)

#define _EXTI_PORT(X)			EXTI_PortSourceGPIO##X
#define EXTI_PORT(X)			_EXTI_PORT(X)

#define _EXIT_PIN(X)			EXTI_PinSource##X
#define EXIT_PIN(X)				_EXIT_PIN(X)

#define _EXTI_LINE(X)			EXTI_Line##X
#define EXTI_LINE(X)			_EXTI_LINE(X)

#define _EXTI_IRQ(X)			EXTI##X##_IRQn
#define EXTI_IRQ(X)				_EXTI_IRQ(X)

#define _EXTI_IRQ_HANDLER(X)	EXTI##X##_IRQHandler
#define EXTI_IRQ_HANDLER(X)		_EXTI_IRQ_HANDLER(X)

#define _UART_PORT(X)			USART##X
#define UART_PORT(X)			_UART_PORT(X)

#define _UART_IRQ(X)			USART##X##_IRQn
#define UART_IRQ(X)				_UART_IRQ(X)

#define _UASRT_IRQ_HANDLER(X)	USART##X##_IRQHandler
#define UASRT_IRQ_HANDLER(X)	_UASRT_IRQ_HANDLER(X)

#define _UART_RCC(APB, X)		RCC_APB##APB##Periph_USART##X
#define UART_RCC(APB, X)		_UART_RCC(APB, X)

#define _TIM_PORT(X)			TIM##X
#define TIM_PORT(X)				_TIM_PORT(X)

#define _TIM_RCC(APB, X)		RCC_APB##APB##Periph_TIM##X
#define TIM_RCC(APB, X)			_TIM_RCC(APB, X)

#define _RCC_CLOCKCMD(APB)		RCC_APB##APB##PeriphClockCmd
#define RCC_CLOCKCMD(APB)		_RCC_CLOCKCMD(APB)

/*************** cobra function define *************/
/* for timer */
#define DELAY_NS_TIM		2
#define DELAY_NS_TIM_APB	1

/* for console */
#if 1
#define CONSOLE_UART_PORTx	1
#define CONSOLE_UART_APB	2

#define CONSOLE_TX_PORTx	B
#define CONSOLE_TX_PINx		6
#define CONSOLE_TX_AFx		0

#define CONSOLE_RX_PORTx	B
#define CONSOLE_RX_PINx		7
#define CONSOLE_RX_AFx		0

#else
#define CONSOLE_UART_PORTx	2
#define CONSOLE_UART_APB	1

#define CONSOLE_TX_PORTx	A
#define CONSOLE_TX_PINx		2
#define CONSOLE_TX_AFx		1

#define CONSOLE_RX_PORTx	A
#define CONSOLE_RX_PINx		3
#define CONSOLE_RX_AFx		1
#endif

/*************** cobra function define *************/

/* for mod_power */
#define KEY_POWER_PORTx			A
#define KEY_POWER_PINx			5
#define KEY_POWER_IRQ_RANGE		4_15

#define	POWER_CTRL_PORTx		B
#define POWER_CTRL_PINx			5

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_DEFINE_H_ */
