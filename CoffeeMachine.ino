
//https://arduino-info.wikispaces.com/LCD-Blue-I2C
//https://www.sparkfun.com/tutorials/246
//http://playground.arduino.cc/Learning/SerialLCD

/*
 * incorporate auto shutoff into menu, ability to turn on and off and change time
 * 
 */

/*
 * DMarkwell
 * Arduino Timer on/off and grinder timer control
 * arduino Nano + rtc ds3231 + 20x4 LCD + 9 buttons
 */
 
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>   //Libraries
#include "RTClib.h"
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
 
RTC_DS3231 RTC;// define the Real Time Clock object; SDA to analog 4,  SCL to analog 5


// ------------Button and Pin Declaration------------



//-------------BUTTON PANEL----------
const int Menu=4; // BROWN - SWITCH TO ACCESS/EXIT THE MENU
const int TIMER_SWITCH =5; //WHITE/BROWN - switch to turn the timer function on and off
const int GRIND_L_UP =6; //BLUE
const int GRIND_L_DOWN =7;//WHITE/BLUE
const int GRIND_R_UP =8;//GREEN
const int GRIND_R_DOWN =9;//WHITE/GREEN

//-----------------------------------




//----------COFFEE MACHINE-----------

//switch to turn the coffee machine on and off
const int COFFEE_ON =3;//BLUE

//relay for the coffee machine.
const int Relay =A1; //BROWN - HIGH LEVEL TRIGGER

// WHITE/BLUE AND WHITE/BROWN UN-USED

//-----------------------------------


//------------GRINDER----------------

//left and right switches on the grinder
const int GRIND_L =11;//GREEN
const int GRIND_R =12;//WHITE/GREEN

//relay for the grinder
const int Relay2 =A2; //BROWN - LOW LEVEL TRIGGER

//WHITE/BROWN UN-USED

//-----------------------------------


//---------------I2C-----------------
// SCL -- GREEN
//SDA -- WHITE/GREEN
//-----------------------------------


// ORANGE -- +5V
// WHITE/ORANGE -- GROUND



//-------------Global variable declaration-------------------

int set_onHH = 8;    //The default "ON" hour
int set_onMM = 0;    //The default "ON" minute
int set_offHH = 9;   //The default "OFF" hour
int set_offMM = 30;  //The default "OFF" minute
int set_onwHH = 9;   //The default WEEKEND "ON" hour
int set_onwMM = 0;   //The default WEEKEND "ON" minute
int set_offwHH = 12; //The default WEEKEND "OFF" hour
int set_offwMM = 0;  //The default WEEKEND "OFF" minute
int sleepTime = 22;  // The default auto-off time - prevents machine being left on overnight
int set_L = 14;          //The default Left grind time
int set_R = 0;           //The default Right grind time
boolean light = HIGH;
boolean Timer=LOW;
unsigned long grindRunL;
unsigned long grindRunR;
boolean sleep=HIGH;
boolean daylight = LOW;
boolean lastdaylight;
boolean notify = LOW;
int flash=0;




//  Variables for debouncing

boolean lastTimerSwitch =LOW;
boolean currentTimerSwitch =LOW;
boolean currentCoffeeON = LOW;
boolean lastCoffeeON =LOW;
boolean CoffeeState =LOW;
boolean lastGrinderLup =LOW;
boolean currentGrinderLup =LOW;
boolean lastGrinderLdown =LOW;
boolean currentGrinderLdown =LOW;
boolean lastGrinderRup =LOW;
boolean currentGrinderRup =LOW;
boolean lastGrinderRdown =LOW;
boolean currentGrinderRdown =LOW;
boolean lastGrinderL =LOW;
boolean currentGrinderL =LOW;
boolean lastGrinderR =LOW;
boolean currentGrinderR =LOW;
boolean lastMenu =LOW;
boolean currentMenu = LOW;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};




void setup(){
  
   Wire.begin();
   RTC.begin();   
   Serial.begin(57600);

   pinMode(Relay, OUTPUT);  //COFFEE MACHINE
   pinMode(Relay2, OUTPUT); //GRINDER

   //------ INITIALIZING BOTH RELAYS IN THE OFF POSITION ------

   //COFFEE RELAY - HIGH LEVEL TRIGGER
   digitalWrite(Relay, LOW);

   //GRINDER RELAY - LOW LEVEL TRIGGER
   digitalWrite(Relay2, HIGH);
   
    //------ INITIALIZING THE LCD ------
    lcd.begin(20, 4);
    vari(); // program to initialize the display

    
    // ------ TIME ADJUSTMENT - UNCOMMENT IF NECESSARY ------
    //RTC.adjust(DateTime(2017,4,3,20,49,0));  
 
}




