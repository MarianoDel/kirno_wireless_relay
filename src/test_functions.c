//----------------------------------------------------------
// #### MAGNET PROJECT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
// extern volatile unsigned char timer_6_uif_flag;
// extern volatile unsigned char rx_int_handler;
// extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led (void);
void TF_Led_Sw_Learn_On (void);
    

// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    TF_Led_Sw_Learn_On ();
}


void TF_Led (void)
{
    while (1)
    {
	if (Led_Is_On())
	    Led_Off();
	else
	    Led_On();

	Wait_ms (1000);
    }
}


void TF_Led_Sw_Learn_On (void)
{
    while (1)
    {
	if (Sw_Learn_Is_On())
	    Led_On();
	else
	    Led_Off();

	Wait_ms (100);
    }
}


//--- end of file ---//
