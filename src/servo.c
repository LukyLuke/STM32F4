
#include "../inc/servo.h"

volatile float servo_angle[4] = { 0, 0, 0, 0 };
volatile u16 servo_period[4] = { 0, 0, 0, 0 };
volatile u16 servo_count = 0;

void servo_gpio_init() {
	GPIO_InitTypeDef GPIO_Config;
	GPIO_Config.GPIO_Pin = SERVO_PORTS;
	GPIO_Config.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Config.GPIO_OType = GPIO_OType_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SERVO_REGISTER, &GPIO_Config);
	
	//GPIO_PinAFConfig(SERVO_REGISTER, GPIO_PinSource0, GPIO_AF_TIM3);
	//GPIO_PinAFConfig(SERVO_REGISTER, GPIO_PinSource1, GPIO_AF_TIM3);
	//GPIO_PinAFConfig(SERVO_REGISTER, GPIO_PinSource2, GPIO_AF_TIM3);
	//GPIO_PinAFConfig(SERVO_REGISTER, GPIO_PinSource3, GPIO_AF_TIM3);
}

void servo_init() {
	u16 prescalerValue = (u16)((SystemCoreClock / 2) / SERVO_TIM_PRESCALE) - 1;
	
	// ---------- Interrupt configuration for the servos on TIM3 ---------- //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	// ---------- TIM3 / Time Management configuration ---------- //
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	// STM32F4 runs at 84 MHz and we need 50Hz (20 milliseconds)
	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue; // Prescale the timer to 1 MHz
	TIM_TimeBaseStructure.TIM_Period = SERVO_TIM_PERIOD - 1; // 1 Hz / 20 = 50000 Hz (20000 ms)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	// Set up 4 channels for servo
	/*
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); // Channel 1 configuration = SERVO1 TIM3_CH1
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); // Channel 2 configuration = SERVO2 TIM3_CH2
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); // Channel 3 configuration = SERVO3 TIM3_CH3
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); // Channel 4 configuration = SERVO4 TIM3_CH4
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	*/
	
	// Turning on TIM3 and PWM outputs
	//TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);
}

void servo_set_pos(u16 num, u16 position) {
	if (num <= sizeof(servo_angle)/sizeof(float)) {
		servo_angle[num] = position % (SERVO_TIM_COUNTER - 1);
	}
}

/**
 * Interrupt handler for TIM3
 */
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update)) {
		u16 ports_on = 0;
		u16 ports_off = 0;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		// Count down from 1000 to 0
		servo_count--;
		if (!servo_count) {
			servo_period[0] = servo_angle[0] + SERVO_TIM_MICROSECOND;
			servo_period[1] = servo_angle[1] + SERVO_TIM_MICROSECOND;
			servo_period[2] = servo_angle[2] + SERVO_TIM_MICROSECOND;
			servo_period[3] = servo_angle[3] + SERVO_TIM_MICROSECOND;
			servo_count = SERVO_TIM_COUNTER;
			GPIO_ResetBits(SERVO_REGISTER, SERVO_PORTS);
			return;
		}
		
		// Lower servo ticks
		servo_period[0]--;
		servo_period[1]--;
		servo_period[2]--;
		servo_period[3]--;
		
		// Check whichs ervo must be on and wich off
		if (servo_period[0]) {
			ports_on |= SERVO1;
		} else {
			ports_off |= SERVO1;
		}
		
		if (servo_period[1]) {
			ports_on |= SERVO2;
		} else {
			ports_off |= SERVO2;
		}
		
		if (servo_period[2]) {
			ports_on |= SERVO3;
		} else {
			ports_off |= SERVO3;
		}
		
		if (servo_period[3]) {
			ports_on |= SERVO4;
		} else {
			ports_off |= SERVO4;
		}
		
		// Set adn Unset ports
		GPIO_SetBits(SERVO_REGISTER, ports_on);
		GPIO_ResetBits(SERVO_REGISTER, ports_off);
		
		//TIM3->CCR1 = 1000;// + (u32)(servo_angle[0] * servo_step);
		//TIM3->CCR2 = 1000 + (u32)(servo_angle[1] * servo_step);
		//TIM3->CCR3 = 1000 + (u32)(servo_angle[2] * servo_step);
		//TIM3->CCR4 = 1000 + (u32)(servo_angle[3] * servo_step);
	}
}
