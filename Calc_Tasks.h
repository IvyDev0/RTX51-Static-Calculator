#include <reg51.h>
#include <intrins.h>
#include <rtx51tny.h>
#include <stdio.h> 
#include <string.h>

#define uchar unsigned char
#define INIT 0
#define TASK1 1
#define TASK2 2
#define DELAYTIME 10

#define DISPLAYMODE 0x38
#define CLS 0x01
#define OPENDISPLAY 0X0F
#define CURSORRESET 0X02
#define MOVECURSOR 0X06

#define COLUMN0 0XFE
#define COLUMN1 0XFD
#define COLUMN2 0XFB
#define COLUMN3 0XF7
#define NULLCOLUMN 0X0F
#define NULLKEY 0XF0
#define OP  16    

#define KeyPort P1
#define DataPort P2

// KEYPAD
uchar KeyScan(void);
uchar KeyEncoded(uchar KeyScanCode);
int get_num(uchar ch);

// LCD 
uchar LCD_ReadStatus(void);
void SetPosition(uchar x, uchar y);
void LCD_WriteCmd(uchar CMD, uchar Busy);
void LCD_WriteData(uchar WData);
void LCD_DisplayLine(char Line[]);
void disp_float(float);
void disp_num(int);

// Global Variables
uchar KeyBuffer;
uchar DisplayFlag;
int tmp, numb, sum, mark, digit, i;                 
int numbers[20];
float avg, varsum;
char floatstr[10];

// Pin description
/*
P2 is data bus
P3.7 is RS
P3.6 is E
P1.0 to P1.3 are keypad row outputs
P1.4 to P1.7 are keypad column inputs
*/
sbit LCD_E  = P3^6;     //E pin for LCD
sbit LCD_RS = P3^7;     //RS pin for LCD