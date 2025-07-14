//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SWITCHES_ANSWERS.H ####################
//---------------------------------------------

#ifndef _SWITCHES_ANSWERS_H_
#define _SWITCHES_ANSWERS_H_

// Exported Types Constants and Macros -----------------------------------------

// Switches actions
typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;


typedef enum {
    resp_continue = 0,
    resp_ok,
    resp_error
    
} resp_t;


#endif /* _SWITCHES_ANSWERS_H_ */

//--- end of file ---//
