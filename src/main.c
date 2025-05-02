//------------------------------------------------------
// #### PROYECTO PANEL ALARMA VAPORE - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### MAIN.C #########################################
//------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "hard.h"
#include "gpio.h"
#include "spi.h"
#include "sst25.h"
#include "comm.h"

#include "core_cm0.h"
#include "rws317.h"
#include "flash_program.h"
#include "sst25codes.h"
#include "usart.h"
#include "tim.h"
#include "parameters.h"

#include <stdio.h>
#include <string.h>
#include "it.h"
#include "display_7seg.h"
#include "keypad.h"

#include "gestion.h"
#include "porton_kirno.h"
#include "factory_test.h"

//TODO: para pruebas nuevo soft
// #include "func_alarm.h"
//FIN: para pruebas nuevo soft

/* Externals ------------------------------------------------------------------*/
// ------- Externals para timers -------
volatile unsigned char timer_1seg = 0;
volatile unsigned short timer_standby = 0;
volatile unsigned short timer_led_comm = 0;
volatile unsigned short timer_keypad_enabled = 0;
volatile unsigned short wait_ms_var = 0;

// ------- Externals para el Display -------
volatile unsigned short display_timeout = 0;

// ------- Externals para el keypad -------
volatile unsigned char keypad_timeout = 0;
volatile unsigned short keypad_interdigit_timeout = 0;

// ------- Externals del Puerto serie  -------
volatile unsigned char tx1buff[SIZEOF_DATA];
volatile unsigned char rx1buff[SIZEOF_DATA];
volatile unsigned char usart1_have_data = 0;
volatile unsigned char binary_full = 0;
mem_bkp_typedef memory_backup;
filesystem_typedef files;
unsigned char next_pckt = 0;
unsigned char file_done = 0;

volatile unsigned char pilot_code = 0;
volatile unsigned short code0 = 0;
volatile unsigned short code1 = 0;
volatile unsigned char errorcode = 0;

// para el buzzer en hard.c
volatile unsigned short buzzer_timeout = 0;

#ifdef CONFIGURATION_IN_FLASH
parameters_typedef __attribute__ ((section (".myStruct"))) const param_struct = {30, 5, 30, 5, 30, 5, 30, 5, 3000, 10000};

unsigned char b1t;
unsigned char b1r;
unsigned char b2t;
unsigned char b2r;
unsigned char b3t;
unsigned char b3r;
unsigned char b4t;
unsigned char b4r;

#endif

#ifdef CONFIGURATION_IN_SST
//parameters_typedef param_struct = {30, 5, 30, 5, 30, 5, 30, 5, 3000, 10000};
parameters_typedef param_struct;
#endif
//parameters_typedef * p_params;



volatile short v_samples1[16];
volatile short v_samples2[16];
volatile unsigned char update_samples = 0;
volatile unsigned char buff_in_use = 1;

// Globals ---------------------------------------------------------------------
static __IO uint32_t TimingDelay;

unsigned short counter = TIM3_ARR;


unsigned char siren_state = 0;
unsigned short freq_us = 0;
unsigned char siren_steps = 0;


//TODO: para nuevo codigo
// unsigned short new_global_position = 0;
// unsigned char new_global_button = 0;
//FIN: para nuevo codigo

volatile unsigned short wait_for_code_timeout;
volatile unsigned short interdigit_timeout;

volatile unsigned short siren_timeout;

volatile unsigned short f12_plus_timer;
volatile unsigned char button_timer_secs;
volatile unsigned short button_timer_internal;

#ifdef SOFTWARE_VERSION_2_5
#define	BUTTON4_5MINS_TT    (1000 * 60 * 5)
volatile unsigned int button4_only5mins_timeout = 0;
#endif

//--- Respecto del KeyPad
unsigned char remote_keypad_state = 0;
unsigned char unlock_by_remote = 0;

//--- Respecto de las alarmas y botones
//unsigned short last_act = 0;
unsigned short last_one_or_three = 0;
unsigned short last_two = 0;
unsigned char alarm_state = 0;
unsigned char repetition_counter = 0;

//--- Audio Global Variables
#define FILE_OFFSET 44
unsigned char audio_state = 0;
unsigned char numbers_speak[LAST_NUMBER_SPEAK];
unsigned char * p_numbers_speak;
unsigned int * p_files_addr;
unsigned int * p_files_length;
unsigned int current_size = 0;


