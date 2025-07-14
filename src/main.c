//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### MAIN.C #########################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f0xx.h"
#include "hard.h"
#include "gpio.h"

#include "core_cm0.h"
#include "test_functions.h"
#include "rws317.h"
#include "flash_program.h"
#include "tim.h"
#include "parameters.h"

#include <stdio.h>
#include <string.h>
#include "it.h"


#include "rf_rx_codes.h"
#include "manager.h"
#include "programming.h"


// Externals -------------------------------------------------------------------
volatile unsigned short wait_ms_var = 0;
parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------


// Module Functions ------------------------------------------------------------
void SysTickError (void);
void TimingDelay_Decrement(void);



//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    // Systick Timer Activation
    if (SysTick_Config(48000))
        SysTickError();

    // Hardware Tests
    // TF_Hardware_Tests ();

    // --- main program inits. ---
    // RxCode();    // stop ints
    TIM_14_Init();

    // para rx codes?
    TIM_16_Init();

    // check memory params or get default
    
    while (1)
    {
	Manager();
    }

    return 0;
}
//--- End of Main ---//


// One_ms Interrupt
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    Hard_Timeouts ();

    Programming_Timeouts ();

    Manager_Timeouts ();
    
    // if (wait_for_code_timeout)
    //     wait_for_code_timeout--;

    // //cuenta 1 segundo
    // if (button_timer_internal)
    //     button_timer_internal--;
    // else
    // {
    //     if (button_timer_secs)
    //     {
    //         button_timer_secs--;
    //         button_timer_internal = 1000;
    //     }
    // }
}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}


//--- end of file ---//
