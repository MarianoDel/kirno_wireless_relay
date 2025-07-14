//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PROGRAMING.C #################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "programming.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "codes.h"
#include "rf_rx_codes.h"
#include "parameters.h"


#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    PROG_INIT = 0,
    PROG_BUTTONS,
    PROG_GO_TO_MODES,
    PROG_MODES,
    PROG_DONE

} programming_states_e;


typedef enum {
    UTILS_INIT_WAIT_FREE,
    UTILS_SAVE_KEY,
    UTILS_WAIT_FREE

} programming_utils_e;


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
programming_states_e prog_state = PROG_INIT;
volatile unsigned short prog_timer = 0;


// Private Module Functions ----------------------------------------------------
// resp_t Programing_Codes (programing_codes_t *);


// Module Functions ------------------------------------------------------------
void Programming_Timeouts (void)
{
    if (prog_timer)
	prog_timer--;
    
}


void Programming_Reset (void)
{
    prog_state = PROG_INIT;
}


resp_t Programming (void)
{
    resp_t answer = resp_continue;
    unsigned char resp = 0;
    unsigned char relay_on_programming = 0;
    rf_rx_codes_t my_codes;
    
    switch (prog_state)
    {
    case PROG_INIT:
	Programing_Utils_Reset (4);
	prog_timer = 20000;
	prog_state = PROG_BUTTONS;
	break;
	    
    case PROG_BUTTONS:
	// Hard_Led_Blinking_Update ();
	relay_on_programming = Programming_Utils ();

	if (relay_on_programming != 0)    // verificar esto
	{
	    // if we have the button check the codes
	    // rx codes
	    answer = Rf_Get_Codes (&my_codes);
            
	    if (answer == resp_ok)
	    {
		// evaluate the code
		if (my_codes.bits == 24)
		{
		    if (mem_conf.relay1_actual_code == 0)
		    {
			mem_conf.relay1_code0.code = my_codes.code;
			mem_conf.relay1_code0.bits = my_codes.bits;
			mem_conf.relay1_code0.lambda = my_codes.lambda;
			//next code in pos1
			mem_conf.relay1_actual_code = 1;
		    }
		    else
		    {
			mem_conf.relay1_code1.code = my_codes.code;
			mem_conf.relay1_code1.bits = my_codes.bits;
			mem_conf.relay1_code1.lambda = my_codes.lambda;
			//next code in pos0
			mem_conf.relay1_actual_code = 0;			
		    }
		}

		prog_state = PROG_DONE;
		prog_timer = 3000;
		Hard_Led_Change_Bips (1, 100, 1);
		answer = resp_continue;
	    }
	}
	    
	if (resp & PROG_UTILS_FULL_FLAG)
	{
	    prog_state = PROG_GO_TO_MODES;
	    prog_timer = 3000;
	    Led_On();
	}

	if (!prog_timer)
	{
	    prog_state = PROG_DONE;
	    prog_timer = 3000;
	    Hard_Led_Change_Bips (1, 300, 1);		
	}
	break;
	    
    case PROG_GO_TO_MODES:
	if ((!prog_timer) &&
	    (Check_Sw_Learn () == SW_NO))
	{
	    Programing_Utils_Reset (5);
	    Led_Off();
	    prog_state = PROG_MODES;
	    prog_timer = 20000;		
	}
	break;

    case PROG_MODES:
	Hard_Led_Blinking_Update ();
	resp = Programming_Utils ();

	if (resp & PROG_UTILS_END_FLAG)
	    prog_timer = 20000;

	if (resp & PROG_UTILS_FULL_FLAG)
	{
	    // back to main prog
	}

	if (!prog_timer)
	{
	    prog_state = PROG_DONE;
	    prog_timer = 3000;
	    Hard_Led_Change_Bips (1, 100, 1);
	}
	break;

    case PROG_DONE:
	Hard_Led_Blinking_Update ();

	if (!prog_timer)
	{
	    prog_state = PROG_INIT;
	    answer = resp_ok;
	}
	break;
	    
    default:
	prog_state = PROG_INIT;
	break;

    }

    return answer;
}




unsigned char prog_utils_options = 0;
unsigned char prog_utils_actual = 0;
programming_utils_e prog_utils_state = UTILS_WAIT_FREE;
void Programing_Utils_Reset (unsigned char how_many_options)
{
    prog_utils_state = UTILS_INIT_WAIT_FREE;
    prog_utils_options = how_many_options;
    prog_utils_actual = 1;
    Hard_Led_Change_Bips (prog_utils_actual, 200, 2000);
}


unsigned char Programming_Utils (void)
{
    unsigned char resp = prog_utils_actual;
    
    switch (prog_utils_state)
    {
    case UTILS_INIT_WAIT_FREE:
	if (Check_Sw_Learn() == SW_NO)
	    prog_utils_state = UTILS_SAVE_KEY;

	break;
	    
    case UTILS_SAVE_KEY:
	Hard_Led_Blinking_Update ();

	if (Check_Sw_Learn() > SW_NO)
	{
	    prog_utils_state = UTILS_WAIT_FREE;
	    if (prog_utils_actual < prog_utils_options)
		prog_utils_actual++;
	    else
		prog_utils_actual = 1;

	    Hard_Led_Change_Bips (prog_utils_actual, 200, 2000);
	}
	break;

    case UTILS_WAIT_FREE:
	if (Check_Sw_Learn() == SW_NO)
	{
	    prog_utils_state = UTILS_SAVE_KEY;
	    resp |= PROG_UTILS_CHANGE_FLAG;
	}
	break;

    default:
	prog_utils_state = UTILS_INIT_WAIT_FREE;
	break;
    }

    return resp;
}


//--- end of file ---//