// Module Functions ------------------------------------------------------------
void SysTickError (void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

unsigned char FuncAlarm (unsigned char);
//unsigned char FuncProgCod (unsigned char);


// ------- de los switches ---
unsigned char CheckRemoteKeypad (unsigned char *, unsigned char *, unsigned char *, unsigned short *);

// ------- de la sirena ------
void SirenCommands(unsigned char);
void UpdateSiren (void);
// ------- del audio -------
void UpdateAudio (void);
void VectorToSpeak (unsigned char);
void PositionToSpeak(unsigned short);
// ------- para la bateria -------
void UpdateBattery (void);

// ------- para la salida F12+ -------
#ifdef USE_F12_PLUS_WITH_SM
void F12_State_Machine_Start (void);
void F12_State_Machine_Reset (void);
void F12_State_Machine (void);
#endif

#ifdef PROGRAMA_CHICKEN_BUCLE
//funcion de alarmas, revisa codigo en memoria y actua en consecuencia
unsigned char FuncAlarmChickenBucle (unsigned char sms_alarm);
#endif

//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned char main_state = 0;
    // unsigned short dummy16 = 0;
    //unsigned char i;	//, j;
    char str [42];

    unsigned short position, mass_erase_position;
    unsigned short seq_number = 0;
    unsigned short code_position;
    unsigned int code;
    unsigned char switches, switches_posi0, switches_posi1, switches_posi2;
    //unsigned char switches_last;
    unsigned char result;

    //unsigned short * p_mem_dump;
    //unsigned char button_remote = 0;
    unsigned char digit_remote = 0;
    unsigned char keypad_locked = 1;
    unsigned char remote_is_working = 0;

    //GPIO Configuration.
    GPIO_Config();
    CE_OFF;
    WP_OFF;
    PS_OFF;
    OE_OFF;
    FPLUS_OFF;
#ifdef USE_F12_PLUS_WITH_SM
    F12_State_Machine_Reset();
#else
    F12PLUS_OFF;
#endif
    F5PLUS_OFF;
    BUZZER_OFF;
    LED_OFF;

    // Systick Timer Activation
    if (SysTick_Config(48000))
        SysTickError();


#if (!defined PROGRAMA_FACTORY_TEST) && (!defined PROGRAMA_FACTORY_TEST_ONLY_RF)
    Wait_ms(2000);
#endif

    /* TIMs configuration ------------------------------------------------------*/
    //para audio y sirena
    TIM_1_Init();

    //para codigos
#if (!defined PROGRAMA_PORTON_KIRNO) && (!defined PROGRAMA_FACTORY_TEST_ONLY_RF)
    RxCode();	//trabo la interrupcion
    TIM_14_Init();
#endif
    
    Update_TIM1_CH1(100);
    Power_Ampli_Disa();

    /* SPI configuration ------------------------------------------------------*/
    SPI_Config();		//activo sin interrupcion

    /* USART configuration -----------------------------------------------------*/
    Usart1Config();    //activo con int priority 5

    /* Welcome Code ------------------------------------------------------------*/    
    //---- Defines from hard.h -----//
#ifdef PROGRAMA_DE_GESTION
    Usart1ChangeBaud(USART_115200);
#endif
    Usart1Send((char *) " - Kirno Technology - STM32F030K6\r\n");
    Wait_ms(100);
    Usart1Send((char *) "Sistema de Alarma ALERTA VECINAL\r\n");
    Wait_ms(100);
    
#ifdef HARD
    Usart1Send((char *) HARD);
    Wait_ms(100);
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send((char *) SOFT);
    Wait_ms(100);
#else
#error	"No Soft Version defined in hard.h file"
#endif

#ifdef KIND_OF_PROGRAM
    Usart1Send((char *) KIND_OF_PROGRAM);
    Wait_ms(100);
#else
#error	"No Kind of Program selected in hard.h file"
#endif

    //-- HARDWARE pero para Display --
#ifdef WITH_WELCOME_CODE_ON_DISPLAY
#ifdef HARDWARE_VERSION_1_3
    Display_VectorToStr("h1.3");
    while (!Display_IsFree())
        Display_UpdateSM();
#endif

#ifdef SOFTWARE_VERSION_2_5
    Display_VectorToStr("s2.5");
    while (!Display_IsFree())
        Display_UpdateSM();    
#endif

#ifdef SOFTWARE_VERSION_2_4
    Display_VectorToStr("s2.4");
    while (!Display_IsFree())
        Display_UpdateSM();    
#endif
    
#ifdef SOFTWARE_VERSION_2_1
    Display_VectorToStr("s2.1");
    while (!Display_IsFree())
        Display_UpdateSM();    
#endif
    
#ifdef SOFTWARE_VERSION_2_0
    Display_VectorToStr("s2.0");
    while (!Display_IsFree())
        Display_UpdateSM();    
#endif
#endif
    /* End of Welcome Code ------------------------------------------------------*/    
    
    //--- EMPIEZO PROGRAMA DE GESTION y GRABAR MEMORIA SST ---//
#ifdef PROGRAMA_DE_GESTION

    FuncGestion();

#endif
    //--- FIN PROGRAMA DE GESTION y GRABAR MEMORIA SST ---//    

    //--- EMPIEZO PROGRAMA DE PORTON KIRNO  ---//
#ifdef PROGRAMA_PORTON_KIRNO

    FuncPortonKirno();

#endif
    //--- FIN PROGRAMA DE PORTON KIRNO ---//    
    
    
    //--- EMPIEZO PROGRAMA DE PRUEBAS EN FABRICA ---//
#ifdef PROGRAMA_FACTORY_TEST

    FuncFactoryTest();

#endif
#ifdef PROGRAMA_FACTORY_TEST_ONLY_RF

    FuncFactoryTest_Only_Rf();

#endif
    //--- FIN PROGRAMA DE PRUEBAS EN FABRICA ---//

    //--- INICIO PROGRAMA DE PRODUCCION ---//
    //reset a la SM del display
    Display_ResetSM();
    //apago el display
    Display_ShowNumbers(DISPLAY_NONE);
    
    BuzzerCommands(BUZZER_LONG_CMD, 2);

    p_numbers_speak = numbers_speak;	//seteo puntero

    //Power_Ampli_Ena();
    //Ampli_to_Sirena();
    //SirenCommands(SIREN_MULTIPLE_UP_CMD);
    //SirenCommands(SIREN_MULTIPLE_DOWN_CMD);

    //cargo los valores de memoria
    LoadFilesIndex();
    ShowFileSystem();

    //reviso si esta grabada la memoria
    if (files.posi0 == 0xFFFFFFFF)
    {
        //memoria no grabada
        Display_ShowNumbers(DISPLAY_ERROR);
        BuzzerCommands(BUZZER_LONG_CMD, 10);
        while (1)
            UpdateBuzzer();
    }

#ifdef CONFIGURATION_IN_FLASH
    b1t = param_struct.b1t;
    b1r = param_struct.b1r;
    b2t = param_struct.b2t;
    b2r = param_struct.b2r;
    b3t = param_struct.b3t;
    b3r = param_struct.b3r;
    b4t = param_struct.b4t;
    b4r = param_struct.b4r;
#endif

#ifdef CONFIGURATION_IN_SST
    LoadConfiguration();
    //reviso configuracion de memoria
    if (param_struct.b1t == 0xFF)
    {
        //memoria no configurada
        Display_ShowNumbers(DISPLAY_ERROR2);
        BuzzerCommands(BUZZER_LONG_CMD, 10);
        while (1)
            UpdateBuzzer();
    }
    ShowConfiguration();
#endif

    //si esta todo bien configurado prendo el led
    LED_ON;

    while (1)
    {
        switch (main_state)
        {
        case MAIN_INIT:

            switches_posi0 = 0;
            switches_posi1 = 0;
            switches_posi2 = 0;
            position = 0;
            //code = 0;

#ifdef CON_BLOQUEO_DE_KEYPAD
            if (!interdigit_timeout)
            {
                if (keypad_locked)
                    main_state = MAIN_MAIN;
                else    //cambio 21-09-18 a unica forma de hacer un lock es estar 60s sin tocar nada
                    main_state = MAIN_TO_UNLOCK;
            }
#else
            main_state = MAIN_UNLOCK;
#endif
            break;

        case MAIN_MAIN:
            if (CheckKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position) == KNUMBER_FINISH)
            {
                if (position == 951)
                {
                    keypad_locked = 0;
                    Usart1Send("Master Unlock\r\n");
                    main_state = MAIN_TO_UNLOCK;
                }
                else
                {
                    unsigned short code_in_mem = SST_CheckIndexInMemory(1000);

                    if ((code_in_mem == 0xFFFF) && (position == 416))
                    {
                        Usart1Send("User default Unlock\r\n");
                        main_state = MAIN_TO_UNLOCK;
                        keypad_locked = 0;
                    }

                    if (position == code_in_mem)                        
                    {
                        Usart1Send("User Unlock\r\n");                        
                        main_state = MAIN_TO_UNLOCK;
                        keypad_locked = 0;
                    }
                }
                //position = (unsigned short) SST_CheckIndexInMemory(1000);
                //sprintf(str, "user code: %03d\r\n", position);
                //Usart1Send(str);
            }

            //ahora reviso si hay algun control enviando y si es el remote_keypad o un control de alarma
            switches = CheckRemoteKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
            if (switches == RK_MUST_BE_CONTROL)
            {
                //TODO: codigo original OK
                alarm_state = ALARM_START;	//resetea la maquina de estados de FuncAlarm()
                main_state = MAIN_IN_ALARM;
                //FIN: codigo original OK

                //TODO: codigo nuevo a probar
                // no funciona bien, ver mas abajo
                // unsigned short code_position = 0;
                // unsigned char button = 0;
                // code_position = CheckBaseCodeInMemory(code);
                // if ((code_position >= 0) && (code_position <= 1023))
                //     button = SST_CheckButtonInCode(code);

                // if ((button > 0) && (button <= 4))
                // {
                //     Func_Alarm_Reset_SM();
                //     new_global_position = code_position;
                //     new_global_button = button;
                //     main_state = MAIN_IN_ALARM;
                // }
                //FIN: codigo nuevo a probar
                
            }
            else if (switches == RK_NUMBER_FINISH)
            {
                if (position == 951)
                {
                    Usart1Send("Master Remote Unlock\r\n");
                    main_state = MAIN_TO_UNLOCK;
                    unlock_by_remote = 1;
                    keypad_locked = 0;
                }
                else
                {
                    unsigned short code_in_mem = SST_CheckIndexInMemory(1000);

                    if ((code_in_mem == 0xFFFF) && (position == 416))
                    {
                        Usart1Send("User default Remote Unlock\r\n");
                        main_state = MAIN_TO_UNLOCK;
                        unlock_by_remote = 1;
                        keypad_locked = 0;
                    }

                    if (position == code_in_mem)
                    {
                        Usart1Send("User Remote Unlock\r\n");
                        main_state = MAIN_TO_UNLOCK;
                        unlock_by_remote = 1;
                        keypad_locked = 0;
                    }
                }
            }

            // check at last for a SMS activation
            if (CheckSMS())
            {
                alarm_state = ALARM_START;	//resetea la maquina de estados de FuncAlarm()
                main_state = MAIN_IN_ALARM;
            }
            
            break;

        case MAIN_TO_UNLOCK:
            timer_keypad_enabled = 60000;

            //si se esta mostrando algo espero
            if (Display_IsFree())
            {
                Display_ShowNumbers(DISPLAY_REMOTE);
                digit_remote = 1;
            }
            else
                digit_remote = 0;

            main_state = MAIN_UNLOCK;
            break;

        case MAIN_UNLOCK:
            //este es la caso principal de selcciones desde aca segun lo elegido
            //voy pasando a otros casos

            switches = CheckKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
            if (switches == KCANCEL)
            {
                //se cancelo y no debo ir a ningun lado, me quedo aca
                digit_remote = 0;
            }

            if (switches == KNUMBER_FINISH)
            {
                if (position == 800)
                {
                    Display_ShowNumbers(DISPLAY_PROG);
                    main_state = MAIN_TO_CHANGE_USER_PASSWORD;
                    Usart1Send("Change User Password\r\n");
                    wait_for_code_timeout = param_struct.wait_for_code;
                }
                else
                {
                    Display_ShowNumbers(DISPLAY_LINE);
                    main_state = MAIN_TO_SAVE_AT_LAST;
                    wait_for_code_timeout = param_struct.wait_for_code;
                    sprintf(str, "Guardar en: %03d\r\n", position);
                    Usart1Send(str);
                }
            }

            if (!digit_remote)
            {
                if (Display_IsFree())
                {
                    Display_ShowNumbers(DISPLAY_REMOTE);
                    digit_remote = 1;
                }
            }


            //ahora reviso si hay algun control enviando y si es el remote_keypad o un control de alarma
            switches = CheckRemoteKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
            if (switches == RK_MUST_BE_CONTROL)
            {
                //TODO: codigo original OK
                alarm_state = ALARM_START;	//resetea la maquina de estados de FuncAlarm()
                main_state = MAIN_IN_ALARM;
                //FIN: codigo original OK

                //TODO: codigo nuevo a probar
                // no funciona bien, ver mas abajo
                // unsigned short code_position = 0;
                // unsigned char button = 0;
                // code_position = CheckBaseCodeInMemory(code);
                // if ((code_position >= 0) && (code_position <= 1023))
                //     button = SST_CheckButtonInCode(code);

                // if ((button > 0) && (button <= 4))
                // {
                //     Func_Alarm_Reset_SM();
                //     new_global_position = code_position;
                //     new_global_button = button;
                //     main_state = MAIN_IN_ALARM;
                // }
                //FIN: codigo nuevo a probar
                
            }
            else if (switches == RK_NUMBER_FINISH)
            {
                remote_is_working = 1;
                wait_for_code_timeout = param_struct.wait_for_code;
                sprintf(str, "Keypad Remoto guardar en: %03d\r\n", position);
                Usart1Send(str);
                main_state = MAIN_TO_SAVE_AT_LAST;                
            }
            else if (switches == RK_CANCEL)
                digit_remote = 0;
            

#ifdef CON_BLOQUEO_DE_KEYPAD
            if (!timer_keypad_enabled)
            {
                keypad_locked = 1;
                Usart1Send("Keypad Locked\r\n");
                main_state = MAIN_MAIN;
                unlock_by_remote = 0;
                remote_is_working = 0;
                Display_ShowNumbers(DISPLAY_NONE);
            }
#endif
            // check at last for a SMS activation
            if (CheckSMS())
            {
                alarm_state = ALARM_START;	//resetea la maquina de estados de FuncAlarm()
                main_state = MAIN_IN_ALARM;
            }
            
            break;

        case MAIN_TO_CHANGE_USER_PASSWORD:
            switches = CheckKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
            if (switches == KNUMBER_FINISH)
            {
                SST_WriteCodeToMemory(1000, position);
                Usart1Send("User Password changed\r\n");
                BuzzerCommands(BUZZER_SHORT_CMD, 7);
                main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            }

            if (switches == KCANCEL)
            {
                main_state = MAIN_TO_MAIN_CANCEL;
            }

            if (!wait_for_code_timeout)
                main_state = MAIN_TO_MAIN_TIMEOUT;

            break;

        case MAIN_IN_ALARM:
#ifndef PROGRAMA_CHICKEN_BUCLE            
            //TODO: version anterior que funciona
            // check if we get here from sms or control
            if (CheckSMS())
            {
                result = FuncAlarm(1);    //sms activation only needs one kick
                ResetSMS();
            }
            else
                result = FuncAlarm(0);
            
            if (result == END_OK)
            {
                main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            }
            //FIN: version anterior que funciona
#endif
            
#ifdef PROGRAMA_CHICKEN_BUCLE
            result = FuncAlarmChickenBucle(0);
            
            if (result == END_OK)
            {
                main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            }            
#endif

            //TODO: nueva version FuncAlarm
            // no funciona activacion con botones B2 al B4
            // activa con B1 pero nunca sale de sirena, no pasa audios ni termina
            // falta timer???
            // if (CheckSMS())
            // {
            //     //llegue a la alarma por SMS, doy el primer kick
            //     result = Func_Alarm_SM(SMS_ALARM, 0, 0);
            //     ResetSMS();
            // }
            // else
            // {
            //     //Reviso si hay nuevo control, si no estoy pasando audio
            //     unsigned char button = 0;
            //     unsigned int code = 0;
            //     unsigned short code_position = 0;
                
            //     if (audio_state == AUDIO_INIT)
            //         button = CheckForButtons(&code_position, &code);
                
            //     if (button)
            //     {
            //         new_global_position = code_position;
            //         new_global_button = button;
            //         result = Func_Alarm_SM(NEWCODE_ALARM, new_global_position, new_global_button);
            //     }
            //     else
            //     {
            //         result = Func_Alarm_SM(CONTROL_ALARM, new_global_position, new_global_button);
            //     }
            // }

            // if (result == resp_ok)
            // {
            //     main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            // }            
            //FIN: nueva version FuncAlarm
            break;

        case MAIN_TO_SAVE_AT_LAST:

            //me quedo esperando un c�digo de control valido para guardarlo en posi
            mass_erase_position = position;
            switches = CheckRemoteKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);

            //reviso primero todas las posibilidades del teclado remoto
            if (remote_is_working)
            {
                if (switches == RK_CANCEL)
                    main_state = MAIN_TO_MAIN_CANCEL;

                //se eligio borrar una posicion de control desde el teclado remoto
                //xxx# y 0#
                else if ((switches == RK_NUMBER_FINISH) && (position == 0))	//termino el numero y era 0
                {
                    //se va a borrar la posicion con teclado remoto
                    position = mass_erase_position;	//update de posicion del control
                    main_state = MAIN_TO_DEL_CODE;
                }

                //se eligio entrar en grabado de controles con secuencia desde el teclado remoto
                //dos veces el mismo codigo de grabado
                else if ((switches == RK_NUMBER_FINISH) && (position == mass_erase_position))
                {
                    sprintf(str, "Grabar en secuencia remota desde: %d\r\n", position);
                    Usart1Send(str);
                    Display_ShowNumbers(DISPLAY_S);
                    SirenCommands(SIREN_CONFIRM_OK_CMD);
                    seq_number = 0;                
                    main_state = MAIN_TO_SAVE_IN_SEQUENCE;                
                }
            }

            //ahora podria ser un control
            if (switches == RK_MUST_BE_CONTROL)
            {
                code = code0;
                code <<= 16;
                code |= code1;
                code_position = CheckCodeInMemory(code);
                if (code_position == 0xFFFF)
                {
                    //el codigo no se habia utilizado
                    if (Write_Code_To_Memory(position, code) != 0)
                        Usart1Send((char *) "Codigo Guardado OK\r\n");
                    else
                        Usart1Send((char *) "Error al guardar\r\n");

                    Display_ConvertPosition(position);
                    BuzzerCommands(BUZZER_SHORT_CMD, 7);
                    if (remote_is_working)
                        SirenCommands(SIREN_CONFIRM_OK_CMD);                    

                }
                else
                {
                    //se habia utilizado en otra posicion
                    sprintf(str, "Error codigo ya esta en: %03d\r\n", code_position);
                    Usart1Send(str);

                    Display_ConvertPosition(code_position);
                    BuzzerCommands(BUZZER_HALF_CMD, 2);
                    if (remote_is_working)
                        SirenCommands(SIREN_HALF_CMD);
                    
                }
                main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            }

            if (!remote_is_working)
            {
                //Backup de la posicion del control que me trajo hasta aca
                mass_erase_position = position;
                switches = CheckKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
                if (switches == KCANCEL)
                    main_state = MAIN_TO_MAIN_CANCEL;

                //se eligio borrar una posicion de control
                //xxx# y 0#
                if ((switches == KNUMBER_FINISH) && (position == 0))
                {
                    position = mass_erase_position;	//update de posicion del control
                    main_state = MAIN_TO_DEL_CODE;
                    switches = KNONE;
                }

                //se eligio borrar todos los codigos de memoria (BLANQUEO COMPLETO)
                //000# y luego 999#
                if ((switches == KNUMBER_FINISH) && (position == 999) && (mass_erase_position == 0))
                {
                    Usart1Send((char *) "\r\n- CUIDADO entrando en Blanqueo Completo -\r\n");
                    main_state = MAIN_TO_MASS_ERASE_AT_LAST;
                    switches = KNONE;
                }

                //se eligio entrar en grabado de controles con secuencia
                //dos veces el mismo codigo de grabado
                if ((switches == KNUMBER_FINISH) && (position == mass_erase_position))
                {
                    sprintf(str, "Grabar en secuencia desde: %d\r\n", position);
                    Usart1Send(str);
                    Display_ShowNumbers(DISPLAY_S);
                    seq_number = 0;                
                    main_state = MAIN_TO_SAVE_IN_SEQUENCE;
                    switches = KNONE;
                }
            }

            if (!wait_for_code_timeout)
                main_state = MAIN_TO_MAIN_TIMEOUT;

            break;

        case MAIN_TO_DEL_CODE:
            if (Write_Code_To_Memory(position, 0xFFFFFFFF) != 0)
            {
                Usart1Send((char *) "Codigo Borrado OK\r\n");
                Display_VectorToStr("0.");
                BuzzerCommands(BUZZER_SHORT_CMD, 7);
                if (remote_is_working)
                    SirenCommands(SIREN_CONFIRM_OK_CMD);                    
            }
            else
            {
                Usart1Send((char *) "Error al borrar\r\n");
                Display_ShowNumbers(DISPLAY_NONE);
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                if (remote_is_working)
                    SirenCommands(SIREN_HALF_CMD);                    
            }

            main_state = MAIN_TO_MAIN_OK;
            break;

        case MAIN_TO_MASS_ERASE_AT_LAST:
            //se va a borrar la memoria
            if (EraseAllMemory() != 0)
            {
                Usart1Send((char *) "Memoria Completa Borrada OK\r\n");
                Display_VectorToStr("0.");
                BuzzerCommands(BUZZER_SHORT_CMD, 7);
                main_state = MAIN_TO_MAIN_WAIT_5SEGS;
            }
            else
            {
                Usart1Send((char *) "Error al borrar memoria\r\n");
                Display_ShowNumbers(DISPLAY_NONE);
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                main_state = MAIN_TO_MAIN_CANCEL;
            }
            break;

        case MAIN_TO_SAVE_IN_SEQUENCE:
            //me quedo esperando un c�digo de control valido para guardarlo en posi
            switches = CheckRemoteKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
            if (switches == RK_MUST_BE_CONTROL)
            {
                code = code0;
                code <<= 16;
                code |= code1;
                code_position = CheckCodeInMemory(code);
                if (code_position == 0xFFFF)
                {
                    //el codigo no se habia utilizado
                    if (Write_Code_To_Memory((position + seq_number), code) != 0)
                    {
                        sprintf(str, "Codigo Guardado OK en: %d\r\n", (position + seq_number));
                        Usart1Send(str);
                        Display_ConvertPosition((position + seq_number));
                        BuzzerCommands(BUZZER_SHORT_CMD, 7);                        
                        seq_number++;
                        main_state = MAIN_TO_SAVE_IN_SEQUENCE_WAITING;
                        if (remote_is_working)
                            SirenCommands(SIREN_CONFIRM_OK_CMD);                    
                        
                    }
                    else
                    {
                        Usart1Send((char *) "Error al guardar, problemas de memoria??\r\n");
                        //salgo por error
                        Display_VectorToStr("e");
                        main_state = MAIN_TO_MAIN_WAIT_5SEGS;                        
                    }
                }
                else
                {
                    //se habia utilizado en otra posicion
                    sprintf(str, "Error codigo ya esta en: %03d\r\n", code_position);
                    Usart1Send(str);
                    Display_ConvertPosition(code_position);
                    BuzzerCommands(BUZZER_HALF_CMD, 2);
                    //salgo por error
                    main_state = MAIN_TO_MAIN_WAIT_5SEGS;
                    if (remote_is_working)
                        SirenCommands(SIREN_HALF_CMD);
                    
                }
            }

            if (!remote_is_working)
            {
                switches = CheckKeypad(&switches_posi0, &switches_posi1, &switches_posi2, &position);
                if (switches == KCANCEL)
                {
                    Usart1Send((char *) "Termino de grabar en secuencia\r\n");
                    main_state = MAIN_TO_MAIN_CANCEL;
                }
            }
            else if (switches == RK_CANCEL)
            {
                SirenCommands(SIREN_CONFIRM_OK_CMD);
                Usart1Send((char *) "Termino de grabar remotos en secuencia\r\n");
                main_state = MAIN_TO_MAIN_CANCEL;                
            }

            //si no estoy en enviado la secuencia de numeros pongo la S
            if (Display_IsFree())
                Display_ShowNumbers(DISPLAY_S);
            
            break;
            
        case MAIN_TO_SAVE_IN_SEQUENCE_WAITING:
            if (Display_IsFree())
                main_state = MAIN_TO_SAVE_IN_SEQUENCE;
            
            break;
            
        case MAIN_TO_MAIN_CANCEL:
            Usart1Send((char *) "Opereta cancelada\r\n");
            main_state = MAIN_INIT;
            interdigit_timeout = 300;	//espero que se limpien las teclas
            remote_is_working = 0;
            break;

        case MAIN_TO_MAIN_TIMEOUT:
            Usart1Send((char *) "Timeout\r\n");
            main_state = MAIN_INIT;
            interdigit_timeout = 300;	//espero que se limpien las teclas
            remote_is_working = 0;            
            break;

        case MAIN_TO_MAIN_WAIT_5SEGS:
            interdigit_timeout = ACT_DESACT_IN_MSECS;	//espero 5 segundos luego del codigo grabado OK
            main_state = MAIN_TO_MAIN_WAIT_5SEGSA;
            remote_is_working = 0;            
            break;

        case MAIN_TO_MAIN_WAIT_5SEGSA:
            if (!interdigit_timeout)
                main_state = MAIN_TO_MAIN_OK;
            break;

        case MAIN_TO_MAIN_OK:
            remote_is_working = 0;
            main_state = MAIN_INIT;
            break;

        // case MAIN_MEMORY_DUMP:
        //     //cargo 256 numeros al vector de 1024 posiciones
        //     SST_MemoryDump (OFFSET_CODES_256);
        //     main_state = MAIN_INIT;
        //     break;

        // case MAIN_MEMORY_DUMP2:
        //     //cargo 256 numeros al vector de 1024 posiciones
        //     SST_MemoryDump (OFFSET_CODES_512);
        //     main_state = MAIN_INIT;
        //     break;

        default:
            main_state = MAIN_INIT;
            break;

        }

        UpdateBuzzer();
        Display_UpdateSM();
        UpdateSiren();
        UpdateAudio();
