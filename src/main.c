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

#include "test_functions.h"
#include "flash_program.h"
#include "tim.h"
#include "parameters.h"

#include <stdio.h>
#include <string.h>

#include "manager.h"
#include "programming.h"
#include "det_ac.h"


// Externals -------------------------------------------------------------------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby = 0;
parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash


// Module Functions ------------------------------------------------------------
void SysTickError (void);
void TimingDelay_Decrement(void);
void Factory_Defaults (void);


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
    // para rx codes?
    TIM_16_Init();

    // check memory params or get default
    if (pmem->manager_mode != 0xff)
	memcpy (&mem_conf, pmem, sizeof(mem_conf));
    else
	Factory_Defaults();

    Det_Ac_Init();

    while (1)
    {

	Manager();

    }

    return 0;
}

//--- End of Main ---//

void Factory_Defaults (void)
{
    mem_conf.secs_relays = 60;

    mem_conf.relay1_actual_code = 0;
    mem_conf.relay2_actual_code = 0;
    mem_conf.relay3_actual_code = 0;
    mem_conf.relay4_actual_code = 0;

    mem_conf.manager_mode = 0;
}


// One_ms Interrupt
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;
    
    Hard_Timeouts ();

    Programming_Timeouts ();

    Manager_Timeouts ();
    
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
