/*
 * File:   main.c
 */

#include <xc.h>
#include "clcd.h"
#include "main.h"
#include "matrix_keypad.h"
#include "timers.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)
int operation_mode, reset_flag;
int min,sec;
char min_arr[3], sec_arr[3];
static void init_config(void) {
    init_clcd();            //initialize clcd module
    init_matrix_keypad();   //initialize matrix keypad
    init_timer2();          //initialize timer 2
    BUZZER_DDR = 0;         //output pin
    //initially the buzzer is off
    BUZZER = 0;
    
    FAN_DDR = 0;
    //initially the fan is off
    FAN = 0;
    
    PEIE = 1;               //enable the peripheral interrupt enable
    GIE = 1;                //enable the global interrupt enable
    
}

void main(void) {
    unsigned char key;
    init_config();
    power_on_screen();
    clear_display();
    operation_mode = COOKING_MODE_DISPLAY;

    while (1) {
        key = read_matrix_keypad(STATE);
        if(operation_mode == MICRO_MODE)
        {
            ;                   //not doing anything
        }
        else if(key==1)
        {
            operation_mode = MICRO_MODE;
            reset_flag = MICRO_MODE_RESET;
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            clear_display();
            //__delay_ms(2000);
            clcd_print("Power = 900W",LINE2(0));
            __delay_ms(2000);
            clear_display();
        }
        else if(key==2)
        {
            operation_mode = GRILL_MODE;
            reset_flag = GRILL_MODE_RESET;
            clear_display();
        }
        else if(key==3)
        {
            operation_mode = CONVECTION_MODE;
        }
        else if(key==4)
        {
            if(operation_mode == COOKING_MODE_DISPLAY)
            {
                min = 0;
                sec = 30;
                TMR2ON = 1;
                FAN = 1;
                operation_mode = TIME_DISPLAY;
            }
            else if(operation_mode == TIME_DISPLAY)
            {
                sec = sec + 30;
                if(sec>=60)
                {
                    min++;
                    sec = sec - 60;
                }
            }
            else if(operation_mode == PAUSE_MODE)
            {
                TMR2ON = 1;
                FAN = 1;
                operation_mode = START_MODE;
            }

        }
        else if(key == 5)
        {
            operation_mode = PAUSE_MODE;
        }
        else if(key==6)
        {
            operation_mode = STOP_MODE;
        }
        operation_call(key);
        
        
       
    }
}
void power_on_screen(void)
{
    for(int i = 0;i < 16;i++){
        clcd_putch(BLOCK,LINE1(i));
    }
    clcd_print("Powering ON",LINE2(2));
    clcd_print("Microwave Oven",LINE3(1));
    for(int i = 0;i < 16;i++){
        clcd_putch(BLOCK,LINE4(i));
    }
    __delay_ms(2000);               //to generate a delay of 2sec
}

void clear_display(void){
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
}

void cooking_operations(void){
    clcd_print("1. Micro",LINE1(0));
    clcd_print("2. Grill",LINE2(0));
    clcd_print("3. Convection",LINE3(0));
    clcd_print("4. Start",LINE4(0));
    
    __delay_ms(2000);
}

