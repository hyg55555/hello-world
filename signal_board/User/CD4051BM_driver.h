#ifndef CD4051BM_DRIVER_H
#define CD4051BM_DRIVER_H

#include "stm32f1xx_hal.h"

typedef enum
{
    // CD4051BM1
    HEAT1,
    HEAT2,
    HEAT3,
    HEAT4,
    HEAT5,
    HEAT6,
    HEAT7,
    HEAT8,
    // CD4051BM2
    HEAT9,
    HEAT10,
    HEAT11,
    HEAT12,
    HEAT13,
    HEAT14,
    HEAT15,
    HEAT16,
    // CD4051BM3
    HEAT17,
    HEAT18,
    HEAT19,
    HEAT20,
    // CD4051BM4
    RELAY1 = 24,
    RELAY2,
    RELAY3,
    RELAY4,
    RELAY5,
    RELAY6,
    RELAY7,
    RELAY8,
    // CD4051BM5
    RELAY9,
    RELAY10,
    RELAY11,
    RELAY12,
    RELAY13,
    RELAY14,
    RELAY15,
    RELAY16,
    // CD4051BM6
    RELAY17,
    RELAY18,
    RELAY19,
    RELAY20,
    DETECT_MAX = 48
} CD4051BM_DETECT_ID;

typedef void (*detect_callback)(CD4051BM_DETECT_ID id);

void CD4051BM_scan(void);
void get_CD4051BM_state(uint8_t *sta, uint8_t index);

#endif // !CD4051BM_DRIVER_H
