/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup STM32F30x_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#if (LOG_MAIN_LEVEL > LOG_LEVEL_NOT)
#define MAIN_INFO(fm, ...) { \
		console_cmdline_clean(); \
		console("MAIN    : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define MAIN_INFO(fm, ...)
#endif

#if (LOG_MAIN_LEVEL > LOG_LEVEL_INFO)
#define MAIN_DEBUG(fm, ...) { \
		console_cmdline_clean(); \
		console("MAIN    : " fm, ##__VA_ARGS__) \
		console_cmdline_restore(); \
	}
#else
#define MAIN_DEBUG(fm, ...)
#endif

#define MAIN_LOG(level, fm, ...) MAIN_##level(fm, ##__VA_ARGS__)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void rcc_clock_cmd(void)
{
	/* Enable GPIO Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
#ifdef STM32F303xE
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
#endif
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* Enable SPI Clock */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

#if defined (STM32F303xE)
	/* ADC12 Peripheral clock enable */
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
#elif defined (STM32F051)
	/* ADC1 Peripheral clock enable */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
#endif
}

void rcc_config(void)
{
#if 1
#if (CBA_PLATFORM == PLATFORM_STM32)
	RCC_DeInit();

	/* enable LSI to iwwdg */
	RCC_LSICmd(ENABLE);

#if defined (__STM32F30X)
	/* Enable Prefetch Buffer and set Flash Latency */
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_1);

	/* AHB CLK(HCLK) = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* APB1 CLK(PCLK1) = HCLK/2 */
	RCC_PCLK1Config(RCC_HCLK_Div2);
	/* APB2 CLK(PCLK2) = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1);
#elif defined (__STM32F0XX)
	/* AHB CLK(HCLK) = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* APB CLK(PCLK) = HCLK */
	RCC_PCLKConfig(RCC_HCLK_Div1);
#endif

	RCC_HSEConfig(RCC_HSE_ON);

	if(RCC_WaitForHSEStartUp()) {
		RCC_HSICmd(DISABLE);
#if defined (STM32F303xE)
		/* HSE = 8M */
		/* PLLCLK = (PLL/1) * 9 = 72M */
		RCC_PREDIV1Config(RCC_PREDIV1_Div1);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#elif defined (__STM32F0XX)
		/* HSE = 24M */
		/* PLLCLK = (PLL) * 2 = 48M */
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_2);
#endif
	}
	else {
		//RCC_ClockSecuritySystemCmd(ENABLE);
		RCC_HSICmd(ENABLE);
		RCC_HSEConfig(RCC_HSE_OFF);
#if defined (STM32F303xE)
		/* PLLCLK = (PLL/1) * 9 = 72M */
		RCC_PREDIV1Config(RCC_PREDIV1_Div1);
		RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_9);
#elif defined (__STM32F0XX)
		/* PLLCLK = (PLL/2) * 12 = 48M */
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
#endif
	}

	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

	/* SYSCLK = PLLCLK */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);

#endif

#endif /* 0 */
	rcc_clock_cmd();
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
	   this is done through SystemInit() function which is called from startup
	   file (startup_stm32f0xx.s) before to branch to application main.
	   To reconfigure the default setting of SystemInit() function, refer to
	   system_stm32f0xx.c file
	 */
	//rcc_config();
	console_init_early();
	cmd_init();
	timer_init();
	event_init();
	console_init();
	cobra_sys_init();

	/* Infinite loop */
	while (1) {
		timer_task_handle();
		event_monitor_handle();
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