void loop(){
    
//--------Show current Time On LCD--------//

    DateTime now = RTC.now(); // Clock call
    now = RTC.now();

    clockup();  //program to print the time to the screen 


  //----Debounce  buttons---//

currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentCoffeeON = debounce(lastCoffeeON, COFFEE_ON);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentGrinderL = debounce(lastGrinderL, GRIND_L);
currentGrinderR = debounce(lastGrinderR, GRIND_R);
currentMenu = debounce(lastMenu, Menu);


if (notify==HIGH){

light=LOW;

if(flash ==1){
 lcd.backlight(); 
}

if(flash==100){
  lcd.noBacklight();
}

if (flash==200){
lcd.noBacklight();
flash=0;
}

 flash ++; 
}


// Auto shutoff for the coffee machine to turn off at 22h //

 

if(sleep!=0){
   if(now.hour() >=sleepTime) 
   {digitalWrite(Relay, LOW);
   sleep=0;
   }
}

if(now.hour() >= 0 && now.hour() <sleepTime ){
  sleep=1;
}



//****Backlight off from 22h-6h*********


  

 if (now.hour()>=22 or now.hour()<=6){
  light=LOW;
 }
 else{
  light=HIGH;
 }
 
 if (light==LOW){
  lcd.noBacklight();
  }
 else if (light==HIGH && notify==LOW){
    lcd.backlight();
  }
  



//------ TIMER ON/OFF ------//



if(lastTimerSwitch ==LOW && currentTimerSwitch ==HIGH){
  
  if(Timer==LOW){
  Timer=HIGH;
  }
  
  else if (Timer==HIGH){
    Timer=LOW;
  }
}
lastTimerSwitch = currentTimerSwitch;




if(Timer==LOW){
    lcd.setCursor(1,1);
    lcd.print(" COFFEE TIMER OFF  ");
  }



if(Timer==HIGH && digitalRead(Relay)==LOW){
//  Printing the timer on time

lcd.setCursor(1,1);
lcd.print("COFFEE ON AT ");


if(now.dayOfTheWeek()==6){
  

if(set_onwHH<10){
lcd.print("0");
}

lcd.print(set_onwHH);
lcd.print(":");

if(set_onwMM<10){
lcd.print("0");
}

lcd.print(set_onwMM);
lcd.print(" ");

}

else if(now.dayOfTheWeek()==0) {

  if(now.hour() >= set_onwHH && now.minute() > set_onwMM){
  
if(set_onHH<10){
lcd.print("0");
}

lcd.print(set_onHH);
lcd.print(":");

if(set_onMM<10){
lcd.print("0");
}

lcd.print(set_onMM);
lcd.print(" ");
 }

else {
  
if(set_onwHH<10){
lcd.print("0");
}

lcd.print(set_onwHH);
lcd.print(":");

if(set_onwMM<10){
lcd.print("0");
}

lcd.print(set_onwMM);
lcd.print(" ");
  
}
}


  
else if(now.dayOfTheWeek()==5) {

if(now.hour() >= set_onHH && now.minute() > set_onMM){
  
if(set_onwHH<10){
lcd.print("0");
}

lcd.print(set_onwHH);
lcd.print(":");

if(set_onwMM<10){
lcd.print("0");
}

lcd.print(set_onwMM);
lcd.print(" ");

}

else{
if(set_onHH<10){
lcd.print("0");
}

lcd.print(set_onHH);
lcd.print(":");

if(set_onMM<10){
lcd.print("0");
}

lcd.print(set_onMM);
lcd.print(" ");
}

}
  
  

else {
  
if(set_onHH<10){
lcd.print("0");
}

lcd.print(set_onHH);
lcd.print(":");

if(set_onMM<10){
lcd.print("0");
}

lcd.print(set_onMM);
lcd.print(" ");
  
}
}


if(Timer==HIGH && digitalRead(Relay)==HIGH){
//  Printing the timer off time

lcd.setCursor(1,1);
lcd.print("COFFEE OFF AT ");

if(now.dayOfTheWeek()==0 or now.dayOfTheWeek()==6){
  
if(now.hour() <= set_offwHH && now.minute() < set_offwMM){

if(set_offwHH<10){
lcd.print("0");
}

lcd.print(set_offwHH);
lcd.print(":");

if(set_offwMM<10){
lcd.print("0");
}

lcd.print(set_offwMM);

}

else{
  
if(sleepTime<10){
lcd.print("0");
}

lcd.print(sleepTime);
lcd.print(":00");

}


}

else {

if(now.hour() <= set_offHH && now.minute() < set_offMM){
  
if(set_offHH<10){
lcd.print("0");
}

lcd.print(set_offHH);
lcd.print(":");

if(set_offMM<10){
lcd.print("0");
}

lcd.print(set_offMM);
}

else{
  
if(sleepTime<10){
lcd.print("0");
}

lcd.print(sleepTime);
lcd.print(":00");

}
  
}

}

  //--------- Timer function-------//
  
      
   //---ON WEEKEND TIMER -------//

 if(Timer==HIGH){  
if(now.dayOfTheWeek()==0 or now.dayOfTheWeek()==6){

      if(now.hour() == set_onwHH){
      
             //if current hour matches then the minute is checked                
                      
                    if (now.minute() == set_onwMM ){
                                    
                         digitalWrite(Relay, HIGH);  
                     }
      }  
   // ------ OFF WEEKEND TIMER ------//

      if(now.hour() == set_offwHH){
        
               if (now.minute() == set_offwMM ){
                              
                   digitalWrite(Relay, LOW);  
               }
      }
}
  
// ------ WEEKDAY TIMER ------//    
else {    

    //------ ON TIMER------//
    
    if(now.hour() == set_onHH){

       //if current hour matches then the minute is checked                
              
              if (now.minute() == set_onMM ){
                              
                   digitalWrite(Relay, HIGH);  
              }
    }
    // ------ OFF TIMER ------//
     
    if(now.hour() == set_offHH){

       //if current hour matches then the minute is checked                
              
              if (now.minute() == set_offMM ){
                              
                   digitalWrite(Relay, LOW);  
              }

    }
}
 }     

// ------ Coffee Machine Power Button Relay Control ------//

if(lastCoffeeON ==LOW && currentCoffeeON ==HIGH){
  boolean state = digitalRead(Relay);

  if (state == LOW){
  digitalWrite(Relay, HIGH);
  delay(150);
  }

  else if (state == HIGH){
    digitalWrite(Relay, LOW);
    delay(150);
  }
  else{
    digitalWrite(Relay, LOW);
  }
}
lastCoffeeON = currentCoffeeON;




/*
 * Here starts the code for the grinder. starting with the toggles for the left and right buttons on the grinder, controlling the up and down independantly.
 * 
 */ 


// ------- Turn down the left grind time ------ //

if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH)
{

    if(set_L>0)
    {    //"ON" Set point  down never lower than 0
     set_L--;        
    }
    else{
      set_L=0;
      
    }
}