#ifdef CON_MODIFICACION_DIODO_BATERIA
        UpdateBattery();
#endif
        //OJO esta rutina puede cambiar el estado del programa principal
        main_state = UpdateUart(main_state);
    }

    return 0;
    
}
//--- End of Main ---//

//funcion de alarmas, revisa codigo en memoria y actua en consecuencia
unsigned char FuncAlarm (unsigned char sms_alarm)
{
    unsigned char button;
    unsigned int code;
    unsigned short code_position;

    char str[50];

    switch (alarm_state)
    {
    case ALARM_START:
        alarm_state = ALARM_NO_CODE;
        code = code0;
        code <<= 16;
        code |= code1;

        if (sms_alarm)
        {
            Usart1Send("SMS Activo: 911 B1\r\n");
            last_one_or_three = 911;
            alarm_state = ALARM_BUTTON1;
            repetition_counter = param_struct.b1r;
            button_timer_secs = ACT_DESACT_IN_SECS;	//2 segundos OK y buena distancia 20-5-15
#ifdef USE_F12_PLUS_WITH_SM
            //modificacion 24-01-2019 F12PLUS espera 10 segundos y se activa 5 segundos
            F12_State_Machine_Start();
#endif
#ifdef SOFTWARE_VERSION_2_5
	    button4_only5mins_timeout = BUTTON4_5MINS_TT;
#endif            
        }
        else
        {
            //code_position = CheckCodeInMemory(code);
            code_position = CheckBaseCodeInMemory(code);

            if ((code_position >= 0) && (code_position <= 1023))
            {
                sprintf(str, (char *) "Activo: %03d ", code_position);
                //el codigo existe en memoria
                //reviso el boton
                button = SST_CheckButtonInCode(code);
                if (button == 1)
                {
                    last_one_or_three = code_position;
                    alarm_state = ALARM_BUTTON1;
                    strcat(str, (char *) "B1\r\n");
                    repetition_counter = param_struct.b1r;
#ifdef USE_F12_PLUS_WITH_SM
                    //modificacion 24-01-2019 F12PLUS espera 10 segundos y se activa 5 segundos
                    F12_State_Machine_Start();
#endif
#ifdef SOFTWARE_VERSION_2_5
		    button4_only5mins_timeout = BUTTON4_5MINS_TT;
#endif            

                }
                else if (button == 2)
                {
                    //original boton 2
                    last_two = code_position;
                    alarm_state = ALARM_BUTTON2;
                    strcat(str, (char *) "B2\r\n");
                }
                else if (button == 3)
                {
                    last_one_or_three = code_position;
                    alarm_state = ALARM_BUTTON3;
                    strcat(str, (char *) "B3\r\n");
                    repetition_counter = param_struct.b3r;
#ifdef SOFTWARE_VERSION_2_5
		    button4_only5mins_timeout = BUTTON4_5MINS_TT;
#endif            		    
                }
                else if (button == 4)
                {
                    alarm_state = ALARM_BUTTON4;
                    strcat(str, (char *) "B4\r\n");
                    repetition_counter = param_struct.b4r;
                }

                //button_timer_secs = 5;	//5 segundos suena seguro EVITA PROBLEMAS EN LA VUELTA
                button_timer_secs = ACT_DESACT_IN_SECS;	//2 segundos OK y buena distancia 20-5-15

                Usart1Send(str);
            }
        }
        break;

    case ALARM_BUTTON1:
        FPLUS_ON;
        F5PLUS_ON;
#ifdef USE_F12_PLUS_ON_BUTTON1
        F12PLUS_ON;
#endif

        SirenCommands(SIREN_MULTIPLE_UP_CMD);
        alarm_state++;
        break;

    case ALARM_BUTTON1_A:
        if (!button_timer_secs)
        {
            button_timer_secs = param_struct.b1t;
            alarm_state++;
        }
        break;

    case ALARM_BUTTON1_B:
        //me quedo esperando que apaguen o timer
#ifdef PROGRAMA_NORMAL
        if (CheckForButtons(&code_position, &code) == 1)	//reviso solo boton 1
        {
            alarm_state = ALARM_BUTTON1_FINISH;
        }
#endif

#ifdef PROGRAMA_DE_BUCLE
        if (CheckForButtons(&code_position, &code) == 4)	//desactivo solo con 4
        {
            alarm_state = ALARM_BUTTON1_FINISH;
        }
#endif
        
        if (!button_timer_secs)
        {
            //tengo timeout, avanzo al audio
            alarm_state++;
        }
        break;

    case ALARM_BUTTON1_C:
        //paso el audio y descuento un ciclo
        if (repetition_counter > 1)
        {
            repetition_counter--;
            SirenCommands(SIREN_STOP_CMD);

            if (param_struct.audio_buttons & B1_AUDIO_MASK)
                VectorToSpeak('a');
        
            PositionToSpeak(last_one_or_three);
            alarm_state++;
        }
        else
        {
            Usart1Send((char*) "Timeout B1 ");
            alarm_state = ALARM_BUTTON1_FINISH;
        }
        break;

    case ALARM_BUTTON1_D:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            alarm_state = ALARM_BUTTON1;
        }
        break;

    case ALARM_BUTTON1_FINISH:
        sprintf(str, "Desactivo: %03d\r\n", last_one_or_three);
        Usart1Send(str);

        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        F5PLUS_OFF;
#ifdef USE_F12_PLUS_WITH_SM
        F12_State_Machine_Reset();
#else
        F12PLUS_OFF;
#endif

        PositionToSpeak(last_one_or_three);
        alarm_state++;
        break;

    case ALARM_BUTTON1_FINISH_B:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            return END_OK;
        }
        break;

    case ALARM_BUTTON2:		//solo enciendo reflectores
        FPLUS_ON;

        alarm_state++;