//this function will check the flag value
void operation_call(unsigned char key){
    switch(operation_mode){
        case COOKING_MODE_DISPLAY:
            cooking_operations();
            break;
        case START_MODE:
        case TIME_DISPLAY:
            time_display();
            break;
        case MICRO_MODE:
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            set_time(key);
            break;
        case GRILL_MODE:
            set_time(key);
            break;
        case CONVECTION_MODE:
            break;
        case PAUSE_MODE:
            //switch off the timer and fan
            TMR2ON = 0;
            FAN = 0;
            break;
        case STOP_MODE:
            //switch off the timer and fan and set operation_mode to cooking display mode
            TMR2ON = 0;
            FAN = 0;
            clear_display();
            operation_mode = COOKING_MODE_DISPLAY;
            break;
    }
    reset_flag = RESET_NOTHING;
}
void set_time(unsigned char key){
    static int blink, wait, blink_posi, key_count;
    if(reset_flag >= 0x11){             //initially this condition will be true
        key = ALL_RELEASED;
        key_count = 0;
        sec = 0, min = 0;
        blink = 0;
        blink_posi = 0;
        clcd_print("SET TIME <MM:SS>",LINE1(0));
        clcd_print("TIME- 00:00",LINE2(0));
        clcd_print("*:CLEAR  #:ENTER",LINE4(0));
    }
    if(key!=ALL_RELEASED && key!='*' && key!='#')
    {
        //key: 0,1,2....9
        key_count++;
        if(key_count<=2)
        {
            sec = sec*10 + key;
            blink_posi = 0;
        }
        else if(key_count>2 && key_count<5)
        {
            min = min*10 + key;
            blink_posi = 1;
        }
        
    }
    if(key == '*')
    {
        if(key_count<=2)
        {
            sec = 0;
            key_count = 0;
        }
        else if(key_count>2 && key_count<5)
        {
            min = 0;
            key_count = 2;
        }
    }
    else if(key == '#')
    {
        //timer2 ON
        clear_display();
        TMR2ON = 1;
        FAN = 1;
        operation_mode = TIME_DISPLAY;
    }
    //print min and sec on screen
    sec_arr[0] = sec/10 + '0';
    sec_arr[1] = sec%10 + '0';
    sec_arr[2] = '\0';
    
    min_arr[0] = min/10 + '0';
    min_arr[1] = min%10 + '0';
    min_arr[2] = '\0';
   
    //for 50 iterations it will blink number of sec
    if(wait++ == 50){
        wait = 0;
        blink = !blink;
        clcd_print(min_arr,LINE2(6));
        //clcd_putch(":",LINE2(8));
        clcd_print(sec_arr,LINE2(9));
    }
    if(blink){
        switch(blink_posi){
            case 0: //blink sec
                clcd_print("  ",LINE2(9));  //9 and 10 position
                break;
            case 1: //blink min
                clcd_print("  ",LINE2(6));
                break;
        } 
    }
    
}
void door_status_check(void)
{
    //RB3 switch acts for opening and closing of door
    if(DOOR == OPEN)
    {
        BUZZER = 1;
        FAN = 0;
        TMR2ON = 0;
        clear_display();
        clcd_print("Door Open",LINE2(2));
        clcd_print("Please close",LINE3(2));
        while(DOOR == OPEN)
        {
            ;
        }
        clear_display();
        
    }
    TMR2ON = 1;
    BUZZER = 0;
    FAN = 1;

}
void time_display(void)
{
    door_status_check();
    clcd_print("TIME =   ",LINE1(0));
    //firstly we will convert the min_arr into string
    min_arr[0] = min/10 + '0';
    min_arr[1] = min%10 + '0';
    min_arr[2] = '\0';
    clcd_print(min_arr,LINE1(8));
    
    clcd_putch(":",LINE1(10));
    
    sec_arr[0] = sec/10 + '0';
    sec_arr[1] = sec%10 + '0';
    sec_arr[2] = '\0';
    clcd_print(sec_arr,LINE1(11));
    
    clcd_print(" 4.Start/Resume      ",LINE2(0));
    clcd_print(" 5.Pause             ",LINE3(0));
    clcd_print(" 6.Stop              ",LINE4(0));
    
    //when the time becomes 0 than there is no need to display it
    if(min == 0 && sec == 0)
    {
        TMR2ON = 0;           //as there is no use of timer therefore we can switch off it
        FAN = 0;
        clear_display();
        clcd_print("COOKING TIME UP",LINE1(0));
        
        //switch on the buzzer
        BUZZER = 1;
        __delay_ms(2000);
        //turn off the buzzer
        BUZZER = 0;
        clear_display();
        operation_mode = COOKING_MODE_DISPLAY;
        
    }
    
    
}
  
