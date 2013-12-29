
#include "../inc/receiver.h"

volatile u16 receiver_position[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
volatile u16 receiver_position_read[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
volatile u16 receiver_count = 0;

void receiver_gpio_init() {
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = RECEIVER_PORTS;
	GPIO_Config.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Config.GPIO_OType = GPIO_OType_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RECEIVER_REGISTER, &GPIO_Config);
}

void receiver_init() {
	// ---------- Interrupt configuration for the servos on TIM3 ---------- //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// ---------- TIM3 / Time Management configuration ---------- //
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	// STM32F4 runs at 84 MHz and we need 50Hz (20 milliseconds)
	TIM_TimeBaseStructure.TIM_Prescaler = RECEIVER_TIM_PRESCALE - 1; // Prescale the timer to 1 MHz
	TIM_TimeBaseStructure.TIM_Period = RECEIVER_TIM_PERIOD - 1; // 1 Hz / 20 = 50000 Hz (20000 ms)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

u16 receiver_get_pos(u16 num) {
	if ((num <= sizeof(receiver_position)/sizeof(u16)) && (receiver_position[num] <= RECEIVER_TIM_MICROSECOND)) {
		return receiver_position[num];
	}
	return 0;
}

/**
 * Interrupt handler for TIM2
 */
void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) {
		u16 port = 0;
		
		// Read out all receiver ports
		if (RECEIVER_REGISTER->IDR & RECEIVER1) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER2) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER3) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER4) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER5) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER6) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER7) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		port++;
		
		if (RECEIVER_REGISTER->IDR & RECEIVER8) {
			receiver_position_read[port]++;
		} else if (receiver_position_read[port] >= RECEIVER_TIM_MICROSECOND) {
			receiver_position[port] = receiver_position_read[port] - RECEIVER_TIM_MICROSECOND;
			receiver_position_read[port] = 0;
		}
		
	}
}