#ifdef HT6P20B2_FOURTH_BUTTON
        button_timer_secs = 4;	//4 segundos overdrive
#endif
        break;

    case ALARM_BUTTON2_A:		//espero los primeros 2 segundos
#ifdef HT6P20B2_FOURTH_BUTTON
        if (!button_timer_secs)
        {
            button_timer_secs = 2;
            alarm_state++;
        }
#else
        alarm_state++;
#endif
        break;

    case ALARM_BUTTON2_B:
#ifdef HT6P20B2_FOURTH_BUTTON
        if (CheckForButtons(&code_position, &code) == 2)	//reviso solo boton 2 para simular el 4
        {
            if (code_position == last_two)
                alarm_state = ALARM_BUTTON2_D;
        }
#endif
        if (!button_timer_secs)
        {
            button_timer_secs = param_struct.b2t;
            alarm_state++;
        }
        break;

    case ALARM_BUTTON2_C:
        //me quedo esperando que apaguen o timer

        button = CheckForButtons(&code_position, &code);
#ifdef PROGRAMA_NORMAL
        if (button == 2)	//reviso solo boton 2
        {
            sprintf(str, "Desactivo: %03d\r\n", code_position);
            Usart1Send(str);
            alarm_state = ALARM_BUTTON2_FINISH;
        }

        //reviso el boton1 o sms
        if ((button == 1) || (sms_alarm))
        {
            if (sms_alarm)
            {
                strcpy(str, "SMS Activo: 911 B1\r\n");
                last_one_or_three = 911;
            }
            else
            {
                sprintf(str, "Activo: %03d B1\r\n", code_position);
                last_one_or_three = code_position;
            }
            
            Usart1Send(str);
            repetition_counter = param_struct.b1r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON1;
        }

        if (button == 3)		//reviso el boton
        {
            sprintf(str, "Activo: %03d B3\r\n", code_position);
            Usart1Send(str);

            last_one_or_three = code_position;
            repetition_counter = param_struct.b3r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON3;
        }
