//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANAGER.H #################################
//------------------------------------------------------
#ifndef _MANAGER_H_
#define _MANAGER_H_

// Additional Includes for Configuration ---------------------------------------


// Defines for Configuration ---------------------------------------------------


// Exported Macros and Constants -----------------------------------------------
typedef struct {
    unsigned char bits;
    unsigned char dummy;    
    unsigned short lambda;
    unsigned int code;    
    
} manager_codes_t;

typedef enum {
    PK_INIT = 0,
    PK_STAND_BY,
    PK_OUTPUT_TO_DELAY_ACTIVATE,
    PK_OUTPUT_TO_ACTIVATE,    
    PK_OUTPUT_ACTIVE

} manager_sm_t;




// Exported Module Functions ---------------------------------------------------
void Manager_Timeouts (void);
void Manager (void);


#endif    /* _MANAGER_H_ */

//--- end of file ---//
