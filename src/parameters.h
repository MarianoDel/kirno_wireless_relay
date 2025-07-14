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
    manager_codes_t relay1_code0;
    manager_codes_t relay1_code1;
    unsigned char relay1_actual_code;
    
    manager_codes_t relay2_code0;
    manager_codes_t relay2_code1;
    unsigned char relay2_actual_code;

    manager_codes_t relay3_code0;
    manager_codes_t relay3_code1;
    unsigned char relay3_actual_code;    

    manager_codes_t relay4_code0;
    manager_codes_t relay4_code1;
    unsigned char relay4_actual_code;    
    
} parameters_typedef;


#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