#endif
#ifdef PROGRAMA_DE_BUCLE
        if (button == 1)		//reviso el boton
        {
            sprintf(str, "Activo: %03d B1\r\n", code_position);
            Usart1Send(str);

            last_one_or_three = code_position;
            repetition_counter = param_struct.b1r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON1;
        }

        if (button == 3)		//reviso el boton
        {
            sprintf(str, "Activo: %03d B3\r\n", code_position);
            Usart1Send(str);

            last_one_or_three = code_position;
            repetition_counter = param_struct.b3r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON3;
        }

        if (button == 4)	//reviso boton 4 para desactivar
        {
            sprintf(str, "Desactivo: %03d\r\n", code_position);
            Usart1Send(str);
            alarm_state = ALARM_BUTTON2_FINISH;
        }
#endif

        if (!button_timer_secs)
        {
            //tengo timeout, apago reflectores
            Usart1Send((char*) "Timeout B2\r\n");
            alarm_state = ALARM_BUTTON2_FINISH;
        }
        break;

    case ALARM_BUTTON2_D:
        SirenCommands(SIREN_STOP_CMD);
        PositionToSpeak(last_one_or_three);
        alarm_state++;
        break;

    case ALARM_BUTTON2_E:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            alarm_state++;
        }
        break;

    case ALARM_BUTTON2_FINISH:
        FPLUS_OFF;
        return END_OK;
        break;

    case ALARM_BUTTON3:
        FPLUS_ON;
//			F5PLUS_ON;
//			F12PLUS_ON;
        //SirenCommands(SIREN_SINGLE_CMD);
        SirenCommands(SIREN_SINGLE_CHOPP_CMD);
        //button_timer_secs = 5;	//5 segundos suena seguro EVITA PROBLEMAS EN LA VUELTA
        alarm_state++;
        break;

    case ALARM_BUTTON3_A:
        if (!button_timer_secs)
        {
            button_timer_secs = param_struct.b3t;
            alarm_state++;
        }
        break;

    case ALARM_BUTTON3_B:
        //me quedo esperando que apaguen o timer

        button = CheckForButtons(&code_position, &code);
#ifdef PROGRAMA_NORMAL
        if (button == 3)	//reviso solo boton 3
        {
            sprintf(str, "Desactivo: %03d\r\n", code_position);
            Usart1Send(str);
            alarm_state = ALARM_BUTTON3_FINISH;
        }

        //reviso el boton1 o sms
        if ((button == 1) || (sms_alarm))
        {
            if (sms_alarm)
            {
                strcpy(str, "SMS Activo: 911 B1\r\n");
                last_one_or_three = 911;
            }
            else
            {
                sprintf(str, "Activo: %03d B1\r\n", code_position);
                last_one_or_three = code_position;
            }
        
            Usart1Send(str);
            repetition_counter = param_struct.b1r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON1;
        }
#endif

#ifdef PROGRAMA_DE_BUCLE
        if (button == 1)		//reviso el boton
        {
            sprintf(str, "Activo: %03d B1\r\n", code_position);
            Usart1Send(str);

            last_one_or_three = code_position;
            repetition_counter = param_struct.b1r;
            button_timer_secs = ACT_DESACT_IN_SECS;

            alarm_state = ALARM_BUTTON1;
        }

        if (button == 4)	//reviso solo boton 4
        {
            sprintf(str, "Desactivo: %03d\r\n", code_position);
            Usart1Send(str);
            alarm_state = ALARM_BUTTON3_FINISH;
        }
#endif

        if (!button_timer_secs)
        {
            //tengo timeout, avanzo al audio

            alarm_state++;
        }
        break;

    case ALARM_BUTTON3_C:
        // descuento un ciclo y paso el audio
        if (repetition_counter > 1)
        {
            repetition_counter--;
            SirenCommands(SIREN_STOP_CMD);

            if (param_struct.audio_buttons & B3_AUDIO_MASK)
                VectorToSpeak('b');
        
            PositionToSpeak(last_one_or_three);
            alarm_state++;
        }
        else
        {
            Usart1Send((char*) "Timeout B3 ");
            alarm_state = ALARM_BUTTON3_FINISH;
        }
        break;

    case ALARM_BUTTON3_D:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            alarm_state = ALARM_BUTTON3;
        }
        break;

    case ALARM_BUTTON3_FINISH:
        sprintf(str, "Desactivo: %03d\r\n", last_one_or_three);
        Usart1Send(str);

        SirenCommands(SIREN_STOP_CMD);
//			F12PLUS_OFF;
        FPLUS_OFF;

        PositionToSpeak(last_one_or_three);
        alarm_state++;
        break;

    case ALARM_BUTTON3_FINISH_B:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            return END_OK;
        }
        break;

    case ALARM_BUTTON4:

#ifdef SOFTWARE_VERSION_2_5
	SirenCommands(SIREN_STOP_CMD);

	if (button4_only5mins_timeout)
	    PositionToSpeak(last_one_or_three);
	
	alarm_state++;
#else
        SirenCommands(SIREN_STOP_CMD);
	PositionToSpeak(last_one_or_three);
	alarm_state++;
#endif
        break;

    case ALARM_BUTTON4_A:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            alarm_state++;
        }
        break;

    case ALARM_BUTTON4_FINISH:
        return END_OK;
        break;

    case ALARM_NO_CODE:
        return END_OK;
        break;
    default:
        alarm_state = 0;
        break;
    }

#ifdef USE_F12_PLUS_WITH_SM
    F12_State_Machine();
#endif

    return WORKING;
}


