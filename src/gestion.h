//----------------------------------------------------------
// #### VAPORE ALARM WITH KEYPAD PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GESTION.H ##########################################
//----------------------------------------------------------
#ifndef _GESTION_H_
#define _GESTION_H_

// Additional Includes for Configuration ---------------------------------------


// Defines for Configuration ---------------------------------------------------



// Module Exported Types Constants and Macros ----------------------------------
//estados de la SM de GESTION
typedef enum {
    GESTION_SM_INIT = 0,
    GESTION_SM_MAIN,

    GESTION_SM_TO_MONITORING,
    GESTION_SM_IN_MONITORING,

    GESTION_SM_TO_FLUSH_SST,
    
    GESTION_SM_TO_WRITE_SST_CONF,
    GESTION_SM_TO_WRITE_SST_CONFA,
    GESTION_SM_TO_WRITE_SST_CONFB,
    GESTION_SM_TO_WRITE_SST_CONFC,

    GESTION_SM_GET_READY_FOR_CHUNK,
    GESTION_SM_WAIT_BINARY_CHUNK,
    GESTION_SM_WAIT_NEXT_CHUNK,
    
    GESTION_SM_TO_WRITE_SST0,
    GESTION_SM_TO_WRITE_SST1,
    GESTION_SM_TO_WRITE_SST2,
    GESTION_SM_TO_WRITE_SST3,		
    GESTION_SM_TO_WRITE_SST4,		
    GESTION_SM_TO_WRITE_SST5,		
    GESTION_SM_TO_WRITE_SST6,		
    GESTION_SM_TO_WRITE_SST7,		
    GESTION_SM_TO_WRITE_SST8,
    GESTION_SM_TO_WRITE_SST9,
    GESTION_SM_TO_WRITE_SSTB1,
    GESTION_SM_TO_WRITE_SSTB3,
    
    GESTION_SM_TO_MONITORING_LEAVE,
    GESTION_SM_WAIT_TO_LEAVE
    
} gestion_sm_t;




// Module Exported Functions ---------------------------------------------------
void FuncGestion (void);


#endif    /* _GESTION_H_ */

//--- end of file ---//
