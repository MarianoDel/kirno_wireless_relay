//------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### MAIN.C #########################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f0xx.h"
#include "hard.h"
#include "gpio.h"

#include "core_cm0.h"
#include "test_functions.h"
#include "rws317.h"
#include "flash_program.h"
#include "tim.h"
#include "parameters.h"

#include <stdio.h>
#include <string.h>
#include "it.h"

// #include "gestion.h"
#include "manager.h"
#include "programing.h"


// Externals -------------------------------------------------------------------
// ------- Externals para timers -------
// volatile unsigned char timer_1seg = 0;
volatile unsigned short timer_standby = 0;
// volatile unsigned short timer_led_comm = 0;
// volatile unsigned short timer_keypad_enabled = 0;
volatile unsigned short wait_ms_var = 0;

// // ------- Externals para el Display -------
// volatile unsigned short display_timeout = 0;

// // ------- Externals para el keypad -------
// volatile unsigned char keypad_timeout = 0;
// volatile unsigned short keypad_interdigit_timeout = 0;

// // ------- Externals del Puerto serie  -------
// volatile unsigned char tx1buff[SIZEOF_DATA];
// volatile unsigned char rx1buff[SIZEOF_DATA];
// volatile unsigned char usart1_have_data = 0;
// volatile unsigned char binary_full = 0;
// mem_bkp_typedef memory_backup;
// filesystem_typedef files;
// unsigned char next_pckt = 0;
// unsigned char file_done = 0;

volatile unsigned char pilot_code = 0;
// volatile unsigned short code0 = 0;
// volatile unsigned short code1 = 0;
// volatile unsigned char errorcode = 0;

// // para el buzzer en hard.c
// volatile unsigned short buzzer_timeout = 0;

// #ifdef CONFIGURATION_IN_FLASH
// parameters_typedef __attribute__ ((section (".myStruct"))) const param_struct = {30, 5, 30, 5, 30, 5, 30, 5, 3000, 10000};

// unsigned char b1t;
// unsigned char b1r;
// unsigned char b2t;
// unsigned char b2r;
// unsigned char b3t;
// unsigned char b3r;
// unsigned char b4t;
// unsigned char b4r;

// #endif

// #ifdef CONFIGURATION_IN_SST
// //parameters_typedef param_struct = {30, 5, 30, 5, 30, 5, 30, 5, 3000, 10000};
// parameters_typedef param_struct;
// #endif
// //parameters_typedef * p_params;



// volatile short v_samples1[16];
// volatile short v_samples2[16];
// volatile unsigned char update_samples = 0;
// volatile unsigned char buff_in_use = 1;

// Globals ---------------------------------------------------------------------
// static __IO uint32_t TimingDelay;

// unsigned short counter = TIM3_ARR;


// unsigned char siren_state = 0;
// unsigned short freq_us = 0;
// unsigned char siren_steps = 0;


// //TODO: para nuevo codigo
// // unsigned short new_global_position = 0;
// // unsigned char new_global_button = 0;
// //FIN: para nuevo codigo

// volatile unsigned short wait_for_code_timeout;
// volatile unsigned short interdigit_timeout;

// volatile unsigned short siren_timeout;

// volatile unsigned short f12_plus_timer;
// volatile unsigned char button_timer_secs;
// volatile unsigned short button_timer_internal;

// #ifdef SOFTWARE_VERSION_2_5
// #define	BUTTON4_5MINS_TT    (1000 * 60 * 5)
// volatile unsigned int button4_only5mins_timeout = 0;
// #endif

// //--- Respecto del KeyPad
// unsigned char remote_keypad_state = 0;
// unsigned char unlock_by_remote = 0;

// //--- Respecto de las alarmas y botones
// //unsigned short last_act = 0;
// unsigned short last_one_or_three = 0;
// unsigned short last_two = 0;
// unsigned char alarm_state = 0;
// unsigned char repetition_counter = 0;

