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
#include "det_ac.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
// extern volatile unsigned char timer_6_uif_flag;
// extern volatile unsigned char rx_int_handler;
// extern volatile unsigned char usart3_have_data;

extern void Manager_Relay_Ch1_On (void);
extern void Manager_Relay_Ch1_Off (void);


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led (void);
void TF_Led_Sw_Learn_On (void);
void TF_Relay_In_Sync (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    // TF_Led_Sw_Learn_On ();
    TF_Relay_In_Sync ();
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


void TF_Relay_In_Sync (void)
{
    unsigned char rel_state = 0;

    Det_Ac_Init();

    timer_standby = 2000;

    while (1)
    {
	Hard_Det_AC_Update ();

	if (!timer_standby)
	{
	    if (rel_state == 0)
	    {
		timer_standby = 8000;
		rel_state = 1;
		Manager_Relay_Ch1_On();
	    }
	    else if (rel_state == 1)
	    {
		timer_standby = 8000;
		rel_state = 0;
		Manager_Relay_Ch1_Off();
	    }
	}	
    }
}

//--- end of file ---//
