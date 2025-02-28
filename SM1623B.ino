/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where can do changes and look to the results     */
/****************************************************/
//
/*************************************************************/
/*ATT:The model SM1623B use two Bytes to fill memory map.    */
/*************************************************************/
//The SM1623B support 7 grids of 14 segments, but the panel need
//10 grids, by this reason they swapp the grids by segments and change
// common pin Anode/Cathode to allow this. 
//Note they do the count from 1 to 14 and 1 to 7, not use "0" as normal.
//But they use "0" to define memory address at table of memory!!!

//set your clock speed
#define F_CPU 16000000UL
//these are the include files. They are outside the project folder
// #include <avr/io.h>
// #include <util/delay.h>
// #include <avr/interrupt.h>

// Standard Input/Output functions 1284
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/atomic.h>
#include <stdbool.h>
#include "stdint.h"
#include <Arduino.h>

#define SM1623B_in 7   // If 0 write LCD, if 1 read of LCD
#define SM1623B_clk 8  // if 0 is a command, if 1 is a data0
#define SM1623B_stb 9  // Must be pulsed to LCD fetch data of bus

#define AdjustPins    PIND // before is C, but I'm use port C to VFC Controle signals

unsigned char DigitTo7SegEncoder(unsigned char digit, unsigned char common);

/*Global Variables Declarations*/
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char minute = 0;
unsigned char secs=0;
unsigned char seconds=0;
unsigned char milisec = 0;

unsigned char memory_secs=0;
unsigned char memory_minutes=0;

unsigned char number;
unsigned char secsU =0x00;
unsigned char numberA1 =0x00;
unsigned char secsD =0x00;
unsigned char numberB1 =0x00;
unsigned char minutU =0x00;
unsigned char numberC1 =0x00;
unsigned char minutD =0x00;
unsigned char numberD1 =0x00;
unsigned char hourU =0x00;
unsigned char numberE1 =0x00;
unsigned char hourD =0x00;
unsigned char numberF1 =0x00;

unsigned char digit=0;
unsigned char grid=0;
unsigned char gridSegments = 0b00000011; // Here I define the number of GRIDs and Segments I'm using
unsigned char wheelGrid = 0x00;

boolean flag=true;
boolean flagSecs=false; // Used in case of using buttons to setting clock, set buttons to do it!

unsigned int segOR[14];  // 7*2 result as 14 positions of map memory to fill table(0b1100xxxx) where x is to address of one grid in normal use!!!
unsigned int wd0 =0x00;
unsigned int wd1 =0x00;
unsigned char segmWheels = 0x00;

