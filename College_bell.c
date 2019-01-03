#include <CloudX/M633.h>
#include <CloudX/Ds1302.h>
#include <CloudX/LCD5110.h>
#include <CloudX/Serial.h>
#include <CloudX/Keypad.h>
#include <string.h>

//#define _font_5x7_
#define _font_3x5_
#include <CloudX\Font.h> 


char readAndStore(char *Time, byte x);
byte writeArrayToRAM(char *timearray,byte address);
void ReadArrayFromRAM(char *timearray);
byte compareTimeArray(char time[], char time2[],int size);
byte SetupTime(char *timearray);
byte readKeys(char *time);
void LCD_clear();
char Keys1;

byte keypadMap[10]={0,1,2,3,4,5,6,7,8,9};

byte lastwriteaddr=0;
unsigned int lastwritecount =0;
void main(void){  

    pinMode(6, OUTPUT);
    digitalWrite(6,LOW);
    
 byte minutes=0;
 byte seconds=0;
 byte hours=0; 
 
DS1302_init(8,7,12);  //CLK IS 8 DATA I/O IS 7, CE IS 12.......
//SetDate_and_time(5,8,18,30,7,8,0); // write  a function call around this to set time with keypad...(:
Serial_begin(9600);

LCD5110_setting(1, 2, 3, 4, 5 );  //Rset,CE,DC,DIn,Clk

char time[9] = {'0','0',':','0','0',':','0','0','\0'}; //array time for the clock being read ..........  
char time2[9] = {'0','0',':','0','0',':','0','0','\0'}; // array of time being entered .........
  LCD5110_clear();
//Lcd_cmd(cursorOff);
 
 /*****************************************************************************/
 /////////////////////KEYPAD_INITIALIZTION/////////////////////////////////////
 
 #define NumberOfRows  4            // set display to four rows
#define NumberOfColumns  3           // set display to three columns
char  KeypadCharacters[NumberOfRows][NumberOfColumns] = {
    '1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#'
};
char RowPins[NumberOfRows] = {13,14,15,16};
char ColumnsPins[NumberOfColumns] = {9,10,11};

char Keys;

 KeypadSetting(PULLUPROW, RowPins, ColumnsPins, NumberOfRows, NumberOfColumns, KeypadCharacters,6);

 
while(1){

 LCD5110_writeText(FONT_3x5, 3,32, 1, 2, "SMART-BELL", White); 
 LCD5110_writeText(FONT_3x5,3,32, 2, 4, "USING", White); 
 LCD5110_writeText(FONT_3x5,3,32, 3,3, "CLOUDX", White); 
 
unsigned int Ringsno1=0;
    
Time_read( &hours,&minutes,&seconds);


time[0]= BCD2UpperCh(hours);
time[1]=BCD2LowerCh(hours);
time[3]=BCD2UpperCh(minutes);
time[4]= BCD2LowerCh(minutes);
time[6]= BCD2UpperCh(seconds); 
time[7]= BCD2LowerCh(seconds);

char ringsno[];

//Lcd_writeText(3,0,"TIME:");
 LCD5110_writeText(FONT_3x5,3,32, 5, 2, "TIME:", White);
 LCD5110_writeText(FONT_3x5,3,32, 5,6,time, White);
//Lcd_writeText(3,5,time);
//we need to keep track of time we stored 
// into the RAM Last using the global variable 
//lastwriteaddr ........ we used lastwritecount to read as our reading has to start from address 
// where we wrote to last 
// also know we had to reduce the lastwriteaddr while reading by 2, 

if(lastwriteaddr!=0){

    lastwritecount = lastwriteaddr;
   while(lastwritecount !=0) {
        lastwritecount -=2;
       ReadArrayFromRAM(time2);
       
      
       Serial_writeText(time2);
       Serial_writeText("\r\n");
      if(compareTimeArray(time,time2,5)==1){
   
     // Lcd_writeText(4,-1,"BREAK-TIME NERDS");}else{ //do the following
          LCD5110_writeText(FONT_3x5,3,32, 6,1,"BREAK TIME KIDS", White);
         
          digitalWrite(6,HIGH);
          
     // Lcd_writeText(4,-1,"                ");
     // Lcd_writeText(4,5,time2);
      
      }else{
       digitalWrite(6,LOW);
       LCD5110_writeText(FONT_3x5,3,32, 6,1,"               ", White);
      }
   }
}



Keys=getKey();
if(Keys=='*'){
    char back;
    lastwriteaddr=0;
     LCD_clear();
//    Lcd_cmd(clear);
     LCD5110_writeText(FONT_3x5,3,32, 1,1,"PRESS # FOR BACK", White);
     LCD5110_writeText(FONT_3x5,3,32, 2,1,"ENTER NO OF RINGS", White);
 //   Lcd_writeText(1,1,"PRESS # FOR BACK");
 //   Lcd_writeText(2,1,"ENTER NO OF RING");
    Keys=getKey();
    while(Keys !='#'){
    byte counter=0;
    while(Keys !='*'){
        switch (Keys)
      {
         case 0:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
              ringsno[counter]= Keys;   
              ++counter;     
              break;
    }
      Keys=getKey();
      back = 0;
      //this takes care of the back algorithm
      if(Keys == '#'){
          back =1; Keys = '*'; // Lcd_cmd(clear);
      }
    }
    if(back == 0){
    ringsno[counter]='\0';
    LCD5110_writeText(FONT_3x5,3,32, 3,3,"NO:", White);
      LCD5110_writeText(FONT_3x5,3,32, 3,7,ringsno, White);
  //  Lcd_writeText(3,1,"NO:");
  //  Lcd_writeText(3,6,ringsno);
   // DelayMs(2000);
    //i need to work on this to convert interger array to a whole number
   // Ringsno1 = atoi(ringsno);
   
      char *myrings;
      myrings = ringsno;
    for(byte x=0; x<counter; x++){
  
   Ringsno1 = (10*(Ringsno1) + keypadMap[*(myrings++)-'0']); 
    }
    
    if(Ringsno1 == 0) { LCD5110_writeText(FONT_3x5,3,32, 4,2,"NOT VALID", White);Keys = '#'; DelayMs(1000); Ringsno1 = 0;}
    if(Ringsno1 > 15) { LCD5110_writeText(FONT_3x5,3,32, 4,1,"MAX-NO-EXCEEDED", White);Keys='#';DelayMs(2000); Ringsno1 = 0;}
    
    LCD_clear();
    
     lastwriteaddr=0;
     for(byte x=1; x<=Ringsno1;x++){
    Keys = readAndStore(time,x);
       LCD_clear();
     }
    }else{ Keys ='#';
    }
    }
}

if(Keys == '0'){
if(SetupTime(time)){
    
 LCD5110_writeText(FONT_3x5,3,32, 5,3,"SAVE SUCESSFULL", White);
DelayMs(1000);
 LCD5110_clear();
}else{
    
 LCD5110_writeText(FONT_3x5,3,32, 5,3,"WRONG INPUT    ", White);
DelayMs(1000);
LCD5110_clear();
 
}

}
}
}