// ------ Turn up the left grind time ------ //
if (lastGrinderLup== LOW && currentGrinderLup == HIGH)
{
 
      if(set_L<99){  
      set_L++;
      }
    else{
      set_L=99;
    }
}



lastGrinderLup=currentGrinderLup;
lastGrinderLdown=currentGrinderLdown;



//---Print the set L time---//

lcd.setCursor(7,3);
if (set_L>0){
if(set_L < 10){   
  lcd.print("0");
}
lcd.print(set_L);
}
else if (set_L==0){
  lcd.print("ON");
}


//-----Turn down the right grind time-----//

if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH)
{

    if(set_R>0)
    {    //"ON" Set point  down never lower than 0
     set_R--;        
    }
    else{
      set_R=0;
    }
}




//----Turn up the right grind time-----//

if (lastGrinderRup== LOW && currentGrinderRup == HIGH)
{
      if(set_R<99){  
      set_R++;
      }
    else{
      set_R=99;
    }
    
}


lastGrinderRdown=currentGrinderRdown;
lastGrinderRup=currentGrinderRup;

lcd.setCursor(16,3);
if(set_R>0){
  
    if(set_R < 10){   
    lcd.print("0");
    }
lcd.print(set_R);
}

else if (set_R==0){
  lcd.print("ON"); //if set point is zero that the button will work as a push and hold to operate
}


//---- code that executes upon the press of the left grinder button ---- //


if (lastGrinderL==LOW && currentGrinderL==HIGH)
{
       boolean state = digitalRead(Relay2);
  
      if(set_L>0){
          if (state==HIGH){
              digitalWrite(Relay2, LOW);
              grindRunL =sec();
              
          }

            else {
              digitalWrite(Relay2, HIGH);
              grindRunL =0;
          }

          }
      
      
      else if(set_L==0){
          digitalWrite(Relay2, LOW);
          delay(60);
          grindRunL =0;
      } 
}

if(lastGrinderL==HIGH && currentGrinderL==HIGH && set_L==0){
  digitalWrite(Relay2, LOW);
  delay(60);
}

if(lastGrinderL==HIGH && currentGrinderL==LOW && set_L==0 ){
  digitalWrite(Relay2, HIGH);
}

lastGrinderL=currentGrinderL;




// ---- code that executes upon the press of the right grinder button ---- //


if (lastGrinderR==LOW && currentGrinderR==HIGH)
{
       boolean state = digitalRead(Relay2);
  
      if(set_R>0){
          if (state==HIGH){
            
              digitalWrite(Relay2, LOW);
               grindRunR =sec();
          }
      
        else {
              digitalWrite(Relay2, HIGH);
              grindRunR =0;
          }
          }
      
      
      else if(set_R==0){
          digitalWrite(Relay2, LOW);
          delay(60);
          grindRunR =0;
      } 
}

if(lastGrinderR==HIGH && currentGrinderR==HIGH && set_R==0){
  digitalWrite(Relay2, LOW);
  delay(60);
}

if(lastGrinderR==HIGH && currentGrinderR==LOW && set_R==0 ){
  digitalWrite(Relay2, HIGH);
}

lastGrinderR=currentGrinderR;




if(grindRunL!=0){

if((sec()-grindRunL)>=(set_L)){
  digitalWrite(Relay2, HIGH);
  grindRunL=0;
}
if((sec()-grindRunL)<(set_L)){
  digitalWrite(Relay2, LOW);
}
}

if(grindRunR!=0){
  
  if((sec()-grindRunR)<(set_R)){
  digitalWrite(Relay2, LOW);

  
}

if((sec()-grindRunR)>=(set_R)){
  digitalWrite(Relay2, HIGH);
    grindRunR=0;
}
  
}



