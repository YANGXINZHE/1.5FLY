/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "sysconfig.h"
#include "sys.h"
#include "adc.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
/*=====================================================================================================*/
/*=====================================================================================================*/
#include "tim3.h"
#include "MPU6050.h"
//#include "MS5611.h"
#include "data_transfer.h"
u32				Debug_cnt;
u32 debug_cntshow=0;
bool BT_LOW = FALSE;
u16 link_num = 2000;

void TIM3_IRQHandler(void)		//0.5ms中断一次
{	
	static u8 ms1_cnt=0;
	static u16 s1_cnt=0;
	
	if(TIM3->SR & TIM_IT_Update)		//if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{     
		TIM3->SR = ~TIM_FLAG_Update;//TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);   //清除中断标志
		TIM3_IRQCNT ++;
		
		ms1_cnt++;
		s1_cnt++;
		if(ms1_cnt==2)//1ms
		{
			ms1_cnt = 0;
			MPU6050_Read();
			FLAG_ATT = 1;
		}
		else
		{
			;//MS5611_loop();
		}
		if(s1_cnt==2000)
		{
			s1_cnt = 0;
			debug_cntshow = Debug_cnt;
			Debug_cnt = 0;
		}
	 //if(PBin(9)==1) 
	if(DEBUG_EN == TRUE)		 
		Data_Exchange();
	}
}
#include "tim_pwm_in.h"
void TIM4_IRQHandler(void)		//0.5ms中断一次
{	
	Tim4_Pwm_In_Irq();
}
//#ifdef USE_USART1
void USART1_IRQHandler(void)  //串口中断函数
{
	//if(DEBUG_EN == TRUE)
		Uart1_IRQ();
	//link_num = 1000;
}
//#else
void USART2_IRQHandler(void)  //串口中断函数
{
//	if(DEBUG_EN == FALSE)
		Uart2_IRQ();
}
//#endif

#ifdef USE_IIC1
void I2C1_EV_IRQHandler(void)
{
  i2cInterruptHandlerI2c1();
}

void I2C1_ER_IRQHandler(void)
{
  i2cErrorInterruptHandlerI2c1();
}
#else
void I2C2_EV_IRQHandler(void)
{

}

void I2C2_ER_IRQHandler(void)
{
  I2C_ClearFlag(I2C2, 0x1000FFFF);
}
#endif

/** @param  This function handles DMA1_Channel1 interrupt request
  * @param  None
  * @retval None
  */
bool BT_low_state=false;
void DMA1_Channel1_IRQHandler(void)
{
 static u8 low_num =0;
	DMA_ClearITPendingBit(DMA1_IT_TC1);

	if(((4096 * 247)/ADC_ConvertedValue)<300)//计算电源电压小于3V
	{
		if(low_num++ >200)
		{ 
			BT_low_state = true;
		}
	}
	else
	{
		low_num=0;
		//low_state = 0;
	}
		
}
 
/*=====================================================================================================*/
/*=====================================================================================================*/
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	  GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	  GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	  GPIO_ResetBits(GPIOA, GPIO_Pin_12);
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
