//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PROGRAMING.C #################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "programing.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "codes.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_standby;


// Globals ---------------------------------------------------------------------
programing_sm_t prog_state = PROG_INIT;


// Private Module Functions ----------------------------------------------------
resp_t Programing_Codes (programing_codes_t *);


// Module Functions ------------------------------------------------------------
void Programing (void)
{
    while (1)
    {
        switch (prog_state)
        {
        case PROG_INIT:
	    if (Sw_Learn_Is_On())
		Led_On();

            if (Check_Sw_Learn() == SW_NO)
	    {
                prog_state = PROG_SAVE_B1;
		Led_Off();
		Hard_Led_Change_Bips (1);
	    }
	    
            if (Check_Sw_Learn() == SW_HALF)
	    {
                prog_state = PROG_CHANGE_MODE;
		Led_Off();
	    }
            break;

        case PROG_SAVE_B1:
	    Hard_Led_Blinking_Update ();

            if (Check_Sw_Learn() > SW_NO)
	    {
                prog_state = PROG_SAVE_B1_WAIT_FREE;
		Hard_Led_Change_Bips (2);
	    }
	    
            // resp = Programing_Codes (&my_codes);
            
            // if (resp == resp_ok)
            // {
            //     sprintf(s_send, "finded! rxcode: 0x%x rxlambda: %d rxbits: %d\n",
            //             my_codes.code,
            //             my_codes.lambda,
            //             my_codes.bits);
            //     // Usart1Send(s_send);

            //     //chequeo parametros del codigo y activo
            //     if ((my_codes.bits == 28) &&
            //         (my_codes.lambda > 300) &&
            //         (my_codes.lambda < 330))
            //     {
            //         // control 1 B3 y B4
            //         // control 2 B3 y B4
            //         if ((my_codes.code == 0x63d1f45) ||
            //             (my_codes.code == 0x63d1f15) ||
            //             (my_codes.code == 0x7401045) ||
            //             (my_codes.code == 0x7401015))
            //         {
            //             prog_state = PROG_OUTPUT_TO_ACTIVATE;                        
            //         }
            //     }
            // }
            break;

        case PROG_SAVE_B1_WAIT_FREE:
	    Hard_Led_Blinking_Update ();

	    if (Check_Sw_Learn() == SW_NO)
		prog_state = PROG_SAVE_B2;
	    
	    break;
	    
        case PROG_SAVE_B2:
	    Hard_Led_Blinking_Update ();

            if (Check_Sw_Learn() > SW_NO)
	    {
                prog_state = PROG_SAVE_B2_WAIT_FREE;
		Hard_Led_Change_Bips (3);
	    }
	    break;

        case PROG_SAVE_B2_WAIT_FREE:
	    Hard_Led_Blinking_Update ();

	    if (Check_Sw_Learn() == SW_NO)
		prog_state = PROG_SAVE_B3;
	    
	    break;
	    
        case PROG_SAVE_B3:
	    Hard_Led_Blinking_Update ();

            if (Check_Sw_Learn() > SW_NO)
	    {
                prog_state = PROG_SAVE_B3_WAIT_FREE;
		Hard_Led_Change_Bips (4);
	    }
	    break;

        case PROG_SAVE_B3_WAIT_FREE:
	    Hard_Led_Blinking_Update ();

	    if (Check_Sw_Learn() == SW_NO)
		prog_state = PROG_SAVE_B4;
	    
	    break;
	    
        case PROG_SAVE_B4:
	    Hard_Led_Blinking_Update ();

            if (Check_Sw_Learn() > SW_NO)
	    {
                prog_state = PROG_SAVE_B4_WAIT_FREE;
		Hard_Led_Change_Bips (1);
	    }
	    break;
	    
        case PROG_SAVE_B4_WAIT_FREE:
	    Hard_Led_Blinking_Update ();

	    if (Check_Sw_Learn() == SW_NO)
		prog_state = PROG_SAVE_B1;
	    
	    break;

        case PROG_CHANGE_MODE:
            // if (!timer_standby)
            // {
            //     if (minutes)
            //     {
            //         minutes--;
            //         timer_standby = 60000;
            //     }
            //     else
            //     {
            //         // FPLUS_OFF;
            //         // BuzzerCommands(BUZZER_LONG_CMD, 3);
            //         // timer_standby = 3000;
            //         // Usart1Send("Output is OFF\n");
            //         prog_state = PROG_INIT;
            //     }
            // }

            // if ((FPLUS) && (!siren_timeout))
            // {
                // siren_timeout = 5000;
                // BuzzerCommands(BUZZER_HALF_CMD, 2);
            // }
            break;
            

        default:
            prog_state = PROG_INIT;
            break;

        }
    }
}