if(lastMenu ==LOW && currentMenu ==HIGH){
menu();
}

lastMenu = currentMenu;

//lcd.setCursor(0,3);
//lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);


}




void menu(){
  
  lcd.clear();
  
  vari2();

  delay(300);

  int menu=1;
  int select=1;
  
  lastdaylight=daylight;


  
while( menu==1 ){
    

while(select==1){

 
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);

  
  if(flash<=60){
    
        lcd.setCursor(4,1);
        
        if(set_onHH<10){
          
            lcd.print("0");
        
         }

       lcd.print(set_onHH);
       lcd.print(":");
       lcd.setCursor(7,1);

      if(set_onMM<10){
        
          lcd.print("0");
          
       }
      lcd.print(set_onMM);
  }

  
  
  if(flash==60){
    
      lcd.setCursor(4,1);
      lcd.print("     ");
      
  }
  
  if(flash==3200){
    
      lcd.setCursor(4,1);
      lcd.print("   ");
      flash=0;
  }
  
flash++;

    
//***************** setting the on weekday time ***********************
 
      if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
       
               if(set_onHH>0)
                  {    //"ON" Set point  down never negative.
                   set_onHH--;        
                  }
                  else{
                    set_onHH=23;
                      //below zero the time will loop back to 23.
                  }
            
      }
      
      
      
      if (lastGrinderLup== LOW && currentGrinderLup == HIGH){
      
               if(set_onHH<23){   //"ON" Set point up never higher than 23
                    set_onHH++;
                    }
                  else{
                    set_onHH=0;
                    //above 23 the time will loop back to zero.
                 }
          
      }
      
lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;



        if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
        
              if(set_onMM>0){    //"ON" Set point  down never lower than 0
                  set_onMM--;        
                  }
                  else{
                  set_onMM=59;
                }
         
        }



        if (lastGrinderRup== LOW && currentGrinderRup == HIGH){
        
              if(set_onMM<59){   //"ON" Set point up never higher than 59
                  
                  set_onMM++;
                  
                  }
                else{
                  
               set_onMM=0;
               
               }
            
        }


lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 



        if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
        {

             lcd.setCursor(4,1);
                    
                    if(set_onHH<10){
                      
                        lcd.print("0");
                    
                     }
            
                   lcd.print(set_onHH);
                   lcd.print(":");
                   lcd.setCursor(7,1);
            
                  if(set_onMM<10){
                    
                      lcd.print("0");
                      
                   }
                  lcd.print(set_onMM);

                flash=0;
                select++;
  
        }

        

lastTimerSwitch=currentTimerSwitch;



//************** Button to exit the menu ***************************



    if (lastMenu== LOW && currentMenu == HIGH)
    {
        lcd.clear();
        select=0;
        flash=0;
        vari();
        menu++;     
    }

lastMenu=currentMenu;

}






while(select==2){

 
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);

  
  if(flash<=60){
    
         lcd.setCursor(14,1);
         
         if(set_offHH<10){
            lcd.print("0");
          }
      
          lcd.print(set_offHH);
          lcd.print(":");
          
          if(set_offMM<10){
              lcd.print("0");
          }
      
          lcd.print(set_offMM);
  
    }



    if(flash==60){
      
          lcd.setCursor(14,1);
          lcd.print("     ");
    
    }

    
  if(flash==3200){
    
        lcd.setCursor(14,1);
        lcd.print("     ");
        flash=0;
  }
  
flash++;

    
//***************** setting the off weekday time ***********************



    if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
     
         if(set_offHH>0){    
          
             set_offHH--; 
                    
            }
            
            else{
              
              set_offHH=23;
                
            }        
        }



      if (lastGrinderLup== LOW && currentGrinderLup == HIGH){
      
               if(set_offHH<23){   //"ON" Set point up never higher than 23
                    set_offHH++;
                    
                    }
                    
                  else{
                    
                    set_offHH=0;
                   
                  }
          
      }


 
lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;



      if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
      
               if(set_offMM>0){ 
                    set_offMM--;        
                }
                  
                else{
                  set_offMM=59;
                }
      }
      
      
      
      if (lastGrinderRup== LOW && currentGrinderRup == HIGH){
      
              if(set_offMM<59){   
                
                  set_offMM++;
                  }
                  
                else{
                  
               set_offMM=0;
                }
                
      }




lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 

  


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
    lcd.setCursor(14,1);
         
      if(set_offHH<10){
            lcd.print("0");
          }
      
      lcd.print(set_offHH);
      lcd.print(":");
          
      if(set_offMM<10){
              lcd.print("0");
          }
      
    lcd.print(set_offMM);
    flash=0;      
   select++;    
}

lastTimerSwitch=currentTimerSwitch;


//************** Button to exit the menu ***************************

if (lastMenu== LOW && currentMenu == HIGH)
{
    lcd.clear();
   select=0;
   flash=0;
    vari();
   menu++;   
     
}

lastMenu=currentMenu;

}


