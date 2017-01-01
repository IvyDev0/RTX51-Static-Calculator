#include "Calc_Tasks.h"

int flag = 0;

uchar KeyScan(void)
{
    uchar ScanValue;
    KeyPort = COLUMN0;
    ScanValue = KeyPort & NULLKEY;
    if (ScanValue != NULLKEY)
    {
        ScanValue |= (COLUMN0 & NULLCOLUMN);
        return ScanValue;
    }
    KeyPort = COLUMN1;
    ScanValue = KeyPort & NULLKEY;
    if (ScanValue != NULLKEY)
    {
        ScanValue |= (COLUMN1 & NULLCOLUMN);
        return ScanValue;
    }
    KeyPort = COLUMN2;
    ScanValue = KeyPort & NULLKEY;
    if (ScanValue != NULLKEY)
    {
        ScanValue |= (COLUMN2 & NULLCOLUMN);
        return ScanValue;
    }
    KeyPort = COLUMN3;
    ScanValue = KeyPort & NULLKEY;
    if (ScanValue != NULLKEY)
    {
        ScanValue |= (COLUMN3 & NULLCOLUMN);
        return ScanValue;
    }
    return (NULLCOLUMN | NULLKEY);
}

uchar KeyEncoded(uchar KeyScanCode)
{
    uchar KeyCode = NULLCOLUMN | NULLKEY;
    switch(KeyScanCode)
    {
        case 0xEE:KeyCode='0';break;
        case 0xED:KeyCode='1';break;
        case 0xEB:KeyCode='2';break;
        case 0xE7:KeyCode='3';break;

        case 0xDE:KeyCode='4';break;
        case 0xDD:KeyCode='5';break;
        case 0xDB:KeyCode='6';break;
        case 0xD7:KeyCode='7';break;
        
        case 0xBE:KeyCode='8';break;
        case 0xBD:KeyCode='9';break;
        case 0xBB:KeyCode='A';break; // ADD
        case 0xB7:KeyCode='B';break; // CLC
        
        case 0x7E:KeyCode='C';break; // VAR(X)
        case 0x7D:KeyCode='D';break; // AVG(X)
        case 0x7B:KeyCode='E';break; // SUM(X)
        case 0x77:KeyCode='F';break; // SUM(X^2)
    }
    return KeyCode;
}

int get_num(uchar ch)         //convert char into int
{
    switch(ch)
    {
        case '0': return 0; break;
        case '1': return 1; break;
        case '2': return 2; break;
        case '3': return 3; break;
        case '4': return 4; break;
        case '5': return 5; break;
        case '6': return 6; break;
        case '7': return 7; break;
        case '8': return 8; break;
        case '9': return 9; break;
        case 'A': LCD_WriteCmd(CLS, 0);return 10; break;
        case 'B': LCD_WriteCmd(CLS, 0);return 11; break;
        case 'C': LCD_WriteCmd(CLS, 0);return 12; break;
        case 'D': LCD_WriteCmd(CLS, 0);return 13; break;
        case 'E': LCD_WriteCmd(CLS, 0);return 14; break;
        case 'F': LCD_WriteCmd(CLS, 0);return 15; break;
        default: LCD_WriteCmd(CLS, 0); LCD_DisplayLine("ERROR");return 16; break;     
    }
}

uchar LCD_ReadStatus(void) 
{
    DataPort = 0XFF;
    LCD_RS = 0;
    LCD_E = 1;
    while(DataPort & 0x80);
    return DataPort;
}

void LCD_WriteCmd(uchar CMD, uchar Busy)
{
    if (Busy)
        LCD_ReadStatus();
    LCD_RS = 0;
    LCD_E = 1;
    DataPort = CMD;
    LCD_E = 0;
    _nop_(); _nop_(); _nop_(); _nop_();
    LCD_E = 1;
}

void LCD_WriteData(uchar WData)
{
    LCD_RS = 1;
    LCD_E = 1;
    DataPort = WData;
    LCD_E = 0;
    _nop_(); _nop_(); _nop_(); _nop_();
    LCD_E = 1;
}

void LCD_DisplayLine(char Line[])
{
    int i;
    for (i=0; i<strlen(Line); ++i)
        LCD_WriteData(Line[i]);

    _nop_(); _nop_(); _nop_(); _nop_();
}

