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


void Det_Ac_Deact (void)
{
    TIM_17_Deact();
}


void Det_Ac_Int_Handler (void)
{
    if (Led_Is_On())
	Led_Off();
    // else
	// Led_On();
}

//--- end of file ---//
