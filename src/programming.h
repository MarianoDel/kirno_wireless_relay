//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PROGRAMMING.H #################################
//------------------------------------------------------
#ifndef _PROGRAMMING_H_
#define _PROGRAMMING_H_

// Additional Includes for Configuration ---------------------------------------
#include "switches_answers.h"


// Defines for Configuration ---------------------------------------------------


// Exported Macros and Constants -----------------------------------------------
typedef struct {
    unsigned char bits;
    unsigned int code;
    unsigned short lambda;
    
} programming_codes_t;


#define PROG_UTILS_FULL_FLAG    0x80
#define PROG_UTILS_CHANGE_FLAG    0x40
#define PROG_UTILS_END_FLAG    0x20
// Exported Module Functions ---------------------------------------------------
void Programing_Utils_Reset (unsigned char how_many_options);
unsigned char Programming_Utils (void);
void Programming_Timeouts (void);
resp_t Programming (void);
void Programming_Reset (void);


#endif    /* _PROGRAMMING_H_ */

//--- end of file ---//
