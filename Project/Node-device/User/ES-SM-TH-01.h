#ifndef __ES_SM_TH_H
#define __ES_SM_TH_H
#include "stm8s_uart1.h"
#include "stm8s_it.h"

void ES_SM_Init(void);

uint8_t get_soil_moisture(uint8_t *pData);



#endif