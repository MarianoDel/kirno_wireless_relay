//----------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #################################
//----------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"


// Module Private Types & Macros -----------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char Led_Is_On (void)
{
    return LED;
}


void Led_On (void)
{
    LED_ON;
}


void Led_Off (void)
{
    LED_OFF;
}


unsigned char Sw_Learn_Is_On (void)
{
    return SW_LEARN;
}


// void OnOff_On (void)
// {
//     ON_OFF_ON;
// }


// void OnOff_Off (void)
// {
//     ON_OFF_OFF;
// }

// #endif

// #ifdef HARDWARE_VERSION_1_0
// void Ena_Ch1_On (void)
// {
//     ENA_CH1_ON;
// }


// void Ena_Ch2_On (void)
// {
//     ENA_CH2_ON;
// }


// void Ena_Ch3_On (void)
// {
//     ENA_CH3_ON;
// }
    
// void Ena_Ch4_On (void)
// {
//     ENA_CH4_ON;
// }


// void Ena_Ch1_Off (void)
// {
//     ENA_CH1_OFF;
// }


// void Ena_Ch2_Off (void)
// {
//     ENA_CH2_OFF;
// }


// void Ena_Ch3_Off (void)
// {
//     ENA_CH3_OFF;
// }


// void Ena_Ch4_Off (void)
// {
//     ENA_CH4_OFF;
// }


//--- end of file ---//
