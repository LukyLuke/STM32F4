
#include "inc/config.h"

int main(void) {
	/*if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1);
	}*/

	// Deinitialize all GPIO registers
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
	GPIO_DeInit(GPIOG);
	GPIO_DeInit(GPIOH);
	GPIO_DeInit(GPIOI);
	
	// Initialize all required GPIO ports and registers
	init_gpio();
	servo_gpio_init();
	receiver_gpio_init();
	
	// Interrupt priority: 2 bits for priority and 2 bits for sub priority
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	// Initialize all systems, inetrrupts, etc.
	servo_init();
	receiver_init();
	
	// Just initialize some dummy LED values to toggle them for testing
	GPIO_SetBits(LED_REGISTER, LED3 | LED4);
	GPIO_ResetBits(LED_REGISTER, LED1 | LED2);
	
	while (1) {
		wait(5000);
		
		//GPIO_ToggleBits(LED_REGISTER, LED1 | LED2 | LED3 | LED4);
		
		// The first receiver port controls the LEDs
		if (receiver_get_pos(1) > 0) {
			GPIO_SetBits(LED_REGISTER, LED3 | LED4);
			GPIO_ResetBits(LED_REGISTER, LED1 | LED2);
		} else {
			GPIO_SetBits(LED_REGISTER, LED1 | LED2);
			GPIO_ResetBits(LED_REGISTER, LED3 | LED4);
		}
		
		//servo_set_pos(0, servo_angle[0] + 10);
		//servo_set_pos(1, servo_angle[1] + 10);
		//servo_set_pos(2, servo_angle[2] + 10);
		//servo_set_pos(3, servo_angle[3] + 10);
	};

	return 0;
}

/**
 * Initialize the system.
 */
void init_gpio() {
	GPIO_InitTypeDef GPIO_Config;

	// GPIOx Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	// ---------- LED Configuration ---------- //
	// Configure all LED-Ports in output pushpull mode
	GPIO_Config.GPIO_Pin = LED_PORTS;
	GPIO_Config.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Config.GPIO_OType = GPIO_OType_PP;
	GPIO_Config.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_REGISTER, &GPIO_Config);
}

/**
 * Wait for the given amount of micro seconds.
 * This is NOT precise!
 */
void wait(__IO u32 micro) {
	for (u32 count = HSI_VALUE / 2000; count > 0; count--) {
		delay(micro);
	}
}

/**
 * A simple counter for a delay
 */
void delay(__IO u32 nCount) {
	while (nCount--);
}

/**
 * To prevent compilation errors.
 */
void _init() {}