while(select==3){

 
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);

  
  if(flash<=60){
    
        lcd.setCursor(4,3);
        
        if(set_onwHH<10){
          
            lcd.print("0");
        
         }

       lcd.print(set_onwHH);
       lcd.print(":");
       lcd.setCursor(7,3);

      if(set_onwMM<10){
        
          lcd.print("0");
          
       }
      lcd.print(set_onwMM);
  }

  
  
  if(flash==60){
    
      lcd.setCursor(4,3);
      lcd.print("     ");
      
  }
  
  if(flash==3200){
    
      lcd.setCursor(4,3);
      lcd.print("   ");
      flash=0;
  }
  
flash++;

    
//***************** setting the on weekday time ***********************
 
      if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
       
               if(set_onwHH>0)
                  {    //"ON" Set point  down never negative.
                   set_onwHH--;        
                  }
                  else{
                    set_onwHH=23;
                      //below zero the time will loop back to 23.
                  }
            
      }
      
      
      
      if (lastGrinderLup== LOW && currentGrinderLup == HIGH){
      
               if(set_onwHH<23){   //"ON" Set point up never higher than 23
                    set_onwHH++;
                    }
                  else{
                    set_onwHH=0;
                    //above 23 the time will loop back to zero.
                 }
          
      }
 
lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;



        if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
        
              if(set_onwMM>0){    //"ON" Set point  down never lower than 0
                  set_onwMM--;        
                  }
                  else{
                  set_onwMM=59;
                }
         
        }



        if (lastGrinderRup== LOW && currentGrinderRup == HIGH){
        
              if(set_onwMM<59){   //"ON" Set point up never higher than 59
                  
                  set_onwMM++;
                  
                  }
                else{
                  
               set_onwMM=0;
               
               }
            
        }




lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 



        if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
        {

             lcd.setCursor(4,3);
                    
                    if(set_onwHH<10){
                      
                        lcd.print("0");
                    
                     }
            
                   lcd.print(set_onwHH);
                   lcd.print(":");
                   lcd.setCursor(7,3);
            
                  if(set_onwMM<10){
                    
                      lcd.print("0");
                      
                   }
                  lcd.print(set_onwMM);

                flash=0;
                select++;
  
        }

        

lastTimerSwitch=currentTimerSwitch;



//************** Button to exit the menu ***************************



    if (lastMenu== LOW && currentMenu == HIGH)
    {
        lcd.clear();
        select=0;
        flash=0;
        vari();
        menu++;     
    }

lastMenu=currentMenu;

}






while(select==4){


 
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);

  
  if(flash<=60){
    
         lcd.setCursor(14,3);
         
         if(set_offwHH<10){
            lcd.print("0");
          }
      
          lcd.print(set_offwHH);
          lcd.print(":");
          
          if(set_offwMM<10){
              lcd.print("0");
          }
      
          lcd.print(set_offwMM);
  
    }



    if(flash==60){
      
          lcd.setCursor(14,3);
          lcd.print("     ");
    
    }

    
  if(flash==3200){
    
        lcd.setCursor(14,3);
        lcd.print("     ");
        flash=0;
  }
  
flash++;

    
//***************** setting the off weekday time ***********************



    if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
     
         if(set_offwHH>0){    
          
             set_offwHH--; 
                    
            }
            
            else{
              
              set_offwHH=23;
                
            }        
        }



      if (lastGrinderLup== LOW && currentGrinderLup == HIGH){
      
               if(set_offwHH<23){   //"ON" Set point up never higher than 23
                    set_offwHH++;
                    
                    }
                    
                  else{
                    
                    set_offwHH=0;
                   
                  }
          
      }


 
lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;



      if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
      
               if(set_offwMM>0){ 
                    set_offwMM--;        
                }
                  
                else{
                  set_offwMM=59;
                }
      }
      
      
      
      if (lastGrinderRup== LOW && currentGrinderRup == HIGH){
      
              if(set_offwMM<59){   
                
                  set_offwMM++;
                  }
                  
                else{
                  
               set_offwMM=0;
                }
                
      }




lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 

  


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
    lcd.setCursor(14,3);
         
      if(set_offwHH<10){
            lcd.print("0");
          }
      
      lcd.print(set_offwHH);
      lcd.print(":");
          
      if(set_offwMM<10){
              lcd.print("0");
          }
      
    lcd.print(set_offwMM);
   flash=0; 
   lcd.clear();   
   vari3();   
   select++;    
}

lastTimerSwitch=currentTimerSwitch;


//************** Button to exit the menu ***************************

if (lastMenu== LOW && currentMenu == HIGH)
{
    lcd.clear();
   select=0;
   flash=0;
    vari();
   menu++;   
     
}

lastMenu=currentMenu;

}


//----Setting the Day----//

