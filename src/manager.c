//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANAGER.C #################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "manager.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "rf_rx_codes.h"
#include "programming.h"
#include "parameters.h"

#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    MANAGER_INIT,
    MANAGER_RUNNING,
    MANAGER_PROGRAMMING
    
} manager_sm_e;


typedef enum {
    MANAGER_MODE_KEY = 0,
    MANAGER_MODE_PULSED,
    MANAGER_MODE_SWITCH,
    MANAGER_MODE_TIMER
    
} manager_mode_e;


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
manager_sm_e manager_state = MANAGER_INIT;
unsigned char manager_mode = 0;

volatile unsigned short manager_relay_1 = 0;
volatile unsigned short manager_relay_2 = 0;
volatile unsigned short manager_relay_3 = 0;
volatile unsigned short manager_relay_4 = 0;


// Private Module Functions ----------------------------------------------------
void Manager_Change_Relay (unsigned char code_getted,
			   unsigned char mode,
			   rf_rx_codes_t * rx_code);

void Manager_Key_SM (unsigned char code_getted, rf_rx_codes_t * rx_code);
void Manager_Pulsed_SM (unsigned char code_getted, rf_rx_codes_t * rx_code);
void Manager_Switch_SM (unsigned char code_getted, rf_rx_codes_t * rx_code);
void Manager_Timer_SM (unsigned char code_getted, rf_rx_codes_t * rx_code);

void Manager_Key_SM_Reset (void);
void Manager_Pulsed_SM_Reset (void);
void Manager_Switch_SM_Reset (void);
void Manager_Timer_SM_Reset (void);


// Module Functions ------------------------------------------------------------
void Manager_Timeouts (void)
{
    if (manager_relay_1)
	manager_relay_1--;

    if (manager_relay_2)
	manager_relay_2--;

    if (manager_relay_3)
	manager_relay_3--;

    if (manager_relay_4)
	manager_relay_4--;
    
    
}


void Manager (void)
{
    rf_rx_codes_t new_code;
    resp_t resp = resp_continue;
    unsigned char code_getted = 0;

    while (1)
    {
	switch (manager_state)
	{
	case MANAGER_INIT:
	    Hard_Led_Change_Bips ((manager_mode + 1), 200, 4000);

	    Manager_Key_SM_Reset ();
	    Manager_Pulsed_SM_Reset ();
	    Manager_Switch_SM_Reset ();
	    Manager_Timer_SM_Reset ();
	    code_getted = 0;
	    
	    manager_state = MANAGER_RUNNING;
	    break;

	case MANAGER_RUNNING:
	    Hard_Led_Blinking_Update ();

	    resp = Rf_Get_Codes (&new_code);
	    if (resp == resp_ok)
		code_getted = 1;

	    Manager_Change_Relay (code_getted, manager_mode, &new_code);
	    code_getted = 0;
	    
	    if (Check_Sw_Learn() > SW_NO)
	    {
		Programming_Reset ();
		manager_state = MANAGER_PROGRAMMING;
	    }
	    break;
	    
	case MANAGER_PROGRAMMING:
	    resp = Programming ();
	    if (resp == resp_ok)
	    {
		manager_state = MANAGER_INIT;
	    }
	    break;

	default:
	    manager_state = MANAGER_INIT;
	    break;
	}
    }
}


void Manager_Change_Relay (unsigned char code_getted,
			   unsigned char mode,
			   rf_rx_codes_t * rx_code)
{
    switch (mode)
    {
    case MANAGER_MODE_KEY:
	Manager_Key_SM (code_getted, rx_code);
	break;

    case MANAGER_MODE_PULSED:
	Manager_Pulsed_SM (code_getted, rx_code);	
	break;

    case MANAGER_MODE_SWITCH:
	Manager_Switch_SM (code_getted, rx_code);	
	break;

    case MANAGER_MODE_TIMER:
	Manager_Timer_SM (code_getted, rx_code);	
	break;

    }
}


void Manager_Key_SM (unsigned char code_getted, rf_rx_codes_t * check_code)
{
    if (!code_getted)
	return;

    if (check_code->bits != 24)
	return;

    // for (int i = 0; i < check_code->bits; i++)
    // {
    // 	Led_On();
    // 	Wait_ms(250);
    // 	Led_Off();
    // 	Wait_ms(250);
    // }

    // Wait_ms (10000);
    

    // relay 1
    if (!manager_relay_1)
    {
	// check relay1 code
	if ((mem_conf.relay1_code0.code == check_code->code) ||
	    (mem_conf.relay1_code1.code == check_code->code))
	{
	    if (Relay_Ch1_Is_On())
	    {
		Relay_Ch1_Off();
		manager_relay_1 = 4000;
	    }
	    else
	    {
		Relay_Ch1_On();
		manager_relay_1 = 4000;
	    }
	}
    }

    // relay 2
    if (!manager_relay_2)
    {
	// check relay2 code
	if ((mem_conf.relay2_code0.code == check_code->code) ||
	    (mem_conf.relay2_code1.code == check_code->code))
	{
	    if (Relay_Ch2_Is_On())
	    {
		Relay_Ch2_Off();
		manager_relay_2 = 4000;
	    }
	    else
	    {
		Relay_Ch2_On();
		manager_relay_2 = 4000;
	    }
	}
    }

    // relay 3
    if (!manager_relay_3)
    {
	// check relay3 code
	if ((mem_conf.relay3_code0.code == check_code->code) ||
	    (mem_conf.relay3_code1.code == check_code->code))
	{
	    if (Relay_Ch3_Is_On())
	    {
		Relay_Ch3_Off();
		manager_relay_3 = 4000;
	    }
	    else
	    {
		Relay_Ch3_On();
		manager_relay_3 = 4000;
	    }
	}
    }

    // relay 4
    if (!manager_relay_4)
    {
	// check relay4 code
	if ((mem_conf.relay4_code0.code == check_code->code) ||
	    (mem_conf.relay4_code1.code == check_code->code))
	{
	    if (Relay_Ch4_Is_On())
	    {
		Relay_Ch4_Off();
		manager_relay_4 = 4000;
	    }
	    else
	    {
		Relay_Ch4_On();
		manager_relay_4 = 4000;
	    }
	}
    }
}


void Manager_Pulsed_SM (unsigned char code_getted, rf_rx_codes_t * rx_code)
{
    
}


void Manager_Switch_SM (unsigned char code_getted, rf_rx_codes_t * rx_code)
{
    
}


void Manager_Timer_SM (unsigned char code_getted, rf_rx_codes_t * rx_code)
{
    
}


void Manager_Key_SM_Reset (void)
{
}

void Manager_Pulsed_SM_Reset (void)
{
}

void Manager_Switch_SM_Reset (void)
{
}

void Manager_Timer_SM_Reset (void)
{
}


//--- end of file ---//
