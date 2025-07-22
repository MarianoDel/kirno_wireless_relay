//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### TIM.C #################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "det_ac.h"


// Module Private Types Constants and Macros -----------------------------------
#define RCC_TIM1_CLK    (RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON    (RCC->APB2ENR |= 0x00000800)
#define RCC_TIM1_CLK_OFF    (RCC->APB2ENR &= ~0x00000800)

#define RCC_TIM3_CLK    (RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON    (RCC->APB1ENR |= 0x00000002)
#define RCC_TIM3_CLK_OFF    (RCC->APB1ENR &= ~0x00000002)

#define RCC_TIM14_CLK    (RCC->APB1ENR & 0x00000100)
#define RCC_TIM14_CLK_ON    (RCC->APB1ENR |= 0x00000100)
#define RCC_TIM14_CLK_OFF    (RCC->APB1ENR &= ~0x00000100)

#define RCC_TIM15_CLK    (RCC->APB2ENR & 0x00010000)
#define RCC_TIM15_CLK_ON    (RCC->APB2ENR |= 0x00010000)
#define RCC_TIM15_CLK_OFF    (RCC->APB2ENR &= ~0x00010000)

#define RCC_TIM16_CLK    (RCC->APB2ENR & 0x00020000)
#define RCC_TIM16_CLK_ON    (RCC->APB2ENR |= 0x00020000)
#define RCC_TIM16_CLK_OFF    (RCC->APB2ENR &= ~0x00020000)

#define RCC_TIM17_CLK    (RCC->APB2ENR & 0x00040000)
#define RCC_TIM17_CLK_ON    (RCC->APB2ENR |= 0x00040000)
#define RCC_TIM17_CLK_OFF    (RCC->APB2ENR &= ~0x00040000)


// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;



// Globals ---------------------------------------------------------------------


// Module Functions ------------------------------------------------------------
void Update_TIM1_CH1 (unsigned short a)
{
    TIM1->CCR1 = a;
}


void Update_TIM3_CH1 (unsigned short a)
{
    TIM3->CCR1 = a;
}


void Update_TIM3_CH2 (unsigned short a)
{
    TIM3->CCR2 = a;
}


void Wait_ms (unsigned short wait)
{
    wait_ms_var = wait;
    while (wait_ms_var);
}


void TIM3_IRQHandler (void)	//1 ms
{
    if (TIM3->SR & 0x01)	//bajo el flag
	TIM3->SR = 0x00;
}


// void TIM1_BRK_UP_TRG_COM_IRQHandler (void)	//16KHz
// {
// 	//bajar flag
// 	if (TIM1->SR & 0x01)	//bajo el flag
// 		TIM1->SR = 0x00;

// }

void TIM_1_Init (void)
{
    unsigned long temp;
    
    if (!RCC_TIM1_CLK)
        RCC_TIM1_CLK_ON;

    // timer config
    //TIM1->CR1 = 0x00;		//clk int / 1; upcounting ARPE bit in the TIMx_CR1 para pwm mode 1
    TIM1->CR1 = TIM_CR1_ARPE;
    TIM1->CR2 = 0x00;		//igual al reset

    //OJO si es PWM OCxPE debe ser 1
    TIM1->CCMR1 = 0x0068;			//CH1 output PWM mode 1 update on UE
    //TIM1->CCMR1 = 0x0028;			//CH1 output; inactive on output match; update on UE	NO FUNCIONA

    //CH1 y CH1N negado enable on pin CC1P CC1NP active high
    TIM1->CCER |= TIM_CCER_CC1E |  TIM_CCER_CC1NE;    // original board
    //TIM1->CCER |= TIM_CCER_CC1E |  TIM_CCER_CC1NE | TIM_CCER_CC1P;	//salen las 2 en fase
    //CH1 y CH1N negado enable on pin CC1P CC1NP active low
    // TIM1->CCER |= TIM_CCER_CC1E |  TIM_CCER_CC1NE | TIM_CCER_CC1P | TIM_CCER_CC1NP;	//contrafase activo al reves

    //hab general de OC y estado inhabilitado alto
    //TIM1->BDTR |= TIM_BDTR_MOE;
    // general enable for OC, low disable, and dead-time
    // TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSI | 182;    //5us @ 48MHz
    TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSI | (128 + 28);    //3.8us @ 48MHz    

    TIM1->ARR = TIM1_ARR;
    TIM1->CNT = 0;
    TIM1->PSC = 47;	//prescaler divido 48MHz / (1 + 47)
    //TIM3->PSC = 0;	//prescaler divido 48MHz / (1 + 0)
    //TIM3->EGR = TIM_EGR_UG;

    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM1->CR1 |= TIM_CR1_CEN;

    temp = GPIOA->AFR[0];
    temp &= 0x0FFFFFFF;
    temp |= 0x20000000;	//PA7 -> AF2
    GPIOA->AFR[0] = temp;

    temp = GPIOA->AFR[1];
    temp &= 0xFFFFFFF0;
    temp |= 0x00000002;	//PA8 -> AF2
    GPIOA->AFR[1] = temp;

    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 2);
}


