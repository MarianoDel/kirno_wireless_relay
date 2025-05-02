//------------------------------------------------------
// #### PROYECTO PANEL ALARMA VAPORE - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### RWS317.H #######################################
//------------------------------------------------------
#ifndef _RWS317_H_
#define _RWS317_H_

// Additional Includes for Configuration ------------------------------
#include "flash_program.h"    //info in sst or info in flash
#include "hard.h"             //info del tipo de programa
#include "rws317_hard_conf.h"    //hardware conf mostly for tims


//------ FUNCIONAMIENTO GENERAL DEL EQUIPO ------//
// VER HARD.H LO QUE CORRESPONDE A HARDWARE
// VER RWS317.H LO QUE TIENE QUE VER CON CODIGOS Y CONTROLES


//--- TEMAS DE LOS CODIGOS ---//
//---- Chip Interno de los Controles
//#define HT6P20B2
//#define EV1527
//#define PT2264
// #define HT6P20B2_AND_PT2264
#define EV1527_AND_PT2264

#define B1_BY_B2		//botones cambiados B1 x B2


// Exported Macros and Defines ----------------------------------------
#if (defined PROGRAMA_NORMAL) || (defined PRODUCCION_NORMAL)
#define ACT_DESACT_IN_SECS	2
#define ACT_DESACT_IN_MSECS	2000
#endif
#if (defined PROGRAMA_DE_BUCLE) || (defined PRODUCCION_BUCLE)
#define ACT_DESACT_IN_SECS	6
#define ACT_DESACT_IN_MSECS	6000
#endif
#if (defined PROGRAMA_CHICKEN_BUCLE) || (defined PRODUCCION_CHICKEN_BUCLE)
#define ACT_DESACT_IN_SECS	6
#define ACT_DESACT_IN_MSECS	6000
#endif
#if (defined PROGRAMA_FACTORY_TEST) || (defined PROGRAMA_FACTORY_TEST_ONLY_RF)	//evita problemas de compilacion
#define ACT_DESACT_IN_SECS	2
#define ACT_DESACT_IN_MSECS	2000
#endif
#ifdef PROGRAMA_DE_GESTION	//evita problemas de compilacion en el main
#define ACT_DESACT_IN_SECS	2
#define ACT_DESACT_IN_MSECS	2000
#endif
#ifdef PROGRAMA_PORTON_KIRNO	//evita problemas de compilacion en el main
#define ACT_DESACT_IN_SECS	2
#define ACT_DESACT_IN_MSECS	2000
#endif

#ifdef EV1527
#define WITHOUT_ABSOLUTE_VALUE_CTROL_EV1527
#define EV1527_BUTTONS
#define EV1527_BASE_CODE
#define EV1527_CODE_MASK		0xFFFFFFE0
#define EV1527_BUTTON_MASK		0x0000001F
#endif

#ifdef HT6P20B2
#define WITHOUT_ABSOLUTE_VALUE_CTROL_NEW
#define HT6P20B2_BUTTONS
#define HT6P20B2_BASE_CODE
#define HT6P20B2_FOURTH_BUTTON
#define HT6P20B2_CODE_MASK			0xFFFFFF0F	//desplaza 4 bits
#define HT6P20B2_BUTTON_MASK		0x0000000F
#endif

#ifdef PT2264
#define WITHOUT_ABSOLUTE_VALUE_CTROL_PT2264
#endif

#ifdef HT6P20B2_AND_PT2264		//HT6 en controles PT2264 en gestion remota
#define WITHOUT_ABSOLUTE_VALUE_CTROL_HT6P20B2_AND_PT2264
#define HT6P20B2_BUTTONS
#define HT6P20B2_BASE_CODE
#define HT6P20B2_FOURTH_BUTTON
#define HT6P20B2_CODE_MASK			0xFFFFFF0F	//desplaza 4 bits
#define HT6P20B2_BUTTON_MASK		0x0000000F
#endif

#ifdef EV1527_AND_PT2264		//HT6 en controles PT2264 en gestion remota
#define WITHOUT_ABSOLUTE_VALUE_CTROL_EV1527_AND_PT2264
#define EV1527_BUTTONS
#define EV1527_BASE_CODE
#define EV1527_CODE_MASK		0xFFFFFFE0
#define EV1527_BUTTON_MASK		0x0000001F
#endif

//#define WITHOUT_CODES

