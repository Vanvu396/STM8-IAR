#include "sht3x.h"


void SHT30_Init(void){
  I2C_DeInit();
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
  
  GPIO_Init(GPIOB , GPIO_PIN_4 , GPIO_MODE_IN_PU_NO_IT);   
  GPIO_Init(GPIOB , GPIO_PIN_5 , GPIO_MODE_IN_PU_NO_IT);
  
  I2C_Init(100000, SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
  
  I2C_Cmd(ENABLE);
  
}

//--------------------------------------

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
  Delay_ms(50);
  
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  }
  
  I2C_SendData(pData[0]);
  
  Delay_ms(50);
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  if(len > 1){
    for(uint8_t i = 1; i < len; i++){
      I2C_SendData(pData[i]);
      Delay_ms(50);
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

//-------------------------------

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
  
  while(I2C_GetFlagStatus( I2C_FLAG_ADDRESSSENTMATCHED) == RESET);
  
  Delay_ms(100);
  
   
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
    if(millis() - timeout >= _timeout){
      timeout = millis();
      break;
    }
  };
  
  //clear ACK
  I2C_AcknowledgeConfig(I2C_ACK_NEXT);
  
  Delay_ms(100);

  pData[0] = I2C_ReceiveData();
   I2C_AcknowledgeConfig(I2C_ACK_NEXT);
  Delay_ms(100);
  
  while(I2C_GetFlagStatus(I2C_FLAG_STOPDETECTION ) == SET);
     
  if(len > 1){
    for(uint8_t i = 1; i < len; i++){
       while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)){
          if(millis() - timeout >= _timeout){
            timeout = millis();
            break;
          }
       };
       pData[i] = I2C_ReceiveData();
       Delay_ms(100);
       while(I2C_GetFlagStatus(I2C_FLAG_STOPDETECTION ) == SET);
       
  
    }
  }
  while(I2C->CR2 & I2C_CR2_STOP);
  
   I2C_GenerateSTOP(ENABLE);
}

//----------------------------------
static uint8_t calculate_crc(const uint8_t *data, uint8_t length){
  uint8_t crc = 0xff;
  for (uint8_t i = 0; i < length; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if ((crc & 0x80u) != 0){
        crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
      }else{
        crc <<= 1u;
      }
    }
  }
return crc;
}


//-------------------------------------------------

static uint16_t uint8_to_uint16(uint8_t msb, uint8_t lsb){
  return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

//--------------------------------------------------

uint8_t get_sht30(uint8_t *pData){
  uint8_t temperature_crc, humidity_crc;
  uint16_t temperature, humidity;
  uint8_t buffer[7];
  uint8_t command_buffer[2] = {(SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH & 0xff00u) >> 8u, SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH & 0xffu};

  I2C_Transmit(SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW << 1u, command_buffer, 2, 10);
  Delay_ms(1);
  I2C_Receive(SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_LOW <<1u, buffer, 7, 10);
  temperature_crc = calculate_crc(buffer, 2);
  humidity_crc = calculate_crc(buffer + 4, 2);

  if (temperature_crc == buffer[3] && humidity_crc == buffer[6]){
    int16_t temperature_raw = (int16_t)uint8_to_uint16(buffer[0], buffer[1]);
    uint16_t humidity_raw = uint8_to_uint16(buffer[4], buffer[5]);

    temperature = (uint16_t)((-45.0f + 175.0f * (float)temperature_raw / 65535.0f)*100);
    humidity = (uint16_t)((100.0f * (float)humidity_raw / 65535.0f)*100);
    
    pData[0] = humidity / 100;
    pData[1] = humidity % 100;
    
    pData[2] = temperature / 100;
    pData[3] = temperature % 100;
    
    return 1;
  }else{
    return 0;
  }

}


