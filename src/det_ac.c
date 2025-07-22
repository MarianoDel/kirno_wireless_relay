//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DET_AC.C #################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "det_ac.h"
#include "tim.h"
#include "hard.h"


// Define for configs ----------------------------------------------------------



// Module Private Types Constants and Macros -----------------------------------



// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Private Module Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Det_Ac_Init (void)
{
    // Enable TIM deact
    TIM_17_Init();
}


void Det_Ac_Act (unsigned short new_arr)
{
    TIM_17_Activate_With_Arr (new_arr);
}


// not used tim17 is on one pulse mode
void Det_Ac_Deact (void)
{
    TIM_17_Deact();
}


#define DET_AC_EDGE_DETECT    0
#define DET_AC_FIRST_TIMER_DISCONNECT    1
#define DET_AC_SECOND_TIMER_CONNECT    2
volatile unsigned char det_ac_handler_sm = DET_AC_EDGE_DETECT;
void Det_Ac_Edge_Detect (void)
{
    if (det_ac_handler_sm == DET_AC_EDGE_DETECT)
    {
	// Led_On();
	Det_Ac_Act (330);    // first timer on +3.3ms for disconnect
	det_ac_handler_sm = DET_AC_FIRST_TIMER_DISCONNECT;
    }
}


void Det_Ac_Int_Handler (void)
{
    if (det_ac_handler_sm == DET_AC_FIRST_TIMER_DISCONNECT)
    {
	// Led_On();
	Hard_Ac_Int_Handler_Disconnect();
	Det_Ac_Act (880);    // connect on +8.8ms	
	det_ac_handler_sm = DET_AC_SECOND_TIMER_CONNECT;
    }
    else if (det_ac_handler_sm == DET_AC_SECOND_TIMER_CONNECT)
    {
	// Led_Off();
	Hard_Ac_Int_Handler_Connect();
	det_ac_handler_sm = DET_AC_EDGE_DETECT;
    }
}

//--- end of file ---//