char readAndStore(char *Time, byte x){   //Time was passed into the array to save memory from definig new one.
    char *time;
    char Keys,round,round1,round2;
    round= Binary2BCD(x);
    time = Time;
     byte terminated=0;
   
time[0]= '0';
time[1]= '0';
time[3]= '0';
time[4]= '0';
time[6]= '0';
time[7]= '0';

//Lcd_writeText(1,5,"RINGS:"); 
LCD5110_writeText(FONT_3x5,3,32, 1,3,"RINGS:", White);
round1 = BCD2UpperCh(round);round2 = BCD2LowerCh(round);LCD5110_writeCP(FONT_3x5,3,32,round1);;LCD5110_writeCP(FONT_3x5,3,32,round2);
//Lcd_writeText(2,5,time);
LCD5110_writeText(FONT_3x5,3,32, 3,1,"TIME:", White);


/**********************************entry point*******/
 terminated = readKeys(time);
     
    //if(count==8 && terminated == 2) {return '#';}  
    if(terminated == 0){
       //wait for * to be pressed
        while(Keys!='*') Keys = getKey();
        //write to PIC MCU EEPROM KEEP TRACK OF EEPROM USING ADDRESS
     if(writeArrayToRAM(time,lastwriteaddr)){
       LCD5110_writeText(FONT_3x5,3,32, 5,1,"SAVE SUCESSFULL", White);
      //Lcd_writeText(3,-3,"SAVE SUCESSFULL");
      DelayMs(2000);
     }else{
         LCD5110_writeText(FONT_3x5,3,32,5,1,"SAVE FAILED    ", White);
  //   Lcd_writeText(3,-3,"SAVE  FAILED    ");
     }
                  
        return '#';//TO BREAK AWAY FROM OUTER LOOP
     // break ;// last statement  
    }
    if (terminated ==1){
    // write to Ram and set key;
    if(writeArrayToRAM(time,lastwriteaddr)){
       
    //  Lcd_writeText(3,-3,"SAVE SUCESSFULL");
         LCD5110_writeText(FONT_3x5,3,32, 5,1,"SAVE SUCESSFULL", White);
      DelayMs(2000);
     }else{
  //   Lcd_writeText(3,1,"SAVE  FAILED    ");
         LCD5110_writeText(FONT_3x5,3,32, 5,1,"SAVE FAILED    ", White);
      DelayMs(1000);
     }
        return '#'; //acess to exit the loop...
    }

}
//Lcd_cmd(clear);
    
