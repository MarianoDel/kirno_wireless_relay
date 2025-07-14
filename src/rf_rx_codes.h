//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### RF_RX_CODES.H #################################
//------------------------------------------------------
#ifndef _RF_RX_CODES_H_
#define _RF_RX_CODES_H_


#include "switches_answers.h"

// Exported Types Constants and Macros -----------------------------------------
typedef struct {
    unsigned char bits;
    unsigned int code;
    unsigned short lambda;
    
} rf_rx_codes_t;



// Exported Module Functions ---------------------------------------------------
resp_t Rf_Get_Codes (rf_rx_codes_t * new_code_st);


#endif    /* _RF_RX_CODES_H_ */

//--- end of file ---//
