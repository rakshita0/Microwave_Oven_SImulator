/* 
 * File:   main.h
 */

#ifndef MAIN_H
#define	MAIN_H

#define COOKING_MODE_DISPLAY    0x01
#define MICRO_MODE              0x02
#define GRILL_MODE              0x03
#define CONVECTION_MODE         0x04
#define START_MODE              0x05
#define TIME_DISPLAY            0x06
#define PAUSE_MODE              0x07
#define STOP_MODE               0x08

#define MICRO_MODE_RESET        0x11
#define GRILL_MODE_RESET        0x12
#define RESET_NOTHING           0xF0

#define BUZZER                  RC1
#define BUZZER_DDR              TRISC1
#define FAN                     RC2
#define FAN_DDR                 TRISC2
#define DOOR                    RB3
#define OPEN                    0
#define CLOSE                   1

void power_on_screen(void);
void cooking_operations(void);
void clear_display(void);
void operation_call(unsigned char);
void set_time(unsigned char);
void time_display(void);

#endif	/* MAIN_H */

