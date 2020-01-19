#ifndef _COBRA_COMMON_H_
#define _COBRA_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif

/*************** branch item define start ***********/
#define PLATFORM_STM8		0
#define PLATFORM_STM32		1
#define PLATFORM_LINUX		2
/*************** branch item define end *************/

#define CBA_PLATFORM		PLATFORM_STM32

/* function defined */
#include <stdio.h>
#include <string.h>

#if (CBA_PLATFORM == PLATFORM_STM8)
#include <stm8s.h>

#elif (CBA_PLATFORM == PLATFORM_STM32)
#if defined (STM32F303xE)
#define __STM32F30X
#elif defined (STM32F051)
#define __STM32F0XX
#endif

#if defined (_STM32F30X)
#include <stm32f30x.h>
#include <stm32f30x_rcc.h>
#include <stm32f30x_usart.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_misc.h>
#include <stm32f30x_tim.h>
#include <stm32f30x_exti.h>
#include <stm32f30x_syscfg.h>
#include <stm32f30x_iwdg.h>
#elif defined (__STM32F0XX)
#include <stm32f0xx.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_usart.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_tim.h>
#include <stm32f0xx_exti.h>
#include <stm32f0xx_syscfg.h>
#include <stm32f0xx_iwdg.h>
#endif

#elif (CBA_PLATFORM == PLATFORM_LINUX)
#include <linux.h>

#endif

#define BIT_ISSET(a, s) (((a) >> (s)) & 0x1)
#define BIT_SET(a, s)   ((a) = (a) | 0x1 << (s))
#define BIT_CLR(a, s)   ((a) = (a) & ~(0x1 << (s)))

#define CBA_SUCCESS     0
#define CBA_FAILURE     -1

#define CBA_TRUE        1
#define CBA_FALSE       0

#define CBA_ENABLE      CBA_TRUE
#define CBA_DISABLE     CBA_FALSE

#define CBA_NULL		((void *)0)
#define CBA_BOOL		uint8_t

#ifdef __cplusplus
}
#endif

#endif /* _COBRA_COMMON_H_ */