void disp_float(float res)
{
     sprintf(floatstr,"%0.2f",res);
     LCD_DisplayLine(floatstr);

}
void disp_num(int numb)            //displays number on LCD
{
    int i,j,n,len=1;
    int tmp=numb;
    if(numb<0)
    {
        numb = -1*numb;  // Make number positive
        LCD_WriteData('-');  // Display a negative sign on LCD
    }

    while((tmp=tmp/10)!=0)  
        len++;
    
    for(i=len;i>0;i--)
    {
        n=1;
        for(j=0;j<i-1;j++)
            n=n*10;
        LCD_WriteData(0x30+numb/n); 
        numb = numb%n;
    }
}
void TASK1_ReadyKey(void) _task_ TASK1
{
    uchar KeyScanValue = NULLKEY|NULLCOLUMN;
    uchar KeyValue;

    while(1)
    {
        KeyScanValue = KeyScan();
        if(KeyScanValue!=(NULLKEY|NULLCOLUMN)) 
        {
            flag = 1;
            KeyValue = KeyScanValue;
        }
        else if(KeyScanValue==(NULLKEY|NULLCOLUMN) && flag == 1)
        {   
            flag = 0;
            KeyBuffer = KeyEncoded(KeyValue);
            DisplayFlag = 1;
        }
    }
}

void TASK2_DisplayBuffer(void) _task_ TASK2
{
    while(1)
    {
        if(DisplayFlag)
        {
            os_wait(K_IVL, DELAYTIME, 0);
            tmp = get_num(KeyBuffer);
            if(mark == 1){
                LCD_WriteCmd(CLS, 0); 
                mark = 0;
            }
            if(tmp<10){  // tmp is a number
                LCD_WriteData(KeyBuffer);
                os_wait(K_IVL, DELAYTIME, 0);
                numb = numb*10+tmp;
            }
            else{
                if(tmp>11 && digit==0)
                    LCD_DisplayLine(" WRONG INPUT");
                else{
                    switch(tmp)         
                    {
                    case 10:
                        //LCD_DisplayLine("ADD");
                        numbers[digit] = numb;
                        //LCD_WriteData(0x30+numbers[digit]);
                        ++digit; numb = 0;                    
                        break;
                    case 11: // CLC
                        for(i = 0; i<digit; ++i)
                            numbers[i] = 0;
                        digit = 0; sum = 0; numb = 0; 
                        break;
                    case 12: // VAR
                        for(i = 0; i<digit; ++i)
                            sum += numbers[i];
                        avg = 1.0*sum/i;
                        varsum = 0;
                        for(i = 0; i<digit; ++i)
                        {
                            varsum += (numbers[i]-avg)*(numbers[i]-avg);
                            numbers[i] = 0;
                        }
                        LCD_DisplayLine(" VAR(X):");
                        disp_float(varsum/i); 
                        digit = 0; sum = 0; mark = 1;
                        break;
                    case 13: // AVG
                        for(i = 0; i<digit; ++i)
                        {
                            sum += numbers[i];
                            numbers[i] = 0;
                        }
                        LCD_DisplayLine(" AVG(X):");
                        disp_float(1.0*sum/i); 
                        digit = 0; sum = 0; mark = 1;
                        break;
                    case 14: // SUM
                        for(i = 0; i<digit; ++i)
                        {
                            sum += numbers[i];
                            numbers[i] = 0;
                        }
                        LCD_DisplayLine(" SUM(X):");
                        disp_num(sum);
                        os_wait(K_IVL, DELAYTIME, 0);
                        digit = 0; sum = 0; mark = 1;
                        break;
                    case 15: // SUM2
                        for(i = 0; i<digit; ++i)
                        {
                            sum += numbers[i]*numbers[i];
                            numbers[i] = 0;
                        }
                        LCD_DisplayLine(" SUM(X^2):");
                        disp_num(sum); 
                        digit = 0; sum = 0; mark = 1; 
                        break;
                    }    
                }   
            }
            DisplayFlag = 0;
        }
    }
}

void TASK0(void) _task_ INIT
{
    P0 = 0x00;   // not used
    P1 = 0xf0;   // used for generating outputs and taking inputs from Keypad
    P2 = 0x00;   // used as data port for LCD
    P3 = 0x00;   // used for RS and E  

    DisplayFlag = 0;
    digit = 0; sum = 0; mark = 0; numb = 0;
    for(i = 0; i<20; ++i)
        numbers[i] = 0;

    while(1)
    {
        LCD_WriteCmd(DISPLAYMODE, 0);
        os_wait(K_IVL, 1, 0);
        LCD_WriteCmd(OPENDISPLAY, 0); // display the cursor
        os_wait(K_IVL, 1, 0);
        LCD_WriteCmd(MOVECURSOR,0); // cursor move with input
        os_wait(K_IVL, 1, 0);
        LCD_WriteCmd(CLS, 0); // clear sreen
        os_wait(K_IVL, 1, 0);
        LCD_WriteCmd(CURSORRESET,0); // reset cursur

        os_create_task(TASK1);
        os_create_task(TASK2);
        os_delete_task(INIT);
    }
}