#ifdef PROGRAMA_CHICKEN_BUCLE
//funcion de alarmas, revisa codigo en memoria y actua en consecuencia
unsigned char FuncAlarmChickenBucle (unsigned char sms_alarm)
{
    unsigned char button;
    unsigned int code;
    unsigned short code_position;

    char str[50];

    switch (alarm_state)
    {
    case ALARM_START:
        alarm_state = ALARM_NO_CODE;
        code = code0;
        code <<= 16;
        code |= code1;

        if (sms_alarm)
        {
            Usart1Send("SMS Not supported\r\n");
        }
        else
        {
            //code_position = CheckCodeInMemory(code);
            code_position = CheckBaseCodeInMemory(code);

            if ((code_position >= 0) && (code_position <= 1023))
            {
                sprintf(str, (char *) "Activo: %03d ", code_position);
                //el codigo existe en memoria
                //reviso el boton
                button = SST_CheckButtonInCode(code);
                if (button == 1)
                {
                    last_one_or_three = code_position;
                    alarm_state = ALARM_BUTTON1;
                    strcat(str, (char *) "B1\r\n");
                    repetition_counter = param_struct.b1r;
#ifdef USE_F12_PLUS_WITH_SM
                    //modificacion 24-01-2019 F12PLUS espera 10 segundos y se activa 5 segundos
                    F12_State_Machine_Start();
#endif

                }
                else if (button == 2)
                {
                    //original boton 2
                    last_two = code_position;
                    alarm_state = ALARM_BUTTON2;
                    strcat(str, (char *) "B2\r\n");
                    repetition_counter = param_struct.b2r;                    
                }
                else if (button == 3)
                {
                    last_one_or_three = code_position;
                    alarm_state = ALARM_BUTTON3;
                    strcat(str, (char *) "B3\r\n");
                    repetition_counter = param_struct.b3r;
                }
                else if (button == 4)
                {
                    alarm_state = ALARM_BUTTON4;
                    strcat(str, (char *) "B4\r\n");
                    repetition_counter = param_struct.b4r;
                }

                //button_timer_secs = 5;	//5 segundos suena seguro EVITA PROBLEMAS EN LA VUELTA
                button_timer_secs = ACT_DESACT_IN_SECS;	//2 segundos OK y buena distancia 20-5-15

                Usart1Send(str);
            }
        }
        break;

    case ALARM_BUTTON1:
        FPLUS_ON;
        F5PLUS_ON;
#ifdef USE_F12_PLUS_ON_BUTTON1
        F12PLUS_ON;
#endif

        SirenCommands(SIREN_MULTIPLE_UP_CMD);
        last_two = 0;
        alarm_state++;
        break;

    case ALARM_BUTTON1_A:
        if (!button_timer_secs)
        {
            button_timer_secs = 90;
            alarm_state++;
        }
        break;

    case ALARM_BUTTON1_B:
        //me quedo esperando que apaguen o timer
        if (CheckForButtons(&code_position, &code) == 4)	//desactivo solo con 4
        {
            alarm_state = ALARM_BUTTON1_D;
        }
        
        if (!button_timer_secs)
        {
            //tengo timeout, corto
            alarm_state++;
        }
        break;

    case ALARM_BUTTON1_C:
        Usart1Send((char*) "Timeout B1 ");

        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        F5PLUS_OFF;
#ifdef USE_F12_PLUS_WITH_SM
        F12_State_Machine_Reset();
#else
        F12PLUS_OFF;
#endif

        PositionToSpeak(last_one_or_three);
        alarm_state = ALARM_BUTTON1_FINISH;
        break;

    case ALARM_BUTTON1_D:
        sprintf(str, "Desactivo: %03d\r\n", last_one_or_three);
        Usart1Send(str);

        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        F5PLUS_OFF;
#ifdef USE_F12_PLUS_WITH_SM
        F12_State_Machine_Reset();
#else
        F12PLUS_OFF;
#endif

        PositionToSpeak(last_one_or_three);
        alarm_state++;
        break;

    case ALARM_BUTTON1_FINISH:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            return END_OK;
        }        
        break;

    case ALARM_BUTTON1_FINISH_B:
        break;

    case ALARM_BUTTON2:		//solo enciendo reflectores y sirena
        FPLUS_ON;
        // SirenCommands(SIREN_MULTIPLE_DOWN_CMD);
        SirenCommands(SIREN_SINGLE_CHOPP_CMD);        
        last_one_or_three = 0;
        alarm_state++;
        break;

    case ALARM_BUTTON2_A:		//espero los primeros 2 segundos
        if (!button_timer_secs)
        {
            // button_timer_secs = param_struct.b2t;
            button_timer_secs = 90;    //1.5 min         
            alarm_state++;
        }
        break;

    case ALARM_BUTTON2_B:
        //me quedo esperando que apaguen o timer
        if (CheckForButtons(&code_position, &code) == 4)	//desactivo solo con 4
        {
            alarm_state = ALARM_BUTTON2_D;
        }
        
        if (!button_timer_secs)
        {
            //tengo timeout, corto
            alarm_state++;
        }
        break;

    case ALARM_BUTTON2_C:
        Usart1Send((char*) "Timeout B2 ");
        
        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        PositionToSpeak(last_two);
        alarm_state = ALARM_BUTTON2_FINISH;
        break;

    case ALARM_BUTTON2_D:
        sprintf(str, "Desactivo: %03d\r\n", last_two);
        Usart1Send(str);

        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        PositionToSpeak(last_two);
        alarm_state++;
        break;

    case ALARM_BUTTON2_E:
        alarm_state++;
        break;

    case ALARM_BUTTON2_FINISH:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            return END_OK;
        }
        break;

    case ALARM_BUTTON3:
        FPLUS_ON;
        SirenCommands(SIREN_SINGLE_CHOPP_SMALL_CMD);
        last_two = 0;        
        alarm_state++;
        break;

    case ALARM_BUTTON3_A:
        if (!button_timer_secs)
        {
            button_timer_secs = 90;
            alarm_state++;
        }
        break;

    case ALARM_BUTTON3_B:
        //me quedo esperando que apaguen o timer
        if (CheckForButtons(&code_position, &code) == 4)	//desactivo solo con 4
        {
            alarm_state = ALARM_BUTTON3_D;
        }
        
        if (!button_timer_secs)
        {
            //tengo timeout, corto
            alarm_state++;
        }
        break;

    case ALARM_BUTTON3_C:
        Usart1Send((char*) "Timeout B3 ");
        
            // alarm_state = ALARM_BUTTON3_FINISH;
        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        PositionToSpeak(last_one_or_three);
        alarm_state = ALARM_BUTTON3_FINISH;
        break;

    case ALARM_BUTTON3_D:
        sprintf(str, "Desactivo: %03d\r\n", last_one_or_three);
        Usart1Send(str);

        SirenCommands(SIREN_STOP_CMD);
        FPLUS_OFF;
        PositionToSpeak(last_one_or_three);
        alarm_state++;
        break;

    case ALARM_BUTTON3_FINISH:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            return END_OK;
        }
        break;

    case ALARM_BUTTON3_FINISH_B:
        break;

    case ALARM_BUTTON4:
        SirenCommands(SIREN_STOP_CMD);
        if (last_one_or_three)
            PositionToSpeak(last_one_or_three);
        else if (last_two)
            PositionToSpeak(last_two);
        
        alarm_state++;
        break;

    case ALARM_BUTTON4_A:
        if (audio_state == AUDIO_INIT)
        {
            //termino de enviar audio
            alarm_state++;
        }
        break;

    case ALARM_BUTTON4_FINISH:
        return END_OK;
        break;

    case ALARM_NO_CODE:
        return END_OK;
        break;
    default:
        alarm_state = 0;
        break;
    }

#ifdef USE_F12_PLUS_WITH_SM
    F12_State_Machine();
#endif

    return WORKING;
}
#endif    //PROGRAMA_CHICKEN_BUCLE


#ifdef USE_F12_PLUS_WITH_SM
#ifdef USE_F12_PLUS_SM_DIRECT
#define F12PLUS_ACTIVE    F12PLUS_ON
#define F12PLUS_INACTIVE    F12PLUS_OFF
#endif
#ifdef USE_F12_PLUS_SM_NEGATE
#define F12PLUS_ACTIVE    F12PLUS_OFF
#define F12PLUS_INACTIVE    F12PLUS_ON
#endif

typedef enum {
    F12_PLUS_WAITING = 0,    
    F12_PLUS_ACTIVE,
    F12_PLUS_DONE

} f12_plus_state_t;
f12_plus_state_t f12_plus_state;

void F12_State_Machine_Reset (void)
{
    f12_plus_state = F12_PLUS_DONE;
    F12PLUS_INACTIVE;
}

void F12_State_Machine_Start (void)
{
    f12_plus_state = F12_PLUS_WAITING;
    f12_plus_timer = 10000;
}

void F12_State_Machine (void)
{
    switch (f12_plus_state)
    {
    case F12_PLUS_WAITING:
        if (!f12_plus_timer)
        {
            F12PLUS_ACTIVE;
            f12_plus_timer = 5000;
            f12_plus_state = F12_PLUS_ACTIVE;                
        }
        break;

    case F12_PLUS_ACTIVE:
        if (!f12_plus_timer)
        {
            F12PLUS_INACTIVE;
            f12_plus_state = F12_PLUS_DONE;
        }
        break;

    case F12_PLUS_DONE:
        break;
    }
}

#endif    //USE_F12PLUS_WITH_SM

