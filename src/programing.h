//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PROGRAMING.H #################################
//------------------------------------------------------
#ifndef _PROGRAMING_H_
#define _PROGRAMING_H_

// Additional Includes for Configuration ---------------------------------------


// Defines for Configuration ---------------------------------------------------


// Exported Macros and Constants -----------------------------------------------
typedef struct {
    unsigned char bits;
    unsigned int code;
    unsigned short lambda;
    
} programing_codes_t;


typedef enum {
    PROG_INIT = 0,
    PROG_SAVE_B1,
    PROG_SAVE_B1_WAIT_FREE,
    PROG_SAVE_B2,
    PROG_SAVE_B2_WAIT_FREE,
    PROG_SAVE_B3,
    PROG_SAVE_B3_WAIT_FREE,
    PROG_SAVE_B4,
    PROG_SAVE_B4_WAIT_FREE,
    PROG_CHANGE_MODE

} programing_sm_t;




// Exported Module Functions ---------------------------------------------------
void Programing (void);


#endif    /* _PROGRAMING_H_ */

//--- end of file ---//