byte writeArrayToRAM(char *timearray, byte address){ //stores minutes and hours
    char *mytime;
    
    byte hr=0,mins=0;
    mytime = timearray;
    byte address1 = address;
    //
    for(byte x=0; x<5; x++){
      
        if(x<2) hr = 10*(hr) + keypadMap[*(mytime++)-'0'];
     //   PORTC = hr;
        if(x==2) *(mytime++);
        if (x>2) mins = 10*(mins)+ keypadMap[*(mytime++)-'0']; 
     //   PORTC =mins;
    }
    if((hr > 24)||(mins > 60)) return 0;
    RAM_write(address1,hr); address1= address1+2;
    RAM_write(address1,mins);
    lastwriteaddr +=4;
    return 1;
    //return next address
}
void ReadArrayFromRAM(char *timearray){
    char *mytime;
    byte hr=0,mins=0;
    mytime = timearray;
 //   hr= Read_from_ram(address1);++address;hr= BCD2Binary(hr);
    //WE ARE READING FROM THE LAST POSTION WRITTEN TO AND BACK TO THE FIRST
   mins = RAM_read(lastwritecount);lastwritecount = lastwritecount-2;mins =Binary2BCD(mins);
   hr= RAM_read(lastwritecount);hr= Binary2BCD(hr);   
   mytime[0]=BCD2UpperCh(hr);
   mytime[1]=BCD2LowerCh(hr);
   mytime[3]=BCD2UpperCh(mins);
   mytime[4]=BCD2LowerCh(mins);

 }
// u need to rewrite this method 
byte compareTimeArray(char *time, char *time2,int size){
    for (int x=0; x<size;x++){
        if(*(time++) != *(time2++))return 0;
    }
    return 1;
    }


byte SetupTime( char *timearray){
    //char *array1;
timearray[0]= '0';timearray[1]= '0';timearray[3]= '0';timearray[4]= '0';timearray[6]= '0';timearray[7]= '0';
char terminated,Keys;
 byte hr=0,mins=0;

    LCD5110_clear();
       LCD5110_writeText(FONT_3x5,3,32, 3,5,timearray, White);
   // Lcd_writeText(2,5,timearray);
    terminated = readKeys(timearray);
    if(terminated == 0){
    while(Keys!='*') Keys = getKey();
    
    for(byte x=0; x<5; x++){
        if(x<2) hr = 10*(hr) + keypadMap[*(timearray++)-'0'];
        if(x==2) *(timearray++);
        if (x>2) mins = 10*(mins)+ keypadMap[*(timearray++)-'0']; 
    }
    if((hr > 24)||(mins > 60)) return 0;
  Time_write(hr,mins,0);
    }
    if(terminated == 1){  // reason for terminations is to store both when you enter few nos into array or when u've filled 
     for(byte x=0; x<5; x++){  // the array.............
        if(x<2) hr = 10*(hr) + keypadMap[*(timearray++)-'0'];
        if(x==2) *(timearray++);
        if (x>2) mins = 10*(mins)+ keypadMap[*(timearray++)-'0']; 
    }
    if((hr > 24)||(mins > 60)) return 0;
   Time_write(hr,mins,0); return 1;
    }
    
    }
    
   

byte readKeys(char *time){
    byte count= 0,terminated=0;
    
    while(count<8){
        LCD5110_writeText(FONT_3x5,3,32, 3,5,time, White);
     Keys1=getKey();
    switch(Keys1){
     case 0:
        if((count == 2)||(count== 5)){Keys1 =':'; time[count] = Keys1; ++count;LCD5110_writeText(FONT_3x5,3,32, 3,5,time, White); terminated =0;}
        break;
        
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        
        time[count] = Keys1;
        ++count;
        //Lcd_writeText(2,5,time);
        LCD5110_writeText(FONT_3x5,3,32, 3,5,time, White);
         terminated =0;
        break; 
        case '*':
            count = 8;
            terminated =1;
            break;
        case'#':  //delete
            --count;
            if ((count != 2)&&(count !=5)) time[count] = '0'; 
             LCD5110_writeText(FONT_3x5,3,32, 3,5,time, White);
          //   Lcd_writeText(2,5,time); 
            break;
      
    }
    }
      return terminated;
}

void LCD_clear(){
 LCD5110_clear();
    LCD5110_fillScreen(Black);
    DelayMs(1000);
    LCD5110_clear();
}
  