unsigned char CheckRemoteKeypad (unsigned char * sp0, unsigned char * sp1, unsigned char * sp2, unsigned short * posi)
{
    unsigned char button_remote = 0;

    switch (remote_keypad_state)
    {
    case RK_NONE:
        //me quedo esperando un c�digo de control
        if (RxCode() == ENDED_OK)
        {
            //reviso aca si es de remote keypad o control
            //si es control contesto MUST_BE_CONTROL

            //en code0 y code1 tengo lo recibido
            button_remote = CheckButtonRemote(code0, code1);

            if (button_remote != REM_NO)
            {
                //se cancelo la operacion
                if (button_remote == REM_B10)
                {
                    Display_ShowNumbers(DISPLAY_NONE);
                    if (unlock_by_remote)
                        SirenCommands(SIREN_HALF_CMD);
                    BuzzerCommands(BUZZER_HALF_CMD, 1);
                    remote_keypad_state = RK_CANCEL;
                }
                else if (((button_remote > REM_NO) && (button_remote < REM_B10)) || (button_remote == REM_B11))	//es un numero 1 a 9 o 0
                {
                    //se presiono un numero - reviso si fue 0
                    if (button_remote == REM_B11)
                    {
                        Display_ShowNumbers(DISPLAY_ZERO);
                        *sp0 = 0;
                    }
                    else
                    {
                        Display_ShowNumbers(button_remote);
                        *sp0 = button_remote;
                    }
                    if (unlock_by_remote)
                        SirenCommands(SIREN_SHORT_CMD);
                    BuzzerCommands(BUZZER_SHORT_CMD, 1);

                    *sp1 = 0;
                    *sp2 = 0;
                    remote_keypad_state = RK_RECEIVING_A;
                    interdigit_timeout = 1000;
                }
            }
            else
                remote_keypad_state = RK_MUST_BE_CONTROL;
        }
        break;

    case RK_RECEIVING_A:
        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_RECEIVING_B;
            interdigit_timeout = param_struct.interdigit;
        }
        break;

    case RK_RECEIVING_B:			//segundo digito o confirmacion del primero
        if (RxCode() == ENDED_OK)
        {
            //en code0 y code1 tengo lo recibido
            button_remote = CheckButtonRemote(code0, code1);

            //se cancelo la operacion
            if (button_remote == REM_B10)
            {
                Display_ShowNumbers(DISPLAY_NONE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_HALF_CMD);
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                remote_keypad_state = RK_CANCEL;
            }

            //si esta apurado un solo numero
            if (button_remote == REM_B12)
            {
                Display_ShowNumbers(DISPLAY_LINE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_SHORT_CMD);
                BuzzerCommands(BUZZER_SHORT_CMD, 2);
                *sp2 = *sp0;
                *sp1 = 0;
                *sp0 = 0;

                *posi = *sp2;
                remote_keypad_state = RK_NUMBER_FINISH;
            }
            //es un numero 1 a 9 o 0
            if (((button_remote > REM_NO) && (button_remote < REM_B10)) || (button_remote == REM_B11))
            {
                if (button_remote == REM_B11)
                {
                    *sp1 = 0;
                    Display_ShowNumbers(DISPLAY_ZERO);
                }
                else
                {
                    *sp1 = button_remote;
                    Display_ShowNumbers(button_remote);
                }

                if (unlock_by_remote)
                    SirenCommands(SIREN_SHORT_CMD);
                BuzzerCommands(BUZZER_SHORT_CMD, 1);
                *sp2 = 0;
                remote_keypad_state = RK_RECEIVING_C;
                interdigit_timeout = 1000;
            }
        }

        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_TIMEOUT;
        }

        break;

    case RK_RECEIVING_C:
        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_RECEIVING_D;
            interdigit_timeout = param_struct.interdigit;
        }
        break;

    case RK_RECEIVING_D:				//tercer digito o confirmacion del segundo
        if (RxCode() == ENDED_OK)
        {
            //en code0 y code1 tengo lo recibido
            button_remote = CheckButtonRemote(code0, code1);

            //se cancelo la operacion
            if (button_remote == REM_B10)
            {
                Display_ShowNumbers(DISPLAY_NONE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_HALF_CMD);
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                remote_keypad_state = RK_CANCEL;
            }

            //si esta apurado dos numeros
            if (button_remote == REM_B12)
            {
                Display_ShowNumbers(DISPLAY_LINE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_SHORT_CMD);
                BuzzerCommands(BUZZER_SHORT_CMD, 2);
                *sp2 = *sp0;
                *sp0 = 0;

                *posi = *sp2 * 10 + *sp1;
                remote_keypad_state = RK_NUMBER_FINISH;
            }

            if (((button_remote > REM_NO) && (button_remote < REM_B10)) || (button_remote == REM_B11))	//es un numero 1 a 9 o 0
            {
                if (button_remote == ZERO_KEY)
                {
                    *sp2 = 0;
                    Display_ShowNumbers(DISPLAY_ZERO);
                }
                else
                {
                    *sp2 = button_remote;
                    Display_ShowNumbers(button_remote);
                }
                if (unlock_by_remote)
                    SirenCommands(SIREN_SHORT_CMD);
                BuzzerCommands(BUZZER_SHORT_CMD, 1);
                remote_keypad_state = RK_RECEIVING_E;
                interdigit_timeout = 1000;
            }
        }

        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_TIMEOUT;
        }
        break;

    case RK_RECEIVING_E:
        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_RECEIVING_F;
            interdigit_timeout = param_struct.interdigit;
        }
        break;

    case RK_RECEIVING_F:
        if (RxCode() == ENDED_OK)
        {
            //en code0 y code1 tengo lo recibido
            button_remote = CheckButtonRemote(code0, code1);

            //se cancelo la operacion
            if (button_remote == REM_B10)
            {
                Display_ShowNumbers(DISPLAY_NONE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_HALF_CMD);
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                remote_keypad_state = RK_CANCEL;
            }

            //es la confirmacion
            if (button_remote == REM_B12)
            {
                Display_ShowNumbers(DISPLAY_LINE);
                if (unlock_by_remote)
                    SirenCommands(SIREN_SHORT_CMD);
                BuzzerCommands(BUZZER_SHORT_CMD, 2);
                *posi = *sp0 * 100 + *sp1 * 10 + *sp2;
                remote_keypad_state = RK_NUMBER_FINISH;
            }
        }

        if (!interdigit_timeout)
        {
            remote_keypad_state = RK_TIMEOUT;
        }
        break;

    case RK_MUST_BE_CONTROL:
    case RK_CANCEL:
    case RK_TIMEOUT:
    default:
        remote_keypad_state = RK_NONE;
        break;
    }
    return remote_keypad_state;
}


//caraga la posicion con VectorToSpeak
void PositionToSpeak(unsigned short posi)
{
	unsigned char a, b, c;

	a = posi / 100;

	if (a == 0)
		VectorToSpeak(10);
	else
		VectorToSpeak(a);

	b = (posi - a * 100) / 10;

	if (b == 0)
		VectorToSpeak(10);
	else
		VectorToSpeak(b);

	c = (posi - a * 100 - b * 10);

	if (c == 0)
		VectorToSpeak(10);
	else
		VectorToSpeak(c);

	VectorToSpeak(0);
}

//carga los numeros a decir en secuencia en un vector
//del 1 al 9; 10 es cero
void VectorToSpeak (unsigned char new_number)
{
	unsigned char i;
	//me fijo si hay espacio
	if (p_numbers_speak < &numbers_speak[LAST_NUMBER_SPEAK - 1])
	{
		//busco la primer posicion vacia y pongo el nuevo numero
		for (i = 0; i < LAST_NUMBER_SPEAK; i++)
		{
			if (numbers_speak[i] == 0)
			{
				numbers_speak[i] = new_number;
				i = LAST_NUMBER_SPEAK;
			}
		}
	}
}

//dice los numero en secuencia
//los numeros se cargan con VectorToSpeak()
void UpdateAudio (void)
{

    switch (audio_state)
    {
    case AUDIO_INIT:
        if (*p_numbers_speak != 0)		//ojo ver esto que no son char
            audio_state++;
        break;

    case AUDIO_UPDATE:
        //habilitar amplificador y pasarlo a audio
        //cargo el audio
        switch (*p_numbers_speak)
        {
        case 10:
            p_files_addr = &files.posi0;
            p_files_length = &files.length0;
            break;

        case 1:
            p_files_addr = &files.posi1;
            p_files_length = &files.length1;
            break;

        case 2:
            p_files_addr = &files.posi2;
            p_files_length = &files.length2;
            break;

        case 3:
            p_files_addr = &files.posi3;
            p_files_length = &files.length3;
            break;

        case 4:
            p_files_addr = &files.posi4;
            p_files_length = &files.length4;
            break;

        case 5:
            p_files_addr = &files.posi5;
            p_files_length = &files.length5;
            break;

        case 6:
            p_files_addr = &files.posi6;
            p_files_length = &files.length6;
            break;

        case 7:
            p_files_addr = &files.posi7;
            p_files_length = &files.length7;
            break;

        case 8:
            p_files_addr = &files.posi8;
            p_files_length = &files.length8;
            break;

        case 9:
            p_files_addr = &files.posi9;
            p_files_length = &files.length9;
            break;

        case 'a':
            p_files_addr = &files.posi10;
            p_files_length = &files.length10;            
            break;

        case 'b':
            p_files_addr = &files.posi11;
            p_files_length = &files.length11;            
            break;

	default:
	    // error in number or audio value
	    *p_numbers_speak = 0;
	    audio_state = AUDIO_INIT;
	    return;
	    break;
        }

        Load16SamplesShort((unsigned short *)v_samples1, *p_files_addr + FILE_OFFSET);
        Load16SamplesShort((unsigned short *)v_samples2, *p_files_addr + FILE_OFFSET + 32);
        current_size = 64;
        update_samples = 0;

        Power_Ampli_Ena ();
        Ampli_to_Audio ();
        Usart1Send((char *) "-> To Audio\r\n");
        audio_state++;
        break;

    case AUDIO_SENDING:
        if (update_samples)	//el update lo hace la interrupcion para la funcion seno
        {
            update_samples = 0;

            if (current_size < (*p_files_length - FILE_OFFSET))
            {
                //LED_ON;
                //DESDE ACA LEVANTO DE LA MEMORIA SST
                //cargo el buffer que no esta en uso
                if (buff_in_use == 1)
                {
                    //cargo el buffer 2
                    Load16SamplesShort((unsigned short *)v_samples2, *p_files_addr + FILE_OFFSET + current_size);
                }
                else if (buff_in_use == 2)
                {
                    //cargo el buffer 1
                    Load16SamplesShort((unsigned short *)v_samples1, *p_files_addr + FILE_OFFSET + current_size);
                }
                current_size += 32;
                //LED_OFF;
            }
            else
            {
                //termine de enviar avanzo para ver si hay mas numeros
                audio_state++;
            }
        }
        break;

    case AUDIO_CHECK_NEXT:
        p_numbers_speak++;
        if (*p_numbers_speak != '\0')		//ojo ver esto que no son char
            audio_state = AUDIO_UPDATE;
        else
            audio_state = AUDIO_FINISHING;

        break;

    case AUDIO_FINISHING:
        //llegue al final
        p_numbers_speak = numbers_speak;
        memset (numbers_speak, '\0', sizeof(numbers_speak));
        Power_Ampli_Disa ();
        Ampli_to_Sirena ();
        Usart1Send((char *) "-> To Sirena\r\n");
        audio_state = AUDIO_INIT;
        break;

    default:
        audio_state = AUDIO_INIT;
        break;
    }

}