// //--- Audio Global Variables
// #define FILE_OFFSET 44
// unsigned char audio_state = 0;
// unsigned char numbers_speak[LAST_NUMBER_SPEAK];
// unsigned char * p_numbers_speak;
// unsigned int * p_files_addr;
// unsigned int * p_files_length;
// unsigned int current_size = 0;


// Module Functions ------------------------------------------------------------
void SysTickError (void);
void TimingDelay_Decrement(void);

// unsigned char FuncAlarm (unsigned char);
//unsigned char FuncProgCod (unsigned char);


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    // Systick Timer Activation
    if (SysTick_Config(48000))
        SysTickError();

    // Hardware Tests
    // TF_Hardware_Tests ();

    // --- main program inits. ---
    // RxCode();    // stop ints
    // TIM_14_Init();

    while (1)
    {
	if (Check_Sw_Learn() > SW_NO)
	    Programing();
	
	// FuncPortonKirno();
    }
    
// #ifdef HARDWARE_VERSION_2_0
//     // --- start peripherals
//     // Init ADC with DMA
//     DMAConfig ();
//     DMA_ENABLE;
    
//     AdcConfig();
//     AdcStart();
    
//     //-- DAC init for signal generation
//     DAC_Config ();
//     DAC_Output1(0);

//     // Init Usart3
//     Usart3Config();

//     // Init Tim3 for neopixel
//     TIM3_Init();
    

//     unsigned char main_state = 0;
//     // unsigned short dummy16 = 0;
//     //unsigned char i;	//, j;
//     char str [42];

//     unsigned short position, mass_erase_position;
//     unsigned short seq_number = 0;
//     unsigned short code_position;
//     unsigned int code;
//     unsigned char switches, switches_posi0, switches_posi1, switches_posi2;
//     //unsigned char switches_last;
//     unsigned char result;

//     //unsigned short * p_mem_dump;
//     //unsigned char button_remote = 0;
//     unsigned char digit_remote = 0;
//     unsigned char keypad_locked = 1;
//     unsigned char remote_is_working = 0;

//     CE_OFF;
//     WP_OFF;
//     PS_OFF;
//     OE_OFF;
//     FPLUS_OFF;
// #ifdef USE_F12_PLUS_WITH_SM
//     F12_State_Machine_Reset();
// #else
//     F12PLUS_OFF;
// #endif
//     F5PLUS_OFF;
//     BUZZER_OFF;
//     LED_OFF;



// #if (!defined PROGRAMA_FACTORY_TEST) && (!defined PROGRAMA_FACTORY_TEST_ONLY_RF)
//     Wait_ms(2000);
// #endif

//     /* TIMs configuration ------------------------------------------------------*/
//     //para audio y sirena
//     TIM_1_Init();

//     //para codigos
// #if (!defined PROGRAMA_PORTON_KIRNO) && (!defined PROGRAMA_FACTORY_TEST_ONLY_RF)
//     RxCode();	//trabo la interrupcion
//     TIM_14_Init();
// #endif
    
//     Update_TIM1_CH1(100);
//     Power_Ampli_Disa();


//     //--- EMPIEZO PROGRAMA DE GESTION y GRABAR MEMORIA SST ---//
// #ifdef PROGRAMA_DE_GESTION

//     FuncGestion();

// #endif
//     //--- FIN PROGRAMA DE GESTION y GRABAR MEMORIA SST ---//    

//     //--- EMPIEZO PROGRAMA DE PORTON KIRNO  ---//
// #ifdef PROGRAMA_PORTON_KIRNO



// #endif
//     //--- FIN PROGRAMA DE PORTON KIRNO ---//    
    
    


    return 0;
    
}
//--- End of Main ---//


// One_ms Interrupt
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (pilot_code)
        pilot_code--;

    Hard_Timeouts ();
    // if (wait_for_code_timeout)
    //     wait_for_code_timeout--;

    // //cuenta 1 segundo
    // if (button_timer_internal)
    //     button_timer_internal--;
    // else
    // {
    //     if (button_timer_secs)
    //     {
    //         button_timer_secs--;
    //         button_timer_internal = 1000;
    //     }
    // }
}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}


//--- end of file ---//
