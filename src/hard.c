//----------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #################################
//----------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"


// Module Private Types & Macros -----------------------------------------------
#define SWITCHES_THRESHOLD_ROOF	4000    // 4 secs roof
#define SWITCHES_THRESHOLD_FULL	3000    // 3 secs
#define SWITCHES_THRESHOLD_HALF	1000    // 1 sec
#define SWITCHES_THRESHOLD_MIN	50    // 50 ms

// Led blinking states
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
    
} led_state_t;

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned short s1_cntr = 0;

// for the LEDs
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;
unsigned short timer_led_on = 0;
unsigned short timer_led_off = 0;
volatile unsigned short timer_led = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char Led_Is_On (void)
{
    return LED;
}


void Led_On (void)
{
    LED_ON;
}


void Led_Off (void)
{
    LED_OFF;
}


unsigned char Sw_Learn_Is_On (void)
{
    return SW_LEARN;
}


void Hard_Timeouts (void)
{
    if (timer_led)
	timer_led--;
    
    if (SW_LEARN)
    {
	if (s1_cntr < SWITCHES_THRESHOLD_ROOF)
	    s1_cntr++;
    }
    else if (s1_cntr > 50)
	s1_cntr -= 50;
    else if (s1_cntr > 10)
	s1_cntr -= 5;
    else if (s1_cntr)
	s1_cntr--;

}


resp_sw_t Check_Sw_Learn (void)
{
    resp_sw_t sw = SW_NO;

    if (s1_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (s1_cntr > SWITCHES_THRESHOLD_HALF)
	sw = SW_HALF;
    else if (s1_cntr > SWITCHES_THRESHOLD_MIN)
	sw = SW_MIN;
    
    return sw;    
}


// change led bips
void Hard_Led_Change_Bips (unsigned char how_many,
			   unsigned short timer_on,
			   unsigned short timer_off)
{
    how_many_blinks = how_many;
    timer_led_on = timer_on;
    timer_led_off = timer_off;
    led_state = START_BLINKING;
}


// blinking the led
void Hard_Led_Blinking_Update (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_ON;
                timer_led = timer_led_on;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_OFF;
                timer_led = timer_led_on;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = timer_led_on;
                    led_state = WAIT_TO_OFF;
                    LED_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = timer_led_off;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


void Relay_Ch1_On (void)
{
    ACT_CH1_ON;
}


void Relay_Ch2_On (void)
{
    ACT_CH2_ON;
}


void Relay_Ch3_On (void)
{
    ACT_CH3_ON;
}
    
void Relay_Ch4_On (void)
{
    ACT_CH4_ON;
}


void Relay_Ch1_Off (void)
{
    ACT_CH1_OFF;
}


void Relay_Ch2_Off (void)
{
    ACT_CH2_OFF;
}


void Relay_Ch3_Off (void)
{
    ACT_CH3_OFF;
}


void Relay_Ch4_Off (void)
{
    ACT_CH4_OFF;
}


unsigned char Relay_Ch1_Is_On (void)
{
    return ACT_CH1;
}


unsigned char Relay_Ch2_Is_On (void)
{
    return ACT_CH2;
}


unsigned char Relay_Ch3_Is_On (void)
{
    return ACT_CH3;
}
    
unsigned char Relay_Ch4_Is_On (void)
{
    return ACT_CH4;
}


//--- end of file ---//
