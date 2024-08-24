#include "debug.h"

// UART TX<=>D6
// SINUS<=>A2

#define SINUS_FREQ  1000

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

static void Init_Sinus(uint32_t freq) {
    const uint8_t SINUS[] = {
#if SINUS_FREQ > 15000
        63, 89, 110, 123, 126, 118, 100, 76, 50, 26, 8, 0, 3, 16, 37
#else
        63,  79,  93,  106, 116, 123, 126, 125, 120, 112, 100, 86,  71,  55,  40,  26,
        14,  6,   1,   0,   3,   10,  20,  33,  47
#endif
    };

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
    TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
    DMA_InitTypeDef DMA_InitStructure = { 0 };

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = 127; // 7 bit PWM
    TIM_TimeBaseStructure.TIM_Prescaler = (48000000 / 128) / (ARRAY_SIZE(SINUS) * freq) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CH2CVR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SINUS;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = ARRAY_SIZE(SINUS);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
//    TIM_GenerateEvent(TIM1, TIM_EventSource_Update);
}

int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk: %u\r\n", SystemCoreClock);
    printf("Generating sinus %u Hz on A2\r\n", SINUS_FREQ);

    Init_Sinus(SINUS_FREQ);

    while(1) {}
}