while(select==5){

  DateTime now = RTC.now(); // Clock call
  now = RTC.now();
  
        currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
        currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
        currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
        currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
        currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
        currentMenu = debounce(lastMenu, Menu);


  
  if(flash<= 60){
    
   lcd.setCursor(10,1);
   
      if (now.day()<10){
        
      lcd.print("0");
      }
      
  lcd.print(now.day());
 
  }
  
  if(flash==100){
    
  lcd.setCursor(10,1);
  lcd.print("  ");
  
  }
  
  if(flash==800){
  lcd.setCursor(10,1);
  lcd.print("  ");
  flash=0;
  }
  
flash++;


if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){

int dy=now.day();
int mn=now.month();
int yr=now.year();

if (mn==6 || mn==9 || mn==11 || mn==4){

if(dy > 1){
  dy--;
}

else if(dy == 1){
  dy=30;
}
  
}

else if (mn==2){

  if(dy > 1){
  dy--;
}

else if(dy == 1){
  dy=28;
}
  
}

else if(mn==1 || mn==3 || mn==5 || mn==7 || mn==8 || mn==10 || mn==12){


  if(dy > 1){
  dy--;
}

else if(dy == 1){
  dy=31;
}

}

dateadj(dy,mn,yr);


}



if (lastGrinderRup== LOW && currentGrinderRup == HIGH){


int dy=now.day();
int mn=now.month();
int yr=now.year();

if (mn==6 || mn==9 || mn==11 || mn==4){

if (dy<30){
  dy++;
 
}

else if(dy==30){
  dy=1;
}
  
}

else if (mn==2){

  if(dy<28){
    dy=28;
  }

  else if(dy==28){
    dy=1;
  }
  
}

else if(mn==1 || mn==3 || mn==5 || mn==7 || mn==8 || mn==10 || mn==12){

if(dy<31){
  dy++;
}
else if(dy==31){
  dy==1;
}
  
}

dateadj(dy,mn,yr);

}

lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
     lcd.setCursor(10,1);
   
      if (now.day()<10){
        
      lcd.print("0");
      }
      
  lcd.print(now.day());
    flash=0;      
   select++;    
}

lastTimerSwitch=currentTimerSwitch;

//************** Button to exit the menu ***************************


if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();
    

   menu++;    
}

lastMenu=currentMenu;


}




//----Setting the Month------//
while(select==6){

DateTime now = RTC.now(); // Clock call
now = RTC.now();
  
      currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
      currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
      currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
      currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
      currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
      currentMenu = debounce(lastMenu, Menu);




  
  if(flash<= 60){
    
    lcd.setCursor(13,1);
    
    if (now.month()<10){
  lcd.print("0");
}
  lcd.print(now.month());
 
  }
  
  if(flash==100){
    
  lcd.setCursor(13,1);
  lcd.print("  ");
  
  }
  
  if(flash==800){
  lcd.setCursor(13,1);
  lcd.print("  ");
  flash=0;
  }
  
flash++;

if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){

int dy=now.day();
int mn=now.month();
int yr=now.year();

if(mn>1){
  mn--;
}
else if(mn==1){
  mn=12;
}


 dateadj(dy,mn,yr); 


}
if (lastGrinderRup== LOW && currentGrinderRup == HIGH){

int dy=now.day();
int mn=now.month();
int yr=now.year();

if(mn<12){
  mn++;
}

else if(mn==12){
  mn=1;
}

 dateadj(dy,mn,yr); 

}

lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
    lcd.setCursor(13,1);
    
    if (now.month()<10){
  lcd.print("0");
}
  lcd.print(now.month());
    flash=0;      
   select++;    
}

lastTimerSwitch=currentTimerSwitch;

//************** Button to exit the menu ***************************


if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();
    

   menu++;    
}

lastMenu=currentMenu;


}




//-----Setting the Year----//

while(select==7){

DateTime now = RTC.now(); // Clock call
now = RTC.now();
  
        currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
        currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
        currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
        currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
        currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
        currentMenu = debounce(lastMenu, Menu);




  
  if(flash<= 60){
    
    lcd.setCursor(16,1);
    lcd.print(now.year());
  }
  
  if(flash==100){
    
  lcd.setCursor(16,1);
  lcd.print("    ");
  
  }
  
  if(flash==800){
  lcd.setCursor(16,1);
  lcd.print("    ");
  flash=0;
  }
  
flash++;



if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
  
int dy=now.day();
int mn=now.month();
int yr=now.year();

yr --;

dateadj(dy,mn,yr); 

}
if (lastGrinderRup== LOW && currentGrinderRup == HIGH){

int dy=now.day();
int mn=now.month();
int yr=now.year();

yr ++;

dateadj(dy,mn,yr);
  
}

lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 



if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
     lcd.setCursor(16,1);
    lcd.print(now.year());
    
    flash=0;      
   select++;    
}

lastTimerSwitch=currentTimerSwitch;




//************** Button to exit the menu ***************************


if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();
    

   menu++;    
}

lastMenu=currentMenu;

}


//******* Adjusting the clock**********
    
while(select==8){


  

  DateTime now = RTC.now(); // Clock call
now = RTC.now();
  
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);


  
  if(flash<= 60){
    
    lcd.setCursor(10,2);
    
    if(now.hour() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.hour(), DEC); //Print hour
    lcd.print(':');
    
    if(now.minute() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.minute(), DEC);
 
  }
  
  if(flash==100){
    
  lcd.setCursor(10,2);
  lcd.print("     ");
  
  }
  
  if(flash==160){
  lcd.setCursor(10,2);
  lcd.print("     ");
  flash=0;
  }
  
flash++;

  
    
//***************** setting the clock ***********************
  
