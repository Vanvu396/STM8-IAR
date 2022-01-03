#include "ES-SM-TH-01.h"
#include "string.h"

extern uint8_t data_rx[7];

uint8_t temperature[8] = {0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xCF};

uint8_t humidity[8] = {0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0F};


void ES_SM_Init(void){
UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  /* Enable UART */
  UART1_Cmd(ENABLE);
    /* Enable general interrupts */
  enableInterrupts();
}

void UART_Transmit(uint8_t *pData, uint8_t len, uint16_t _timeout){
  uint32_t timeout = millis();
  
  for(uint8_t i = 0; i < len; i++){
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET){
      if(millis() - timeout >= _timeout){
        timeout = millis();
        break;
      }
    }
  UART1_SendData8(pData[i]);
  Delay_ms(1);
  }
    
}

void check_sum_modebus_16(const uint8_t *buf, uint8_t *result, unsigned int len){
  uint16_t crc = 0xFFFF;
  unsigned int i = 0;
  char bit = 0;

  for( i = 0; i < len; i++ ){
    crc ^= buf[i];
    for( bit = 0; bit < 8; bit++ ){
      if( crc & 0x0001 )
    {
      crc >>= 1;
      crc ^= 0xA001;
    }else{
      crc >>= 1;
    }
    }
  }
   result[0] = crc;
   result[1] = crc>>8;
}

uint8_t get_temperature(uint8_t *pData){
  
  uint8_t temp_buff[5];
  uint8_t temp_cs[2];
  
  UART_Transmit(temperature, sizeof(temperature), 100);
  
  memcpy(temp_buff, data_rx, 5);
  
  check_sum_modebus_16(data_rx, temp_cs, 5);
  
  if(memcmp(data_rx+5, temp_cs, 2) == 0){
    uint16_t temp_resuil;
    temp_resuil = temp_buff[3]<<8|temp_buff[4];
    pData[0] = temp_resuil/10;
    pData[1] = temp_resuil % 10;
    return 1;
  }else{
    return 0;
  }
}

uint8_t get_humidity(uint8_t *pData){
  uint8_t temp_buff[5];
  uint8_t temp_cs[2];
  
  UART_Transmit(humidity, sizeof(humidity), 100);
  
  memcpy(temp_buff, data_rx, 5);
  
  check_sum_modebus_16(data_rx, temp_cs, 5);
  
  if(memcmp(data_rx+5, temp_cs, 2) == 0){
    uint16_t temp_resuil;
    temp_resuil = temp_buff[3]<<8|temp_buff[4];
    pData[0] = temp_resuil/10;
    pData[1] = temp_resuil % 10;
    return 1;
  }else{
    return 0;
  }
}

  
uint8_t get_soil_moisture(uint8_t *pData){
  uint8_t status_humi, status_temp;
  
  uint8_t _humidity[2];
  uint8_t _temperature[2];
  
  if(get_humidity(_humidity)){
    status_humi =1;
    memcpy(pData+2, _humidity, 2);
  }else{
    status_humi =0;
  }
  
  Delay_ms(200);
  
  if(get_temperature(_temperature)){
    status_temp =1;
    memcpy(pData, _temperature, 2);
  }else{
    status_temp =0;
  }

  Delay_ms(200);
  
  
  if(status_humi && status_temp){
    memset(data_rx, 0, sizeof(data_rx));
    return 1;
  }else{
    return 0;
  }
  
}