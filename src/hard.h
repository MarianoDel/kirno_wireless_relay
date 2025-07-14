//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### HARD.H ################################
//---------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_


// Module Exported Types Constants and Macros ----------------------------------
#include "switches_answers.h"


//----------- Defines For Configuration -------------


//---- Configuration for Hardware Versions ----------
#define HARDWARE_VERSION_1_0    // pre-prod 20 units


//---- Software Version and Configs -----------------
#define SOFTWARE_VERSION_1_0    // first release


//---- Hardware Gpios Config ------------------------
#if (defined HARDWARE_VERSION_1_0)
//GPIOA pin0
//GPIOA pin1    NC

//GPIOA pin2
#define DET_AC    ((GPIOA->IDR & 0x0004) != 0)

//GPIOA pin3    NC

//GPIOA pin4    or Alternative TIM14_CH1
#define RX_CODE ((GPIOA->IDR & 0x0010) != 0)

//GPIOA pin5    NC

//GPIOA pin6
#define ACT_CH1    ((GPIOA->ODR & 0x0040) == 0)
#define ACT_CH1_OFF    (GPIOA->BSRR = 0x00000040)
#define ACT_CH1_ON    (GPIOA->BSRR = 0x00400000)

//GPIOA pin7
#define ACT_CH2    ((GPIOA->ODR & 0x0080) == 0)
#define ACT_CH2_OFF    (GPIOA->BSRR = 0x00000080)
#define ACT_CH2_ON    (GPIOA->BSRR = 0x00800000)

//GPIOA pin8
//GPIOA pin9
//GPIOA pin10    NC

//GPIOA pin11
#define LED    ((GPIOA->ODR & 0x0800) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00000800)
#define LED_OFF    (GPIOA->BSRR = 0x08000000)

//GPIOA pin12
#define SW_LEARN    ((GPIOA->IDR & 0x1000) == 0)

//GPIOA pin13
//GPIOA pin14
//GPIOA pin15    NC

//GPIOB pin0
#define ACT_CH3    ((GPIOB->ODR & 0x0001) != 0)
#define ACT_CH3_ON    (GPIOB->BSRR = 0x00000001)
#define ACT_CH3_OFF    (GPIOB->BSRR = 0x00010000)

//GPIOB pin1
#define ACT_CH4    ((GPIOB->ODR & 0x0002) != 0)
#define ACT_CH4_ON    (GPIOB->BSRR = 0x00000002)
#define ACT_CH4_OFF    (GPIOB->BSRR = 0x00020000)

//GPIOB pin3
//GPIOB pin4
//GPIOB pin5
//GPIOB pin6
//GPIOB pin7    NC

#endif    // HARDWARE_VERSION_1_0
//---- end of Hardware Gpios Config --------------------


// Module Exported Functions ---------------------------------------------------
unsigned char Led_Is_On (void);
void Led_On (void);
void Led_Off (void);
unsigned char Sw_Learn_Is_On (void);
void Hard_Timeouts (void);
resp_sw_t Check_Sw_Learn (void);

void Hard_Led_Blinking_Update (void);
void Hard_Led_Change_Bips (unsigned char how_many,
			   unsigned short timer_on,
			   unsigned short timer_off);

void Relay_Ch1_On (void);
void Relay_Ch2_On (void);
void Relay_Ch3_On (void);
void Relay_Ch4_On (void);
void Relay_Ch1_Off (void);
void Relay_Ch2_Off (void);
void Relay_Ch3_Off (void);
void Relay_Ch4_Off (void);
unsigned char Relay_Ch1_Is_On (void);
unsigned char Relay_Ch2_Is_On (void);
unsigned char Relay_Ch3_Is_On (void);
unsigned char Relay_Ch4_Is_On (void);


#endif /* _HARD_H_ */

//--- end of file ---//