if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
  
  int hr=now.hour();
  int mn=now.minute();
  
    if(now.hour()>=1){
        hr --;
        clockadj(hr,mn);
        
    
  
    }

    else{
        hr=0;
        clockadj(hr,mn);
      
    }

    lcd.setCursor(10,2);
   if(now.hour() < 10)
        {
        lcd.print("0");
        }
        lcd.print(now.hour(), DEC); //Print hour
}



if (lastGrinderLup== LOW && currentGrinderLup == HIGH){

int hr=now.hour();
  int mn=now.minute();
  
 if(now.hour()<=23){
  
     hr++;
    clockadj(hr,mn); 
   
  
    }

    else{
      hr=24;
        clockadj(hr,mn);
       
    }

    lcd.setCursor(10,2);
   if(now.hour() < 10)
        {
        lcd.print("0");
        }
        lcd.print(now.hour(), DEC); //Print hour
}


 
lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;


if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){
  
int hr=now.hour();
  int mn=now.minute();
  
 if(now.minute()>=1){
    mn--;
    clockadj(hr,mn);
      
           
      }

    else {
        mn=0;
        clockadj(hr,mn);     
    }

     lcd.setCursor(13,2);           
    if(now.minute() < 10)
    {
     lcd.print("0");
    }
    lcd.print(now.minute(), DEC); 

}


if (lastGrinderRup== LOW && currentGrinderRup == HIGH){

  int hr=now.hour();
  int mn=now.minute();


      if(now.minute()<=59){
        mn++;
        clockadj(hr,mn);
       
      }

    else {
       mn=60;  
         clockadj(hr,mn);     
    }

     lcd.setCursor(13,2);           
    if(now.minute() < 10)
    {
     lcd.print("0");
    }
    lcd.print(now.minute(), DEC); 
                
}



lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup; 


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH)
{
   lcd.setCursor(10,2);
    
    if(now.hour() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.hour(), DEC); //Print hour
    lcd.print(':');
    
    if(now.minute() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.minute(), DEC);

    flash=0;
    select++;    
}

lastTimerSwitch=currentTimerSwitch;



//************** Button to exit the menu ***************************



if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();
   menu++;     
}

lastMenu=currentMenu;
}


while(select==9){

  DateTime now = RTC.now(); // Clock call
  now = RTC.now();
  
currentTimerSwitch = debounce(lastTimerSwitch, TIMER_SWITCH);
currentGrinderLup = debounce(lastGrinderLup, GRIND_L_UP);
currentGrinderLdown = debounce(lastGrinderLdown, GRIND_L_DOWN);
currentGrinderRup = debounce(lastGrinderRup, GRIND_R_UP);
currentGrinderRdown = debounce(lastGrinderRdown, GRIND_R_DOWN);
currentMenu = debounce(lastMenu, Menu);
 
  if(flash<=60){
  lcd.setCursor(17,3);

if (daylight==LOW){
  lcd.print("OFF");

}
else if(daylight=HIGH){
  lcd.print(" ON");
}
  }
  
  if(flash> 60 && flash<100){
  lcd.setCursor(17,3);
  lcd.print("   ");
  }
  if(flash==100){
  lcd.setCursor(17,3);
  lcd.print("   ");
  flash=0;
  }
  
flash++;
  
//***************** setting the on weekday time ***********************
  
if (lastGrinderLdown== LOW && currentGrinderLdown == HIGH){
  
 if(daylight==LOW){
  daylight=HIGH;
 }
 
 else if(daylight=HIGH){
  daylight=LOW;
 }

}



if (lastGrinderLup== LOW && currentGrinderLup == HIGH){

 if(daylight==LOW){
  daylight=HIGH;
 }
 
 else if(daylight=HIGH){
  daylight=LOW;
 }
    
}



lastGrinderLdown = currentGrinderLdown;
lastGrinderLup = currentGrinderLup;


if (lastGrinderRdown== LOW && currentGrinderRdown == HIGH){

  if(daylight==LOW){
  daylight=HIGH;
 }
 
 else {
  daylight=LOW;
 }
 
}



if (lastGrinderRup== LOW && currentGrinderRup == HIGH){

 if(daylight==LOW){
  daylight=HIGH;
 }
 
 else {
  daylight=LOW;
 } 
 
}


lastGrinderRdown = currentGrinderRdown;
lastGrinderRup = currentGrinderRup;


if (lastTimerSwitch== LOW && currentTimerSwitch == HIGH){
  
lcd.setCursor(17,3);

if (daylight==LOW){
  lcd.print("OFF");

}
else if(daylight==HIGH){
  lcd.print(" ON");
}

if(lastdaylight != daylight){
 int hr=now.hour();
  int mn=now.minute();

  if(daylight==LOW){
    hr--;
 clockadj(hr,mn);
 lastdaylight=daylight;
  DateTime now = RTC.now(); // Clock call
  now = RTC.now();
  }

  else if (daylight==HIGH){

    hr++;
     clockadj(hr,mn);
     lastdaylight=daylight;
  DateTime now = RTC.now(); // Clock call
  now = RTC.now();
  
  }

  }
   flash=0;
   lcd.clear();
   vari2();
   select=1;   
    
}

lastTimerSwitch=currentTimerSwitch;

//************** Button to exit the menu ***************************

if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();

  int hr=now.hour();
  int mn=now.minute();
   
if(lastdaylight != daylight){

  if(daylight=LOW){
    hr--;
 clockadj(hr,mn);
 lastdaylight=daylight;
  DateTime now = RTC.now(); // Clock call
  now = RTC.now();
  }

  else if (daylight=HIGH){

    hr++;
     clockadj(hr,mn);
  lastdaylight=daylight;
  DateTime now = RTC.now(); 
  now = RTC.now();
  }

  }
  
   menu++;     
}

