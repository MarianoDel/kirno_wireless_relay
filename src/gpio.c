//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### GPIO.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "stm32f0xx.h"


//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    unsigned long temp;

    //--- MODER ---//
    //00: Input mode (reset state)
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved


#ifdef GPIOA_ENABLE

    //--- GPIO A ---//
    if (!GPIOA_CLK)
	GPIOA_CLK_ON;

    temp = GPIOA->MODER;	//2 bits por pin
    temp &= 0xFC3F0CCF;    //PA2 input; PA4 input or alternative; PA6 PA7 outputs
    temp |= 0x00405000;    //PA11 output PA12 input
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;    //1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;    //2 bits por pin
    temp &= 0xFF3F0FFF;    // PA6 PA7 low speed
    temp |= 0x00000000;    // PA11 low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;    //2 bits por pin
    temp &= 0xFCFFFFCF;    // PA2 PA12 pullup
    temp |= 0x01000010;
    GPIOA->PUPDR = temp;

    //Alternate Fuction
    //GPIOA->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1


#endif

#ifdef GPIOB_ENABLE

    //--- GPIO B ---//
    if (!GPIOB_CLK)
	GPIOB_CLK_ON;

    temp = GPIOB->MODER;    //2 bits por pin
    temp &= 0xFFFFFFF0;    //PB0 PB1 output
    temp |= 0x00000005;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;    //1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;    //2 bits por pin
    temp &= 0xFFFFFFFF;    //low speed
    temp |= 0x00000000;
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;    //2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->PUPDR = temp;

    //Alternate Fuction
    //GPIOB->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

#endif

#ifdef GPIOF_ENABLE

    //--- GPIO F ---//
    if (!GPIOF_CLK)
	GPIOF_CLK_ON;

    temp = GPIOF->MODER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->MODER = temp;

    temp = GPIOF->OTYPER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OTYPER = temp;

    temp = GPIOF->OSPEEDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OSPEEDR = temp;

    temp = GPIOF->PUPDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->PUPDR = temp;

#endif

}
//--- end of file ---//
