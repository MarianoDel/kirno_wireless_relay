//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "manager.h"


typedef struct parameters {
    manager_codes_t relay1_code0;    // 8 - 8
    manager_codes_t relay1_code1;    // 8 - 16
    
    manager_codes_t relay2_code0;    // 8 - 24
    manager_codes_t relay2_code1;    // 8 - 32

    manager_codes_t relay3_code0;    // 8 - 40
    manager_codes_t relay3_code1;    // 8 - 48

    manager_codes_t relay4_code0;    // 8 - 56
    manager_codes_t relay4_code1;    // 8 - 64

    unsigned int secs_relays;    // 4 - 68
    
    unsigned char relay1_actual_code;    // 1 - 69
    unsigned char relay2_actual_code;    // 1 - 70
    unsigned char relay3_actual_code;    // 1 - 71
    unsigned char relay4_actual_code;    // 1 - 72

    unsigned char manager_mode;    // 1 - 73
    unsigned char dummy [7];    // 7 - 80
    
} parameters_typedef;


#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