lastMenu=currentMenu;
}




//************** Button to exit the menu ***************************


if (lastMenu== LOW && currentMenu == HIGH)
{
   lcd.clear();
   select=0;
   flash=0;
    vari();
    

   menu++;    
}

lastMenu=currentMenu;
  
}
}



void vari(){
  // this function initializes the main display page
  
  lcd.clear();
   lcd.setCursor(10,0);
    lcd.print("WATER:");
    lcd.setCursor(6,2);
    lcd.print("GRINDER");
    lcd.setCursor(2,3);
    lcd.print("LEFT:");
    lcd.setCursor(10,3);
    lcd.print("RIGHT:");

    //erase below once water sensor is configured
    lcd.setCursor(16,0);
    lcd.print("100%");
}

void vari2(){
lcd.setCursor(0,0);

lcd.print("    MONDAY-FRIDAY   ");

lcd.setCursor(1,1);
lcd.print("ON:");

if (set_onHH<10){
  lcd.print("0");
}
lcd.print(set_onHH);
lcd.print(":");
if (set_onMM<10){
  lcd.print("0");
}
lcd.print(set_onMM);
lcd.setCursor(10,1);

lcd.print("OFF:");
if (set_offHH<10){
  lcd.print("0");
}
lcd.print(set_offHH);
lcd.print(":");
if (set_offMM<10){
  lcd.print("0");
}
lcd.print(set_offMM);

lcd.setCursor(0,2);

lcd.print("  SATURDAY-SUNDAY   ");

lcd.setCursor(1,3);

lcd.print("ON:");
if (set_onwHH<10){
  lcd.print("0");
}
lcd.print(set_onwHH);
lcd.print(":");
if (set_onwMM<10){
  lcd.print("0");
}
lcd.print(set_onwMM);
lcd.setCursor(10,3);
lcd.print("OFF:");
if (set_offwHH<10){
  lcd.print("0");
}
lcd.print(set_offwHH);
lcd.print(":");
if (set_offwMM<10){
  lcd.print("0");
}
lcd.print(set_offwMM);



}


void vari3(){
  
   DateTime now = RTC.now(); // Clock call
  now = RTC.now();

  lcd.setCursor(0,0);
  lcd.print("   Date and Time");
  lcd.setCursor(0,1);
 if(now.dayOfTheWeek() <= 2 || now.dayOfTheWeek()==5){
  lcd.print("  ");
 }

 
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.setCursor(10,1);
  if (now.day()<10){
  lcd.print("0");
}
  lcd.print(now.day());
  lcd.print("/");
  if (now.month()<10){
  lcd.print("0");
}
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
  lcd.setCursor(0,2);
  lcd.print("Set Time:");
 lcd.setCursor(10,2);
    
    if(now.hour() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.hour(), DEC); //Print hour
    lcd.print(':');
    
    if(now.minute() < 10)
    {
    lcd.print("0");
    }
    lcd.print(now.minute(), DEC);
  lcd.setCursor(0,3);
  lcd.print("Daylight Savings");
  lcd.setCursor(17,3);

if (daylight==LOW){
  lcd.print("OFF");

}
else if(daylight==HIGH){
  lcd.print(" ON");
}
 
  
  
}

unsigned long sec(){
  unsigned long sec;
  sec=millis()/1000;
  return sec;
  
}

void clockadj(int hr, int mn){
   RTC.begin(); 
    DateTime now = RTC.now();
    now = RTC.now();
    RTC.adjust(DateTime(now.year(),now.month(),now.day(),hr,mn,now.second()));
   
  }

void dateadj(int dy, int mn, int yr){

   RTC.begin(); 
    DateTime now = RTC.now();
    now = RTC.now();
    RTC.adjust(DateTime(yr,mn,dy,now.hour(),now.minute(),now.second()));
  
}


void clockup(){
  DateTime now = RTC.now(); // Clock call


now = RTC.now();

lcd.setCursor(0,0); 
                
if(now.hour() < 10)
{
lcd.print("0");
}
lcd.print(now.hour(), DEC); //Print hour
lcd.print(':');

if(now.minute() < 10)
{
lcd.print("0");
}
lcd.print(now.minute(), DEC); //Print min
lcd.print(':');

if(now.second() < 10){
lcd.print("0");
}
lcd.print(now.second(), DEC);
}

   //----Debouncing function for all buttons----//
boolean debounce(boolean last, int pin)
    {
    boolean current = digitalRead(pin);
        if (last != current)
        {
        delay(5);
        current = digitalRead(pin);
        }
    return current;


    }