// typedef enum {
//     KC_WAIT_SILENCE_INIT,
//     KC_WAIT_SILENCE,
//     KC_RX,
//     KC_GET_CODE_HT,
//     KC_GET_CODE_PT_EV
    
// } kirno_codes_t;

// kirno_codes_t kc_state = KC_WAIT_SILENCE_INIT;

// unsigned char i = 0;
// resp_t Programing_Codes (programing_codes_t * new_code_st)
// {
//     unsigned int new_code = 0;
//     unsigned short new_lambda = 0;
//     char s_buf [100] = { 0 };
//     resp_t resp = resp_continue;

    
//     switch (kc_state)
//     {
//     case KC_WAIT_SILENCE_INIT:
//         CodesWaitFiveReset();
//         kc_state++;
//         break;
            
//     case KC_WAIT_SILENCE:

//         resp = CodesWaitFive();

//         if (resp == resp_ok)
//         {
//             //estuve 5ms sin nada
//             LED_ON;
//             OCODE_ON;

//             kc_state = KC_RX;
//             // Usart1Send((char *) "nuevo header\n");
//             CodesRecvCode16Reset();
//             resp = resp_continue;
//         }

//         if (resp == resp_error)
//             kc_state = KC_WAIT_SILENCE_INIT;
            
//         break;

//     case KC_RX:
//         resp = CodesRecvCode16(&i);

//         if (resp != resp_continue)
//         {
//             if (resp == resp_ok)
//             {
//                 // sprintf(s_buf, "bits: %d OK\n", i);
//                 // Usart1Send(s_buf);
//                 kc_state = KC_GET_CODE_HT;
//                 resp = resp_continue;
//             }

//             if (resp == resp_error)
//             {
//                 // for debug, shows how many bits gets
//                 // sprintf(s_buf, "bits: %d ERR\n", i);
//                 // Usart1Send(s_buf);
//                 kc_state = KC_WAIT_SILENCE_INIT;
//             }
                
//             LED_OFF;
//             OCODE_OFF;
//         }
//         break;

//     case KC_GET_CODE_HT:
//         resp = CodesUpdateTransitionsHT(i, &new_code, &new_lambda);
                        
//         if (resp == resp_ok)
//         {
//             sprintf(s_buf, "code: 0x%x lambda: %d bits: %d HT\n",
//                     new_code,
//                     new_lambda,
//                     i);

//             // Usart1Send(s_buf);
//             new_code_st->bits = i;
//             new_code_st->code = new_code;
//             new_code_st->lambda = new_lambda;

//             kc_state = KC_WAIT_SILENCE_INIT;
//         }
//         else
//         {
//             kc_state = KC_GET_CODE_PT_EV;
//         }
//         break;
            
//     case KC_GET_CODE_PT_EV:
//         resp = CodesUpdateTransitionsPT_EV(i, &new_code, &new_lambda);

//         if (resp == resp_ok)
//         {
//             sprintf(s_buf, "code: 0x%x lambda: %d bits: %d PT or EV\n",
//                     new_code,
//                     new_lambda,
//                     i);

//             new_code_st->bits = i;
//             new_code_st->code = new_code;
//             new_code_st->lambda = new_lambda;

//             kc_state = KC_WAIT_SILENCE_INIT;
//         }
//         else
//         {
//             sprintf(s_buf, "code error\n");
//         }

//         // Usart1Send(s_buf);
//         kc_state = KC_WAIT_SILENCE_INIT;
//         break;
            
//     default:
//         kc_state = KC_WAIT_SILENCE_INIT;
//         break;

//     }

//     return resp;
// }

//--- end of file ---//
