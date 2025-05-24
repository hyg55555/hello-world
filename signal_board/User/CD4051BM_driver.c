#include "main.h"
#include "CD4051BM_driver.h"

static GPIO_PinState detect_state1;
static GPIO_PinState detect_state2;
static GPIO_PinState detect_state3;
static GPIO_PinState detect_state4;
static GPIO_PinState detect_state5;
static GPIO_PinState detect_state6;

static GPIO_PinState last_state[DETECT_MAX] = {0};

static detect_callback *detect_cb;
void key_driver_register(detect_callback *cb)
{
    detect_cb = cb;
}

static void CD4051BM_delay()
{
	uint16_t delay = 100;
	while(delay--);
}

/**
 * @brief CD4051BM控制器，读取哪一位
 * 
 * @param num 
 */
static void CD4051BM_ctrller(uint8_t num)
{
    GPIO_PinState s1 = num & 0x01 > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET;
    GPIO_PinState s2 = (num >> 1) & 0x01 > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET;
    GPIO_PinState s3 = (num >> 2) & 0x01 > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(CTRL_A2_GPIO_Port, CTRL_A2_Pin, s3);
    HAL_GPIO_WritePin(CTRL_A1_GPIO_Port, CTRL_A1_Pin, s2);
    HAL_GPIO_WritePin(CTRL_A0_GPIO_Port, CTRL_A0_Pin, s1);

    CD4051BM_delay();
}

void CD4051BM_scan(void)
{
    uint8_t i = 0;
    for(i = 0; i < 8; i++)
    {
        CD4051BM_ctrller(i);

        detect_state1 = HAL_GPIO_ReadPin(DETECT_IN1_GPIO_Port, DETECT_IN1_Pin);
        detect_state2 = HAL_GPIO_ReadPin(DETECT_IN2_GPIO_Port, DETECT_IN2_Pin);
        detect_state3 = HAL_GPIO_ReadPin(DETECT_IN3_GPIO_Port, DETECT_IN3_Pin);
        detect_state4 = HAL_GPIO_ReadPin(DETECT_IN4_GPIO_Port, DETECT_IN4_Pin);
        detect_state5 = HAL_GPIO_ReadPin(DETECT_IN5_GPIO_Port, DETECT_IN5_Pin);
        detect_state6 = HAL_GPIO_ReadPin(DETECT_IN6_GPIO_Port, DETECT_IN6_Pin);

        last_state[i] = detect_state1;
        last_state[i + 8] = detect_state2;
        last_state[i + 16] = detect_state3;
        last_state[i + 24] = detect_state4;
        last_state[i + 32] = detect_state5;
        last_state[i + 40] = detect_state6;
    }
}

void get_CD4051BM_state(uint8_t *sta, uint8_t index)
{
    //将状态依次放置到sta bit中
    for (uint8_t i = 0; i < 8; i++)
    {
        *sta |= last_state[index*8 + i] << i;
    }
}