//#define RXCODE_WITH_TIMEOUT
#define RXCODE_WITHOUT_TIMEOUT		//para pruebas, siempre espera al c�digo

//control chino 11.2ms silencio
//control nac 11.2ms silencia
//control chino nuevo 8ms
#define PILOT_CODE_MS 6
#define TIMEOUT_RXCODE 50	//(22 + 2 + 4) * 1.32ms + espera

#ifdef HT6P20B2
//--- SON +/- 20%
#define PILOT_MIN_NEW		280		//valor en us
#define PILOT_MAX_NEW		460		//
#define LAMBDA1_FROM1_MIN_NEW		PILOT_MIN_NEW
#define LAMBDA1_FROM1_MAX_NEW		PILOT_MAX_NEW
#define LAMBDA2_FROM1_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW * 2)
#define LAMBDA2_FROM1_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW * 2)
#define LAMBDA1_FROM0_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW)
#define LAMBDA1_FROM0_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW)
#define LAMBDA2_FROM0_MIN_NEW		(LAMBDA1_FROM0_MIN_NEW * 2)
#define LAMBDA2_FROM0_MAX_NEW		(LAMBDA1_FROM0_MAX_NEW * 2)
#endif

#ifdef EV1527
//--- SON +/- 10%
#define PILOT_MIN0_NEW		324		//324us				360us medido en el chip
#define PILOT_MAX0_NEW		396		//396us
#define PILOT_MIN1_NEW		(PILOT_MIN0_NEW * 3)		//972us
#define PILOT_MAX1_NEW		(PILOT_MAX0_NEW	* 3)		//1188us
#define LAMBDA1_FROM1_MIN_NEW		PILOT_MIN0_NEW
#define LAMBDA1_FROM1_MAX_NEW		PILOT_MAX0_NEW
#define LAMBDA2_FROM1_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW * 3)
#define LAMBDA2_FROM1_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW * 3)
#define LAMBDA1_FROM0_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW)
#define LAMBDA1_FROM0_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW)
#define LAMBDA2_FROM0_MIN_NEW		(LAMBDA1_FROM0_MIN_NEW * 3)
#define LAMBDA2_FROM0_MAX_NEW		(LAMBDA1_FROM0_MAX_NEW * 3)
#endif

#ifdef PT2264
//--- SON +/- 10%
#define PILOT_MIN0_NEW		330		//us				440us y 480 medido en el chip
#define PILOT_MAX0_NEW		660		//us
#define PILOT_MIN1_NEW		(PILOT_MIN0_NEW * 3)		//us
#define PILOT_MAX1_NEW		(PILOT_MAX0_NEW	* 3)		//us
#define LAMBDA1_FROM1_MIN_NEW		PILOT_MIN0_NEW
#define LAMBDA1_FROM1_MAX_NEW		PILOT_MAX0_NEW
#define LAMBDA2_FROM1_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW * 3)
#define LAMBDA2_FROM1_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW * 3)
#define LAMBDA1_FROM0_MIN_NEW		(LAMBDA1_FROM1_MIN_NEW)
#define LAMBDA1_FROM0_MAX_NEW		(LAMBDA1_FROM1_MAX_NEW)
#define LAMBDA2_FROM0_MIN_NEW		(LAMBDA1_FROM0_MIN_NEW * 3)
#define LAMBDA2_FROM0_MAX_NEW		(LAMBDA1_FROM0_MAX_NEW * 3)
#endif

#ifdef EV1527_AND_PT2264
//aca elijo el mayor o menor de los 2
#define ALPHA1_MIN		270		//us
#define ALPHA1_MAX		660		//us
#define ALPHA3_MIN		(ALPHA1_MIN * 3)
#define ALPHA3_MAX		(ALPHA1_MAX * 3)
#endif

#ifdef HT6P20B2_AND_PT2264
//primeros los de HT6
#define PILOT_HT_MIN		280		//us
#define PILOT_HT_MAX		460		//us
#define LAMBDA1_MIN			PILOT_HT_MIN
#define LAMBDA1_MAX			PILOT_HT_MAX
#define LAMBDA2_MIN			(LAMBDA1_MIN * 2)
#define LAMBDA2_MAX			(LAMBDA1_MAX * 2)
//ahora los de PT
#define ALPHA1_MIN		330		//us
#define ALPHA1_MAX		660		//us
#define ALPHA3_MIN		(ALPHA1_MIN * 3)
#define ALPHA3_MAX		(ALPHA1_MAX * 3)
#endif

