/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
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
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_tim4.h"
#include "stm8s_it.h"
#include "stm8s_i2c.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint8_t data[6];

uint8_t command_buffer[2] = {(0x2c06 & 0xff00u) >> 8u, 0x2c06 & 0xffu};

void I2C_Transmit(uint8_t I2C_Slave_Address, uint8_t *pData, uint8_t len, uint8_t _timeout);

void I2C_Receive(uint8_t I2C_Slave_Address, uint8_t *pData, uint8_t len, uint8_t _timeout);


void main(void)
{
   CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
   /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
  
  I2C_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
  
 GPIO_Init(GPIOB , GPIO_PIN_4 , GPIO_MODE_IN_PU_NO_IT);   
  GPIO_Init(GPIOB , GPIO_PIN_5 , GPIO_MODE_IN_PU_NO_IT);
  
  I2C_Init(100000, 0x44, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
  
  I2C_Cmd(ENABLE);
  
  

  /* Infinite loop */
  while (1)
  {
      I2C_Transmit(0x44<<1u, command_buffer, 2, 100);
      Delay_ms(200);
      I2C_Receive(0x44<<1u, data, 6, 100);
      
      Delay_ms(1000);
      
  }
  
}


void I2C_Transmit(uint8_t I2C_Slave_Address, uint8_t *pData, uint8_t len, uint8_t _timeout){
  uint8_t timeout = millis();

  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  }
  
  I2C_GenerateSTART(ENABLE);
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  }
  
  I2C_Send7bitAddress(I2C_Slave_Address,I2C_DIRECTION_TX);
  
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  }
  
  I2C_SendData(pData[0]);
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  if(len > 1){
    for(uint8_t i = 1; i < len; i++){
      I2C_SendData(pData[i]);
      while(!I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED)){
        if(millis() - timeout >= _timeout){
          timeout = millis();
          break;
        }
      }
 }
 
  }
 I2C_GenerateSTOP(ENABLE);
 
 I2C->SR1;        I2C->SR3;
  
 
}


void I2C_Receive(uint8_t I2C_Slave_Address, uint8_t *pData, uint8_t len, uint8_t _timeout){
  uint8_t timeout = millis();
  
  I2C_GenerateSTART(ENABLE);

  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  };
        
  I2C_Send7bitAddress(I2C_Slave_Address,I2C_DIRECTION_RX);
        
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  };
  
  pData[0] = I2C_ReceiveData();
   
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
        
  if(len > 1){
    for(uint8_t i = 1; i < len; i++){
       while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)){
          if(millis() - timeout >= _timeout){
            timeout = millis();
            break;
          }
       };
       pData[i] = I2C_ReceiveData();
       I2C_AcknowledgeConfig(I2C_ACK_CURR);
    }
  }
  while(I2C->CR2 & I2C_CR2_STOP);
   I2C_GenerateSTOP(ENABLE);
}
 




#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
