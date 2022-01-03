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
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "ES-SM-TH-01.h"
#include "sht3x.h"
#include "swuart.h"
#include "string.h"
#include "stdio.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

 uint8_t status_sht30, status_soil_moisture;

  uint8_t HEADER[1] = "$";
  uint8_t ID[2] = {0,1};
  uint8_t value[12];
  
  uint8_t AIR[4];
    uint8_t LAND[4];
    uint8_t CRC[1];
  
    
    typedef union DCD{
      struct{
      uint8_t Header[1];
      uint8_t ID[2];
      uint8_t AIR[4];
      uint8_t LAND[4];
      uint8_t CRC[1];
      };
      uint8_t value[12];
    }DCD_FRAMES;

    
    
 void Check_Sum_8(uint8_t* _data, uint8_t* _value, uint8_t _position_Star, uint8_t _position_End){
   uint16_t Sum = 0;
   for(int i = _position_Star; i<= _position_End; i++){
     Sum += _data[i];
    }
  _value[0] = Sum & 0xFF;
}

void main(void)
{
  Delay_Init();
  uart_init();
  SHT30_Init();
  ES_SM_Init();
  
  
  DCD_FRAMES dcd_frame;
  
  memcpy(dcd_frame.Header, HEADER, 1);
  memcpy(dcd_frame.ID, ID, 2);
 
  

  /* Infinite loop */
  while (1)
  {
    /*
   
    if(get_sht30(AIR)){
      memcpy(dcd_frame.AIR, AIR, 4);
       
      if(get_soil_moisture(LAND)){
        memcpy(dcd_frame.LAND, LAND, 4);
        Check_Sum_8(dcd_frame.value, CRC, 0, 10);
        memcpy(dcd_frame.CRC, CRC, 1);
        memcpy(value, dcd_frame.value, 12);
        UART_SW_Transmit(dcd_frame.value, sizeof(dcd_frame.value));
        
      }
    }*/
    
    UART_SW_Transmit((uint8_t*)"Hello\n", 7);
  
    
    Delay_ms(1000);
  
    
    
  }
  
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