#ifdef CON_MODIFICACION_DIODO_BATERIA
#define KEYPAD_WITH_AC    0
#define KEYPAD_WITH_BATT    1
volatile unsigned char timer_battery = 0;
unsigned short ac_counter = 0;
unsigned short bat_counter = 0;
unsigned char slow_toggle = 0;
unsigned char battery_state = KEYPAD_WITH_AC;

// with AC toggle led on 400ms
// without AC toggle led on 1200ms
void UpdateBattery (void)
{
    if (!timer_battery)
    {
        timer_battery = 1;
        //reviso la pata

        if (ac_counter < 400)
        {
            ac_counter++;
            if (AC_PIN)
                bat_counter++;
        }
        else
        {
            unsigned char current_batt_state;
            
            ac_counter = 0;
            //me fijo si hago toggle
            if (bat_counter > 370)
            {
                current_batt_state = KEYPAD_WITH_AC;
                //CON TENSION AC
                //titila rapido
                if (LED)
                    LED_OFF;
                else
                    LED_ON;
            }
            else
            {
                current_batt_state = KEYPAD_WITH_BATT;
                //SIN TENSION AC
                //titila lento
                if (!slow_toggle)
                {
                    slow_toggle = 2;
                    if (LED)
                        LED_OFF;
                    else
                        LED_ON;
                }
                else
                    slow_toggle--;

            }
            bat_counter = 0;

            // check batt state
            if ((battery_state == KEYPAD_WITH_AC) &&
                (current_batt_state == KEYPAD_WITH_BATT))
            {
                battery_state = KEYPAD_WITH_BATT;
                Usart1Send("\r\nKeypad on BATT\r\n");
            }
            else if ((battery_state == KEYPAD_WITH_BATT) &&
                     (current_batt_state == KEYPAD_WITH_AC))
            {
                battery_state = KEYPAD_WITH_AC;
                Usart1Send("\r\nKeypad with AC\r\n");
            }
        }
    }
}
#endif


void SirenCommands(unsigned char command)
{
	siren_state = command;
}

void UpdateSiren (void)
{
    switch (siren_state)
    {
    case SIREN_INIT:
        break;

    case SIREN_SINGLE:
        //cargo frecuencia y ciclo de trabajo
        //ChangeAmpli(FREQ_1000HZ, DUTY_50_1000);
        ChangeAmpli(FREQ_800HZ, DUTY_50_800);
        Power_Ampli_Ena();
        siren_state = SIREN_SINGLE_RINGING;
        break;

    case SIREN_SINGLE_RINGING:
        break;

    case SIREN_SINGLE_CHOPP_ON:
        if (!siren_timeout)
        {
            //cargo frecuencia y ciclo de trabajo
            //ChangeAmpli(FREQ_1000HZ, DUTY_50_1000);
            ChangeAmpli(FREQ_800HZ, DUTY_50_800);
            Power_Ampli_Ena();
            siren_state = SIREN_SINGLE_CHOPP_OFF;
            siren_timeout = 1500;					//2 seg
        }
        break;

    case SIREN_SINGLE_CHOPP_OFF:
        if (!siren_timeout)
        {
            //vuelvo y dejo 500ms apagado
            Power_Ampli_Disa();
            siren_state--;
            siren_timeout = 500;
        }
        break;

    case SIREN_SINGLE_CHOPP_SMALL_ON:
        if (!siren_timeout)
        {
            ChangeAmpli(FREQ_800HZ, DUTY_50_800);
            Power_Ampli_Ena();
            siren_state = SIREN_SINGLE_CHOPP_SMALL_OFF;
            siren_timeout = 500;
        }
        break;

    case SIREN_SINGLE_CHOPP_SMALL_OFF:
        if (!siren_timeout)
        {
            Power_Ampli_Disa();
            siren_state--;
            siren_timeout = 1500;
        }
        break;
                        
    case SIREN_MULTIPLE_UP:
        siren_steps = 0;
        siren_state = SIREN_MULTIPLE_UP_B;
        Power_Ampli_Ena();
        break;

    case SIREN_MULTIPLE_UP_A:
        if (!siren_timeout)
        {
            //se termino el tiempo, cambio la frecuencia
            if (siren_steps)
                siren_steps--;
            siren_state = SIREN_MULTIPLE_UP_B;
        }
        break;

    case SIREN_MULTIPLE_UP_B:
        if (siren_steps)
        {
            freq_us = freq_us + SIREN_FREQ_STEP;
            siren_timeout = SIREN_STEP_TIMEOUT;
            ChangeAmpli(freq_us, freq_us >> 1);
            siren_state = SIREN_MULTIPLE_UP_A;
        }
        else
        {
            //empiezo el ciclo de nuevo
            siren_steps = SIREN_STEP_RELOAD;
            freq_us = SIREN_FIRST_FREQ;
            ChangeAmpli(freq_us, freq_us >> 1);
            siren_state = SIREN_MULTIPLE_UP_A;
            siren_timeout = SIREN_STEP_TIMEOUT;
        }
        break;

    case SIREN_CONFIRM_OK:
        siren_steps = 7;
        siren_state = SIREN_CONFIRM_OK_B;
        siren_timeout = SIREN_SHORT_TIMEOUT;
        Power_Ampli_Ena();
        break;

    case SIREN_CONFIRM_OK_A:
        if (!siren_timeout)
        {
            if (siren_steps)
            {
                //se termino el bip
                siren_steps--;
                siren_timeout = SIREN_SHORT_TIMEOUT;
                siren_state = SIREN_CONFIRM_OK_B;
                Power_Ampli_Ena();
            }
            else
            {
                //termino la secuencia
                siren_state = SIREN_TO_STOP;
            }
        }
        break;

    case SIREN_CONFIRM_OK_B:
        if (!siren_timeout)
        {
            //se termino el tiempo
            Power_Ampli_Disa();
            siren_state = SIREN_CONFIRM_OK_A;
            siren_timeout = SIREN_SHORT_TIMEOUT;
        }
        break;

    case SIREN_MULTIPLE_DOWN:
        siren_steps = 0;
        siren_state = SIREN_MULTIPLE_DOWN_B;
        Power_Ampli_Ena();
        break;

    case SIREN_MULTIPLE_DOWN_A:
        if (!siren_timeout)
        {
            //se termino el tiempo, cambio la frecuencia
            if (siren_steps)
                siren_steps--;
            siren_state = SIREN_MULTIPLE_DOWN_B;
        }
        break;

    case SIREN_MULTIPLE_DOWN_B:
        if (siren_steps)
        {
            freq_us = freq_us - SIREN_FREQ_STEP;
            siren_timeout = SIREN_STEP_TIMEOUT;
            ChangeAmpli(freq_us, freq_us >> 1);
            siren_state = SIREN_MULTIPLE_DOWN_A;
        }
        else
        {
            //empiezo el ciclo de nuevo
            siren_steps = SIREN_STEP_RELOAD;
            freq_us = SIREN_FIRST_FREQ;
            ChangeAmpli(freq_us, freq_us >> 1);
            siren_state = SIREN_MULTIPLE_DOWN_A;
            siren_timeout = SIREN_STEP_TIMEOUT;
        }
        break;

    case SIREN_SHORT:
        siren_timeout = SIREN_SHORT_TIMEOUT;
        siren_state = SIREN_SHL_TIMEOUT;
        Power_Ampli_Ena();
        break;

    case SIREN_HALF:
        siren_timeout = SIREN_HALF_TIMEOUT;
        siren_state = SIREN_SHL_TIMEOUT;
        Power_Ampli_Ena();
        break;

    case SIREN_LONG:
        siren_timeout = SIREN_LONG_TIMEOUT;
        siren_state = SIREN_SHL_TIMEOUT;
        Power_Ampli_Ena();
        break;

    case SIREN_SHL_TIMEOUT:
        if (!siren_timeout)
        {
            //se termino el tiempo
            siren_state = SIREN_TO_STOP;
        }
        break;

    case SIREN_TO_STOP:
    default:
        Power_Ampli_Disa();
        siren_state = SIREN_INIT;
        break;
    }
}



//One_ms Interrupt
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
        TimingDelay--;

    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (pilot_code)
        pilot_code--;

    //estos dos que siguen podr�an ser el mismo
    if (interdigit_timeout)
        interdigit_timeout--;

    if (wait_for_code_timeout)
        wait_for_code_timeout--;

    //los del keypad
    if (keypad_timeout)
        keypad_timeout--;

    if (keypad_interdigit_timeout)
        keypad_interdigit_timeout--;
    
    if (buzzer_timeout)
        buzzer_timeout--;

    if (display_timeout)
        display_timeout--;

    if (siren_timeout)
        siren_timeout--;

    if (timer_keypad_enabled)
        timer_keypad_enabled--;

    if (f12_plus_timer)
        f12_plus_timer--;
    
#ifdef CON_MODIFICACION_DIODO_BATERIA
    if (timer_battery)
        timer_battery--;
#endif

#ifdef SOFTWARE_VERSION_2_5
    if (button4_only5mins_timeout)
	button4_only5mins_timeout--;
#endif
    
    //cuenta 1 segundo
    if (button_timer_internal)
        button_timer_internal--;
    else
    {
        if (button_timer_secs)
        {
            button_timer_secs--;
            button_timer_internal = 1000;
        }
    }
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