//Array to run the Wheels
unsigned int wheels[]={'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}; // Att: The sequence is already like the wheels is contructed!
//Arrays of bits to Digits
unsigned int segNum0[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000001), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "e" of all digits  // Symbol Play
         (0b0000000000000001), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int segNum1[] ={
  //1        (hgfedcba) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol Repeat // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000000), // "f" of all digits// Symbol Pause
         (0b0000000000000000)  // "g" of all digits // Symbol Wheel
};
unsigned int segNum2[] ={         
  //2        (7654321) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "e" of all digits// Symbol Play
         (0b0000000000000000), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum3[] ={
  //3        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000000), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum4[] ={
  //4        (57643210) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000001), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum5[] ={
  //5        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000001), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum6[] ={
  //6        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000001), // "d" of all digits // Symbol PBC
         (0b0000000000000001), // "e" of all digits// Symbol Play
         (0b0000000000000001), // "f" of all digits// Symbol Pause
         (0b0000000000000001) // "g" of all digits // Symbol Wheel
};
unsigned int segNum7[] ={
  //7        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel // Symbol MP3
         (0b0000000000000000), // "d" of all digits // Symbol PBC
         (0b0000000000000000), // "e" of all digits// Symbol Play
         (0b0000000000000000), // "f" of all digits// Symbol Pause
         (0b0000000000000000) // "g" of all digits // Symbol Wheel
};
unsigned int segNum8[] ={
  //8        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000001), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "e" of all digits  // Symbol Play
         (0b0000000000000001), // "f" of all digits  // Symbol Pause
         (0b0000000000000001)  // "g" of all digits  // Symbol Wheel
};
unsigned int segNum9[] ={
  //9        (57643210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000001), // "f" of all digits  // Symbol Pause
         (0b0000000000000001)  // "g" of all digits  // Symbol Wheel
 };
//Array of bits to Wheels
unsigned int WheelsA[] ={
  //0        (76543210) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000001), // "b" of all digits and symbol wheel   // Symbol VCD & Wheel 1
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsB[] ={
  //0      (76543210) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000001), // "f" of all digits  // Symbol Pause & Whell 2
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsC[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000001)  // "g" of all digits  // Symbol Wheel 3
};
unsigned int WheelsD[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000001), // "d" of all digits  // Symbol PBC & Whell 4
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsE[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000001), // "c" of all digits and symbol wheel   // Symbol MP3 & Wheel 5
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play 
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsF[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000001), // "e" of all digits  // Symbol Play & Wheel 6
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
unsigned int WheelsG[] ={
  //0         (7654321) // Position of GRIDs
         (0b0000000000000000), // "a" of all digits and symbol repeat  // Symbol DVD
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000010)  // "g" of all digits  // Symbol Wheel 7
};
unsigned int WheelsH[] ={
  //0      (76543210) // Position of GRIDs
         (0b0000000000000001), // "a" of all digits and symbol repeat  // Symbol DVD & Wheel 8
         (0b0000000000000000), // "b" of all digits and symbol wheel   // Symbol VCD
         (0b0000000000000000), // "c" of all digits and symbol wheel   // Symbol MP3
         (0b0000000000000000), // "d" of all digits  // Symbol PBC
         (0b0000000000000000), // "e" of all digits  // Symbol Play
         (0b0000000000000000), // "f" of all digits  // Symbol Pause
         (0b0000000000000000)  // "g" of all digits  // Symbol Wheel
};
void setup() {
    // put your setup code here, to run once:

    // initialize digital pin LED_BUILTIN as an output.
      pinMode(LED_BUILTIN, OUTPUT);
      Serial.begin(115200);
      seconds = 0x00;
      minutes =0x00;
      hours = 0x00;

      /*CS12  CS11 CS10 DESCRIPTION
      0        0     0  Timer/Counter1 Disabled 
      0        0     1  No Prescaling
      0        1     0  Clock / 8
      0        1     1  Clock / 64
      1        0     0  Clock / 256
      1        0     1  Clock / 1024
      1        1     0  External clock source on T1 pin, Clock on Falling edge
      1        1     1  External clock source on T1 pin, Clock on rising edge
    */
      // initialize timer1 
      cli();           // disable all interrupts
      //initialize timer1 
      //noInterrupts();    // disable all interrupts, same as CLI();
      TCCR1A = 0;
      TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
      
      TCNT1  = 0;
      
      // Use 62499 to generate a cycle of 1 sex 2 X 0.5 Secs (16MHz / (2*256*(1+62449) = 0.5
      OCR1A = 62498;            // compare match register 16MHz/256/2Hz
      //OCR1A = 1200; // only to use in test, increment seconds more fast!
      TCCR1B |= (1 << WGM12);   // CTC mode
      TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

    // Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
    // a little the value 62499 upper or lower if the clock have a delay or advance on hours.
      
    //  a=0x33;
    //  b=0x01;

    CLKPR=(0x80);
    //Set PORT
    DDRD = 0xFF;  // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
    PORTD=0x00;
    DDRB =0xFF;
    PORTB =0x00;

    SM1623B_init();

    clear_SM1623B();

    //only here I active the enable of interrupts to allow run the test of SM1623B
    //interrupts();             // enable all interrupts, is same as sei();
    sei();
}
void SM1623B_init(void){
  delayMicroseconds(200); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure SM1623B display (grids)
  cmd_with_stb(gridSegments); // cmd 1 // SM1623B is driver until 7 grids
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
}
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
         for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
           digitalWrite(SM1623B_clk, LOW);
                 if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(SM1623B_in, HIGH);
                 }
                 else{ //if bitwise and resolves to false
                   digitalWrite(SM1623B_in, LOW);
                 }
          delayMicroseconds(5);
          digitalWrite(SM1623B_clk, HIGH);
          delayMicroseconds(5);
         }
   //digitalWrite(SM1623B_clk, LOW);
}
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(SM1623B_stb, LOW);
  delayMicroseconds(1);
         for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
           digitalWrite(SM1623B_clk, LOW);
           delayMicroseconds(1);
                 if (data & mask){ // if bitwise AND resolves to true
                    digitalWrite(SM1623B_in, HIGH);
                 }
                 else{ //if bitwise and resolves to false
                   digitalWrite(SM1623B_in, LOW);
                 }
          digitalWrite(SM1623B_clk, HIGH);
          delayMicroseconds(1);
         }
   digitalWrite(SM1623B_stb, HIGH);
   delayMicroseconds(1);
}
void test_WheelsOFF(void){
  //where: 
  //Command 1: Display Mode Setting Command //With Strobe impulse
  //Command 2: Data Setting Command         //With Strobe impulse
  //Command 3: Address Setting Command      //Without Strobe impulse
    digitalWrite(SM1623B_stb, LOW);
    delayMicroseconds(1);
    cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a drive of 7 grids
    cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(SM1623B_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            //
                            cmd_without_stb(0b00000000); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000000); // Symbol DVD
                            
                            cmd_without_stb(0b00000000); // "b" of all digits and symbol wheel 
                            cmd_without_stb(0b00000000); // Symbol VCD
                            
                            cmd_without_stb(0b00000000); // "c" of all digits and symbol wheel
                            cmd_without_stb(0b00000000); // Symbol MP3
                            
                            cmd_without_stb(0b00000000); // "d" of all digits 
                            cmd_without_stb(0b00000000); // Symbol PBC
                            
                            cmd_without_stb(0b00000000); // "e" of all digits
                            cmd_without_stb(0b00000000); // Symbol Play
                            
                            cmd_without_stb(0b00000000); // "f" of all digits
                            cmd_without_stb(0b00000000); // Symbol Pause
                            
                            cmd_without_stb(0b00000000); // "g" of all digits
                            cmd_without_stb(0b00000000); // Symbol Wheel
                            digitalWrite(SM1623B_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(200);
}
void test_WheelsON(void){
  //where: 
  //Command 1: Display Mode Setting Command //With Strobe impulse
  //Command 2: Data Setting Command         //With Strobe impulse
  //Command 3: Address Setting Command      //Without Strobe impulse
    digitalWrite(SM1623B_stb, LOW);
    delayMicroseconds(1);
    cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a drive of 7 grids and 14 segments
    cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(SM1623B_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            //
                            cmd_without_stb(0b00000000); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000010); // Symbol DVD & Wheel 8
                            
                            cmd_without_stb(0b00000000); // "b" of all digits and symbol wheel 
                            cmd_without_stb(0b00000010); // Symbol VCD & Whell 1
                            
                            cmd_without_stb(0b00000000); // "c" of all digits and symbol wheel
                            cmd_without_stb(0b00000010); // Symbol MP3 & Wheel 5
                            
                            cmd_without_stb(0b00000000); // "d" of all digits 
                            cmd_without_stb(0b00000010); // Symbol PBC & Wheel 4
                            
                            cmd_without_stb(0b00000000); // "e" of all digits
                            cmd_without_stb(0b00000010); // Symbol Play & Wheel 6
                            
                            cmd_without_stb(0b00000000); // "f" of all digits
                            cmd_without_stb(0b00000010); // Symbol Pause & Wheel 2
                            
                            cmd_without_stb(0b00000000); // "g" of all digits
                            cmd_without_stb(0b00000110); // Symbol Wheel 3 & 7
                            digitalWrite(SM1623B_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(200);
}
void test_AllON(void){
  //where: 
  //Command 1: Display Mode Setting Command //With Strobe impulse
  //Command 2: Data Setting Command         //With Strobe impulse
  //Command 3: Address Setting Command      //Without Strobe impulse
    digitalWrite(SM1623B_stb, LOW);
    delayMicroseconds(1);
    cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a drive of 7 grids, here is used as 9 grids because they revert grids/segments
    cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
             //
                          digitalWrite(SM1623B_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                            // by this reason I need send 7 blocks of 9 segments, but the total result equal in memory fields!
                            //Grid position;  76543210
                            //Grid position:        98 This result as 9 grids or digit, because they swap grids/segments.
                            cmd_without_stb(0b11111111); // "a" of all digits and symbol repeat
                            cmd_without_stb(0b00000111); // Symbol DVD & whell 8
                            //First two       .:
                            cmd_without_stb(0b11111111); // "b" of all digits and symbol wheel and also the ":"
                            cmd_without_stb(0b00000111); // Symbol VCD & whell 1
                            //First two       .:
                            cmd_without_stb(0b11111111); // "c" of all digits and symbol wheel and also the ":"
                            cmd_without_stb(0b00000111); // Symbol MP3 & whell 5
                            
                            cmd_without_stb(0b11111111); // "d" of all digits 
                            cmd_without_stb(0b00000111); // Symbol PBC & whell 4
                            
                            cmd_without_stb(0b11111111); // "e" of all digits
                            cmd_without_stb(0b00000111); // Symbol Play & wheel 6
                            
                            cmd_without_stb(0b11111111); // "f" of all digits
                            cmd_without_stb(0b00000111); // Symbol Pause & wheel 2
                            
                            cmd_without_stb(0b11111111); // "g" of all digits
                            cmd_without_stb(0b00000111); // Symbol Wheel 3 & 7

                            digitalWrite(SM1623B_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(200);
}
void test_GRID(void){
  //where: 
  //Command 1: Display Mode Setting Command //With Strobe impulse
  //Command 2: Data Setting Command         //With Strobe impulse
  //Command 3: Address Setting Command      //Without Strobe impulse
  unsigned char word1 =0x00;
  unsigned char word0 =0x00;
              for (unsigned int i=512; i>0; i=(i>>1)){
                //Serial.println(i, HEX);
                digitalWrite(SM1623B_stb, LOW);
                delayMicroseconds(1);
                cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a drive of 7 grids
                cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
                           //                          
                          digitalWrite(SM1623B_stb, LOW);
                          delayMicroseconds(1);
                            cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this output pins with segments
                            //               (..gfedcba)  // Position of GRIDs swapped to gain more digits on a IC with only 7 digits, segments give more of 7 digits
                             word1 = (i & 0xff00UL) >>  8;
                             word0 = (i & 0x00ffUL) >>  0;
                             Serial.print("WD0= "); Serial.print(word0, HEX);
                             Serial.print(", WD1= ");Serial.println(word1, HEX);
                            cmd_without_stb(word0); // Group Low of bits of Digit
                            cmd_without_stb(word1); // Group High of bits of Digit
                            digitalWrite(SM1623B_stb, HIGH);
                            cmd_with_stb((0b10001000) | 7); //cmd 4
                            delay(800);
             }
}
void test_panel_DVD(void){
  for (int x=0; x< 10; x++){
    digit=x;
          //
          for (int i=0; i< 10;i++){
              delayMicroseconds(1);
              cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a driver of 7 grids
              cmd_with_stb(0b10000000);   // cmd 2 //Normal operation; Set pulse as 1/16
              digitalWrite(SM1623B_stb, LOW);
              delayMicroseconds(1);
              cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
            //                    
                                switch (i) {
                                  case 0: 
                                        for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum0[v]<<digit);
                                        }break;
                                  case 1: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum1[v]<<digit);
                                        }break;
                                  case 2:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum2[v]<<digit);
                                        } break;
                                  case 3: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum3[v]<<digit);
                                        }break;
                                  case 4: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum4[v]<<digit);
                                        }break;
                                  case 5:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum5[v]<<digit);
                                        }break;
                                  case 6: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum6[v]<<digit);
                                        }break;
                                  case 7: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum7[v]<<digit);
                                        }break;
                                  case 8: 
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum8[v]<<digit);
                                        }break;
                                  case 9:
                                  for(int v=0; v<14; v++){
                                          cmd_without_stb(segNum9[v]<<digit);
                                        } break;
                                  default:break;
                                }  
                  digitalWrite(SM1623B_stb, HIGH);
                  cmd_with_stb((0b10001000) | 7); //cmd 4
                 delay(100);
          }
  }
}
void write_panel_DVD( unsigned char digit, unsigned char grid){         
                                switch (digit) {
                                  case 0: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum0[v]<<grid));
                                        }break;
                                  case 1: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum1[v]<<grid));
                                        }break;
                                  case 2:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum2[v]<<grid));
                                        } break;
                                  case 3: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum3[v]<<grid));
                                        }break;
                                  case 4: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum4[v]<<grid));
                                        }break;
                                  case 5:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum5[v]<<grid));
                                        }break;
                                  case 6: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum6[v]<<grid));
                                        }break;
                                  case 7: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum7[v]<<grid));
                                        }break;
                                  case 8: 
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum8[v]<<grid));
                                        }break;
                                  case 9:
                                  for(int v=0; v<7; v++){
                                          segOR[v] = (segOR[v] |  (segNum9[v]<<grid));
                                        } break;
                                  default:break;
                                } 
                                if (grid == 1){  //This define wich RUN only one time by update, can be any number of grid(segmente on this solution) 0~9.
                                  //Serial.print(grid); Serial.println(" Grelha");
                                  delayMicroseconds(1);
                                  cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a driver of 7 grids
                                  cmd_with_stb(0b10000000); // cmd 2 //Normal operation; Set pulse as 1/16
                                  digitalWrite(SM1623B_stb, LOW);
                                  delayMicroseconds(1);
                                  cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                                        for(int v=0; v<7; v++){
                                           wd0 = (segOR[v] & 0x00ffUL) >>  0;
                                           wd1 = (segOR[v] & 0xff00UL) >>  8;
                                           //Serial.print("Wd0: "), Serial.print(wd0, BIN);Serial.print(", Wd1: "), Serial.println(wd1, BIN);
                                        cmd_without_stb(wd0); //Send all the table of data to the map of driver (00~0D)
                                        cmd_without_stb(wd1); //Send all the table of data to the map of driver (00~0D)
                                        //Serial.println(segOR[v], BIN);
                                        }
                                  digitalWrite(SM1623B_stb, HIGH);
                                  cmd_with_stb((0b10001000) | 7); //cmd 4
                                  delay(10);
                                        for(int v=0; v<14; v++){
                                        segOR[v]=0b00000000;  //Clear of contents of array table
                                        }
                                }
}
void clear_SM1623B(void){
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 14; n++){  // important be 10, if not, bright the half of wells./this on the SM1623B of 6 grids)
        //cmd_with_stb(gridSegments); // cmd 1 // SM1623B is fixed to 5 grids
        cmd_with_stb(0b01000000); //       cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(SM1623B_stb, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | n); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 7 displays of 7segments, 
            //
            //cmd_with_stb((0b10001000) | 7); //cmd 4 set the bright//On SM1628 let the segments "a" of some dsp ON, check!!!
            digitalWrite(SM1623B_stb, HIGH);
            delayMicroseconds(1);
     }
}
/************************** Begin Update Clock **************************/
void send_update_clock(void){
    if (secs >=60){
      secs =0;
      minutes++;
      minute++;
    }
    if (minutes >=60){
      minutes =0;
      minute =0;
      hours++;
    }
    if (hours >=24){
      hours =0;
    }
  
    //*************************************************************
    secsU=DigitTo7SegEncoder(secs%10);
    secsD=DigitTo7SegEncoder(secs/10);
    //*************************************************************
    minutU=DigitTo7SegEncoder(minute%10);
    minutD=DigitTo7SegEncoder(minute/10);
    //**************************************************************
    hourU=DigitTo7SegEncoder(hours%10);
    hourD=DigitTo7SegEncoder(hours/10);
    //**************************************************************
}
void updateDisplay(void){
  //Note: This sequence is to a panel with SM1623B and 6 DSP de 7segm e 1 de digito 1
  //wheels and 8 predefined symbols. ("1.30.2 DP903002R" on PCB panel)
  //Here we can define the number of digit, grid or digits is the number after comma!
  //Not forget, here in this panel, they swapped grid and segments!
    write_panel_DVD( secsU, 4);// 
    write_panel_DVD( secsD, 3);// 
    write_panel_DVD( minutU, 2);// 
    write_panel_DVD( minutD, 1);// 
    write_panel_DVD( hourU, 8);// 
    write_panel_DVD( hourD, 6);// 
}
unsigned char DigitTo7SegEncoder( unsigned char digit){
    // Note the array (segments[]) to draw the numbers is with 2 bytes!!! 20 chars, extract 2 by 2 the number you need. 
    switch(digit){
      case 0:   number=0;      break;  // if remove the LongX, need put here the segments[x]
      case 1:   number=1;      break;
      case 2:   number=2;      break;
      case 3:   number=3;      break;
      case 4:   number=4;      break;
      case 5:   number=5;      break;
      case 6:   number=6;      break;
      case 7:   number=7;      break;
      case 8:   number=8;      break;
      case 9:   number=9;      break;
    }
    return number;
} 
void adjustHMS(){
  // Important is necessary put a pull-up resistor to the VCC(+5VDC) to this pins (3, 4, 5)
  // if dont want adjust of the time comment the call of function on the loop
    /* Reset Seconds to 00 Pin number 3 Switch to GND*/
      if((AdjustPins & 0x08) == 0 )
      {
        _delay_ms(200);
        secs=00;
      }
      
      /* Set Minutes when SegCntrl Pin 4 Switch is Pressed*/
      if((AdjustPins & 0x10) == 0 )
      {
        _delay_ms(200);
        if(minutes < 59)
        minutes++;
        else
        minutes = 0;
      }
      /* Set Hours when SegCntrl Pin 5 Switch is Pressed*/
      if((AdjustPins & 0x20) == 0 )
      {
        _delay_ms(200);
        if(hours < 23)
        hours++;
        else
        hours = 0;
      }
}
void readButtons(){
  //Take special attention to the initialize digital pin LED_BUILTIN as an output.
  //
  int ledPin = 13;   // LED connected to digital pin 13
  unsigned int inPin = 7;     // pushbutton connected to digital pin 7
  unsigned int val = 0;       // variable to store the read value
  unsigned int dataIn=0;

  byte array[8] = {0,0,0,0,0,0,0,0};
  byte together = 0;

  unsigned char receive = 7; //define our transmit pin
  unsigned char data = 0; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask

  array[0] = 1;

  unsigned char btn1 = 0x41;

      digitalWrite(SM1623B_stb, LOW);
        delayMicroseconds(2);
      cmd_without_stb(0b01000010); // cmd 2 //10=Read Keys; 00=Wr DSP;
      delayMicroseconds(2);
      // cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 15H)
      // send without stb
  
  pinMode(7, INPUT_PULLUP);  // Important this point! Here I'm changing the direction of the pin to INPUT data.
  delayMicroseconds(2);
  //PORTD != B01010100; // this will set only the pins you want and leave the rest alone at
  //their current value (0 or 1), be careful setting an input pin though as you may turn 
  //on or off the pull up resistor  
  //This don't send the strobe signal, to be used in burst data send
         for (int z = 0; z < 4; z++){
             //for (mask=00000001; mask > 0; mask <<= 1) { //iterate through bit mask
                   for (int h =8; h > 0; h--) {
                      digitalWrite(SM1623B_clk, HIGH);  // Remember wich the read data happen when the clk go from LOW to HIGH! Reverse from write data to out.
                      delayMicroseconds(2);
                     val = digitalRead(inPin);
                      //digitalWrite(ledPin, val);    // sets the LED to the button's value
                           if (val & mask){ // if bitwise AND resolves to true
                             //Serial.print(val);
                            //data =data | (1 << mask);
                            array[h] = 1;
                           }
                           else{ //if bitwise and resolves to false
                            //Serial.print(val);
                           // data = data | (1 << mask);
                           array[h] = 0;
                           }
                    digitalWrite(SM1623B_clk, LOW);
                    delayMicroseconds(2);
                   } 
             
              Serial.print(z);  // All the lines of print is only used to debug, comment it, please!
              Serial.print(" - " );
                        
                                  for (int bits = 7 ; bits > -1; bits--) {
                                      Serial.print(array[bits]);
                                   }
                        
                        if (z==0){
                           if(array[4] == 1){
                             flagSecs = !flagSecs;  // This change the app to hours or seconds
                           }
                        }
                        
                        if (z==1){
                           if(array[7] == 1){
                              digitalWrite(2, !digitalRead(2));
                          }
                        }
                        
                        if (z==1){
                           if(array[4] == 1){
                             digitalWrite(3, !digitalRead(3));
                             //digitalWrite(SM1623B_onGreen, !digitalRead(SM1623B_onGreen));
                            }
                        } 
                          
                        if (z==2){
                           if(array[7] == 1){
                             digitalWrite(4, !digitalRead(4));
                             //digitalWrite(SM1623B_onGreen, !digitalRead(SM1623B_onGreen));
                            }
                        }                                      
                  Serial.println();
          }  // End of "for" of "z"
      Serial.println();  // This line is only used to debug, please comment it!

 digitalWrite(SM1623B_stb, HIGH);
 delayMicroseconds(2);
 cmd_with_stb((0b10001000) | 7); //cmd 4
 delayMicroseconds(2);
 pinMode(7, OUTPUT);  // Important this point! Here I'm changing the direction of the pin to OUTPUT data.
 delay(1); 
}
/************************** END  Update Clock **************************/
void write_Wheels(unsigned char segm, unsigned char grid){
  //Only to design the Wheels segments move!
                          switch (segm) {
                                  case 'a': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsA[v]<<grid));
                                        }break;
                                  case 'b': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsB[v]<<grid));
                                        }break;
                                  case 'c': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsC[v]<<grid));
                                        } break;
                                  case 'd': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsD[v]<<grid));
                                        }break;
                                  case 'e': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsE[v]<<grid));
                                        }break;
                                  case 'f': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsF[v]<<grid));
                                        }break;
                                  case 'g': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsG[v]<<grid));
                                        }break;
                                  case 'h': for(int v=0; v<8; v++){
                                          segOR[v] = (segOR[v] |  (WheelsH[v]<<grid));
                                        }break;            
                                  //
                                  default:break;
                                }
                                 
                                 if (grid == wheelGrid){  //This define wich RUN only one time by update, can be any number of grid(segmente on this solution) 0~9.
                                  cmd_with_stb(gridSegments); // cmd 1 // SM1623B is a driver of 7 grids
                                  cmd_with_stb(0b10000000); // cmd 2 //Normal operation; Set pulse as 1/16
                                  digitalWrite(SM1623B_stb, LOW);
                                  delayMicroseconds(1);
                                  cmd_without_stb(0b11000000); // Grids of display... they have done the swap of this pins with segments
                                        for(int v=0; v<7; v++){
                                           wd0 = (segOR[v] & 0x00ffUL) >>  0;
                                           wd1 = (segOR[v] & 0xff00UL) >>  8;
                                        //Serial.print("Wd0: "), Serial.print(wd0, BIN);Serial.print(", Wd1: "), Serial.println(wd1, BIN);
                                        cmd_without_stb(wd0); //Send all the table of data to the map of driver (00~0D)
                                        cmd_without_stb(wd1); //Send all the table of data to the map of driver (00~0D)
                                        //Serial.println(segOR[v], BIN);
                                        }
                                  digitalWrite(SM1623B_stb, HIGH);
                                  cmd_with_stb((0b10001000) | 7); //cmd 4
                                  delay(10);
                                        for(int v=0; v<14; v++){
                                        segOR[v]=0b00000000;  //Clear of contents of array table
                                        }
                  }
}
void runWeels(void){
  // Note the bits is already at segments position correct, by this reazon I move "0" the shift register!
    for(int w=0; w<6; w++){
      write_Wheels(wheels[w], wheelGrid);
      delay(60);
    }
}
void loop() {
  //You can comment untill while cycle to avoid the test running.
  wheelGrid = 0x09; //Here if tell to the code the position of grid of wheels. They don't use the "0" to count segments/digits
    for (int i=0; i<4; i++){
      test_WheelsON();
      delay(500);
      test_WheelsOFF();
      delay(500);
      clear_SM1623B();
      test_AllON();
      delay(1000);
      clear_SM1623B();
      delay(500); 
     }
     for (uint8_t i = 0x00; i < 36; i++){
      segmWheels++;
                if(segmWheels == 8){  
                segmWheels = 0;
                }
              // Is very important set same grid inside of function "write_Wheels([x], grid)", line where compare is done!
              write_Wheels(wheels[segmWheels], wheelGrid); // Grid 1 run over digit 5 from right count, grid 0 is first digit after wheel
              delay(100);
     }

  // I decide not use the dinamic refreshing, but you can do it!
       while(1){
                  if (flag=!flag){
                    //digitalWrite(panelLED,  !digitalRead(panelLED));//Only to control the LED is at left of Display. Comment if not use it!!!
                    send_update_clock();
                    delay(50);
                    readButtons();
                    delay(50);
                    updateDisplay();
                    delay(50);
                    segmWheels++;
                        if(segmWheels ==8){  
                        segmWheels = 0;
                        }
              // Is very important set same grid inside of function "write_Wheels([x], grid)", line where compare is done!
              // write_Wheels(wheels[segmWheels], wheelGrid); // Grid 1 run over digit 5 from right count, grid 0 is first digit after wheel
              // delay(10);
              // updateDisplay();
             }   
        }
}
ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
// https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
      secs++;
      flag = !flag;   
} 
