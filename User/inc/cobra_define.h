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

/*************** cobra function define *************/
/* for console */
#define CONSOLE_TX_PORTx	A
#define CONSOLE_TX_PINx		2
#define CONSOLE_TX_AFx		1

#define CONSOLE_RX_PORTx	A
#define CONSOLE_RX_PINx		3
#define CONSOLE_RX_AFx		1
#define CONSOLE_RX_IRQ		USART2_IRQn

/*************** cobra function define *************/

/* for mod_power */
#define KEY_POWER_PORTx		A
#define KEY_POWER_PINx		5
#define KEY_POWER_IRQ		EXTI4_15_IRQn

#define	POWER_LED_PORTx		B
#define POWER_LED_PINx		5

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_DEFINE_H_ */