void TIM_3_Init (void)
{
    if (!RCC_TIM3_CLK)
	RCC_TIM3_CLK_ON;

    //Configuracion del timer.
    TIM3->CR1 = 0x00;		//clk int / 1; upcounting
    TIM3->CR2 = 0x00;		//igual al reset
    TIM3->CCMR1 = 0x7070;			//CH2 y CH1 output PWM mode 2
    //TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;	//CH2 y CH1 enable on pin
    TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E |  TIM_CCER_CC1P;	//CH2 y CH1 negado enable on pin
    TIM3->ARR = TIM3_ARR;
    TIM3->CNT = 0;
    TIM3->PSC = 47;	//prescaler divido 48MHz / (1 + 47)
    //TIM3->PSC = 0;	//prescaler divido 48MHz / (1 + 0)
    //TIM3->EGR = TIM_EGR_UG;

    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
}


///////////////////
// TIM14 Configs //
///////////////////
inline void TIM14_IC_CH1_OFF (void)
{
    //desa int
    //TIM14->DIER |= TIM_DIER_UIE;
    TIM14->DIER &= 0xFFFD;
}


inline void TIM14_IC_CH1_ON (void)
{
    //ena int
    TIM14->DIER |= TIM_DIER_CC1IE;
}


void TIM14_IC_CNT (unsigned short new_counter)
{
    TIM14->CNT = new_counter;
}


void TIM14_IRQHandler (void)	//100uS
{
    //reload or overcaptured
    if (((TIM14->SR & 0x01) != 0) || ((TIM14->SR & 0x20) != 0))
	TIM14->SR = 0x00;	//bajar flag

    //IC OJO puede ser error
    if ((TIM14->SR & 0x02) != 0)
    {
	//ver pag 356
	// Timer_Interrupt_Handler (TIM14->CCR1);
	TIM14->SR = 0x00;
    }
}


void TIM_14_Init (void)
{
    unsigned long temp;

    if (!RCC_TIM14_CLK)
        RCC_TIM14_CLK_ON;

    //Configuracion del timer.
    //TIM14->ARR = 2000; //10m
    TIM14->CR1 = 0;
    TIM14->ARR = 0xFFFF;		//modo continuo
    TIM14->CNT = 0;
    TIM14->PSC = 47;	//1us por tick

    //N = 8 filter; sin prescaler; input capture
    //TIM14->CCMR1 = 0x0031;	//filtro OK
    //TIM14->CCMR1 = 0x0021;	//filtro OK
    //TIM14->CCMR1 = 0x0041;		//filtro OK
    TIM14->CCMR1 = 0x0071;		//filtro OK
    //TIM14->CCMR1 = 0x0001;		//sin filtro
    TIM14->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NP;	//input capture CH1 any edge

    //TIM14->EGR = TIM_EGR_UG;

    temp = GPIOA->AFR[0];
    temp &= 0xFFF0FFFF;
    temp |= 0x00040000;	//PA4 -> AF4
    GPIOA->AFR[0] = temp;

    //Interrupts
    // Enable timer ver UDIS
    //TIM14->DIER |= TIM_DIER_UIE;
    TIM14->DIER |= TIM_DIER_CC1IE;
    TIM14->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM14_IRQn);
    NVIC_SetPriority(TIM14_IRQn, 1);
}


///////////////////
// TIM16 Configs //
///////////////////
void TIM_16_Init (void)
{
    if (!RCC_TIM16_CLK)
        RCC_TIM16_CLK_ON;

    // Timer config
    TIM16->ARR = 0xFFFF;
    TIM16->CNT = 0;
    TIM16->PSC = 47;
    TIM16->EGR = TIM_EGR_UG;

    // Enable timer ver UDIS
    // TIM16->DIER |= TIM_DIER_UIE;    //con int por update event
    TIM16->CR1 |= TIM_CR1_CEN;
}


void TIM16_IRQHandler (void)
{
    if (TIM16->SR & 0x01)
	TIM16->SR = 0x00;    // low flag
}


///////////////////
// TIM17 Configs //
///////////////////
void TIM17_IRQHandler (void)
{
    if (TIM17->SR & 0x01)
    {
	TIM17->SR = 0x00;    // low int flag

	// Hard_Ac_Int_Handler();
	Det_Ac_Int_Handler();
    }
}


void TIM_17_Init (void)
{
    if (!RCC_TIM17_CLK)
	RCC_TIM17_CLK_ON;

    // Timer configs
    TIM17->ARR = 0;
    TIM17->CNT = 0;
    TIM17->PSC = 479;    // tick every 100us
    TIM17->EGR = TIM_EGR_UG;

    // ARR reload with buffer; one pulse mode
    TIM17->CR1 |= TIM_CR1_OPM | TIM_CR1_URS;
    // TIM17->CR1 |= TIM_CR1_URS;    

    // Enable timer ver UDIS
    TIM17->DIER |= TIM_DIER_UIE;
    // TIM17->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM17_IRQn);
    NVIC_SetPriority(TIM17_IRQn, 8);    
}


void TIM_17_Activate_With_Arr (unsigned short new_arr)
{
    TIM17->CNT = 0;
    TIM17->ARR = new_arr;
    TIM17->EGR = TIM_EGR_UG;
    TIM17->CR1 |= TIM_CR1_CEN;
}


void TIM_17_Deact (void)
{
    // TIM17->ARR = 0;
    // TIM17->CNT = 0;
    // TIM17->CR1 &= ~(TIM_CR1_CEN);
}
//--- end of file ---//