//--- CODIGOS DE ERRORES DE BITS ---//
#define ERR_CONTINUE	0
#define ERR_FIRST_HIGH	1
#define ERR_BIT			2
#define ERR_BIT_NEXT			3
#define ERR_BIT_0		4
#define ERR_BIT_1		5
#define ERR_FINISH_OK	6

//--- ESTADOS DE RECEPCION DE BITS ---//
#ifdef HT6P20B2_AND_PT2264
#define FIRST_HIGH		0
#define FIRST_LOW		1
#define NEXT_ALL		2
#define NEXT_HT			3
#define CHECK0_HT		4
#define CHECK1_HT		5
#define NEXT_PT			6
#define CHECK0_PT		7
#define CHECK1_PT		8
#define WAIT_FOR_SILENT		10
#define CODE_FINISH		11
#endif

#ifdef EV1527_AND_PT2264
#define FIRST_HIGH	0
#define FIRST_LOW	1
#define NEXT		2
#define CHECK0		3
#define CHECK1		4
#define WAIT_FOR_SILENT		10
#define CODE_FINISH		11
#endif

#ifdef EV1527
#define FIRST_HIGH	0
#define FIRST_LOW	1
#define NEXT		2
#define CHECK0		3
#define CHECK1		4
#define WAIT_FOR_SILENT		10
#define CODE_FINISH		11
#endif

#ifdef HT6P20B2
#define FIRST_HIGH	0
#define FIRST_LOW	1
#define NEXT		2
#define CHECK0		3
#define CHECK1		4
#define WAIT_FOR_SILENT		10
#define CODE_FINISH		11
#endif

//--- TIPO DE CONTROL ---//
#ifdef HT6P20B2_AND_PT2264
#define CTRL_HT			0
#define CTRL_PT			1
#endif

//--- ESTADOS DE RECEPCION DE CODIGO ---//
#define START		0
#define SEARCHING	1
#define RECEIVING	2
#define ENDED		3
#define ENDED_OK	4
#define NO_CODE		5


//--- BOTONES DE LOS CODIGOS REMOTOS 12 CANALES ---//
#define REM_NO		0
#define REM_B1		1
#define REM_B2		2
#define REM_B3		3
#define REM_B4		4
#define REM_B5		5
#define REM_B6		6
#define REM_B7		7
#define REM_B8		8
#define REM_B9		9
#define REM_B10		10
#define REM_B11		11
#define REM_B12		12


//-------- Functions Wrapper --------//
#ifdef CODES_IN_FLASH
#define CheckCodeInMemory(X)		FLASH_CheckCodeInMemory((X))
#define CheckBaseCodeInMemory(X)	FLASH_CheckBaseCodeInMemory((X))
#define CheckIndexInMemory(X)		FLASH_CheckIndexInMemory((X))
#define Write_Code_To_Memory(X,Y)	FLASH_Write_Code_To_Memory((X),(Y))
#define EraseAllMemory()		FLASH_EraseAllMemory()
#define CheckButtonInCode(X)            FLASH_CheckButtonInCode((X))
#endif

#ifdef CODES_IN_SST
#define CheckCodeInMemory(X)		SST_CheckCodeInMemory((X))
#define CheckBaseCodeInMemory(X)	SST_CheckBaseCodeInMemory((X))
#define CheckIndexInMemory(X)		SST_CheckIndexInMemory((X))
#define Write_Code_To_Memory(X,Y)	SST_WriteCodeToMemory((X),(Y))
#define EraseAllMemory()		SST_EraseAllMemory()
#define CheckButtonInCode(X)            SST_CheckButtonInCode((X))
#endif


//-------- Functions -------------
unsigned char RxCode (void);
unsigned short CheckCodeInMemory_FLASH (unsigned int);
unsigned short CheckBaseCodeInMemory_FLASH (unsigned int);
unsigned int CheckIndexInMemory_FLASH (unsigned short);

//void RxCode_Disable(void);
//void RxCode_Enable(void);
void Timer_Interrupt_Handler (unsigned short);
unsigned char CheckForButtons(unsigned short *, unsigned int *);
unsigned char CheckButtonRemote (unsigned short,  unsigned short);



#endif    /* _RWS317_H_ */

//--- end of file ---//

