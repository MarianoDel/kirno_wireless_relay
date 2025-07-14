//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### RF_RX_CODES.C #################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "rf_rx_codes.h"
#include "codes.h"

#include <stdio.h>


// Define for configs ----------------------------------------------------------
// #define USE_USART_FOR_DEBUG


#ifdef USE_USART_FOR_DEBUG
#include "usart.h"
#endif


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    RF_WAIT_SILENCE_INIT,
    RF_WAIT_SILENCE,
    RF_RX,
    RF_GET_CODE_HT,
    RF_GET_CODE_PT_EV
    
} rf_codes_e;



// Externals -------------------------------------------------------------------



// Globals ---------------------------------------------------------------------
rf_codes_e rf_state = RF_WAIT_SILENCE_INIT;
unsigned char bit_cnt = 0;


// Private Module Functions ----------------------------------------------------



// Module Functions ------------------------------------------------------------
resp_t Rf_Get_Codes (rf_rx_codes_t * new_code_st)
{
    unsigned int new_code = 0;
    unsigned short new_lambda = 0;
    resp_t resp = resp_continue;

#ifdef USE_USART_FOR_DEBUG
    char s_buf [100] = { 0 };
#endif
    
    switch (rf_state)
    {
    case RF_WAIT_SILENCE_INIT:
        CodesWaitFiveReset();
        rf_state++;
        break;
            
    case RF_WAIT_SILENCE:
        resp = CodesWaitFive();

        if (resp == resp_ok)
        {
            //estuve 5ms sin nada
            rf_state = RF_RX;
#ifdef USE_USART_FOR_DEBUG		    
            Usart1Send((char *) "nuevo header\n");
#endif
            CodesRecvCode16Reset();
            resp = resp_continue;
        }

        if (resp == resp_error)
            rf_state = RF_WAIT_SILENCE_INIT;
            
        break;

    case RF_RX:
        resp = CodesRecvCode16(&bit_cnt);

        if (resp != resp_continue)
        {
            if (resp == resp_ok)
            {
#ifdef USE_USART_FOR_DEBUG
                sprintf(s_buf, "bits: %d OK\n", bit_cnt);
                Usart1Send(s_buf);
#endif
                rf_state = RF_GET_CODE_HT;
                resp = resp_continue;
            }

            if (resp == resp_error)
            {
#ifdef USE_USART_FOR_DEBUG		
                // for debug, shows how many bits gets
                sprintf(s_buf, "bits: %d ERR\n", bit_cnt);
                Usart1Send(s_buf);
#endif
                rf_state = RF_WAIT_SILENCE_INIT;
            }
        }
        break;

    case RF_GET_CODE_HT:
        resp = CodesUpdateTransitionsHT(bit_cnt, &new_code, &new_lambda);

#ifdef USE_USART_FOR_DEBUG	
        if (resp == resp_ok)
        {
            sprintf(s_buf, "code: 0x%x lambda: %d bits: %d HT\n",
                    new_code,
                    new_lambda,
                    bit_cnt);

            Usart1Send(s_buf);
            new_code_st->bits = bit_cnt;
            new_code_st->code = new_code;
            new_code_st->lambda = new_lambda;

            rf_state = RF_WAIT_SILENCE_INIT;
        }
        else
        {
            rf_state = RF_GET_CODE_PT_EV;
        }
#else
        if (resp == resp_ok)
        {
            new_code_st->bits = bit_cnt;
            new_code_st->code = new_code;
            new_code_st->lambda = new_lambda;

            rf_state = RF_WAIT_SILENCE_INIT;
        }
        else
        {
            rf_state = RF_GET_CODE_PT_EV;
        }
#endif
        break;
            
    case RF_GET_CODE_PT_EV:
        resp = CodesUpdateTransitionsPT_EV(bit_cnt, &new_code, &new_lambda);

#ifdef USE_USART_FOR_DEBUG
        if (resp == resp_ok)
        {
            sprintf(s_buf, "code: 0x%x lambda: %d bits: %d PT or EV\n",
                    new_code,
                    new_lambda,
                    bit_cnt);

            new_code_st->bits = bit_cnt;
            new_code_st->code = new_code;
            new_code_st->lambda = new_lambda;

            rf_state = RF_WAIT_SILENCE_INIT;
        }
        else
        {
            sprintf(s_buf, "code error\n");
        }

        Usart1Send(s_buf);
#else
        if (resp == resp_ok)
        {
            new_code_st->bits = bit_cnt;
            new_code_st->code = new_code;
            new_code_st->lambda = new_lambda;
            rf_state = RF_WAIT_SILENCE_INIT;
        }
#endif
        rf_state = RF_WAIT_SILENCE_INIT;
        break;
            
    default:
        rf_state = RF_WAIT_SILENCE_INIT;
        break;
    }

    return resp;
}

//--- end of file ---//
