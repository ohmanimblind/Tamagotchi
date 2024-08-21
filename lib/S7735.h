#ifndef S7735_H
#define S7735_H
#include "avr/io.h"
#include "helper.h"

void PrepCommand(){
PORTD = SetBit(PORTD,7,0); //Set A0 -> command
PORTB = SetBit(PORTB,2,0); //CS -> Low
}

void PrepareData(){
PORTD = SetBit(PORTD,7,1); //Set A0 -> Data
//PORTB = SetBit(PORTB,2,0); //CS -> Low
}
//Function: Resets LCD Screen By toggling RS (Pin 8)
void HarwareReset(){
  PORTB = SetBit(PORTB,0,0);
  _delay_ms(200);
  PORTB = SetBit(PORTB,0,1);
  _delay_ms(200);
}
//Function: Initate S7735 Display 
//SWRESET -> SLPOUT -> COLDMOD -> (16-BIT MODE) -> DISPLAY ON
void S7735_init(){
    HarwareReset();
    PrepCommand();
    SPI_SEND(0x01); //SWRESET
    _delay_ms(150);
    PrepCommand();
    SPI_SEND(0x11); //SLPOUT
    _delay_ms(200);
    PrepCommand();
    SPI_SEND(0x3A); //COLMOD
    PrepareData();
    SPI_SEND(0x05); //16-Bit Mode
    _delay_ms(10);
    PrepCommand();
    SPI_SEND(0x29); //Display On
    _delay_ms(200);
    PrepCommand();
    SPI_SEND(0x36);
    PrepareData();
    SPI_SEND(0x00);

}

//White's Out Screen
void WhiteOut(){
    PrepCommand();
    SPI_SEND(0x2A); //CASET
    PrepareData();
    SPI_SEND(0x00); //First 4 Start , last 4 end
    SPI_SEND(0x00);
    SPI_SEND(0x00);
    SPI_SEND(0x81);
    _delay_ms(10);        //0000 007D

    PrepCommand();
    SPI_SEND(0x2B); //RASET
    PrepareData();
    SPI_SEND(0x00); //First 4 Start , last 4 end
    SPI_SEND(0x00);
    SPI_SEND(0x00);
    SPI_SEND(0x81);
    _delay_ms(10);

    PrepCommand();
    SPI_SEND(0x2C); //RAWRI
    PrepareData();
    for (unsigned int j = 0; j < 129 * 130; j++) { //varies 
        PrepareData();
        SPI_SEND(0xFFFF >> 8);    // Send high byte of color
        SPI_SEND(0xFFFF & 0xFF);  // Send low byte of color
    }
}
//Used to write by abstracted 32*16 pixle size
//Parameters: X-Position of Pixel, Y-Position of Pixel, Color wanted to print
int hex_vals_x[] = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80 };
  int hex_vals_y[] = { 
        0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 
        0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C, 
        0x40, 0x44, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 
        0x60, 0x64, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C, 
        0x80 
    };
//Used to write by abstracted 32*16 pixle size
//Parameters: X-Position of Pixel, Y-Position of Pixel, Color wanted to print
void WritePixel(int x_pos, int y_pos, int color_val){
    //Col_SET
    PrepCommand();
    SPI_SEND(0x2A);
    PrepareData();
    SPI_SEND(0x00) ;
    SPI_SEND(hex_vals_x[x_pos])  ; //XS
    SPI_SEND(0x00);
    SPI_SEND(hex_vals_x[x_pos+1]);  //XE
   // _delay_ms(10);
    //Row Set
    PrepCommand();
    SPI_SEND(0x2B);
    PrepareData();
    SPI_SEND(0x00) ;
    SPI_SEND(hex_vals_x[y_pos])   ;//XS
    SPI_SEND(0x00);
    SPI_SEND(hex_vals_x[y_pos+1])  ;//XE
  //  _delay_ms(10);
    PrepCommand();
    SPI_SEND(0x2C); //RAWRI
    PrepareData();
    for (unsigned int j = 0; j <  1000; j++) { //varies 
        PrepareData();
        SPI_SEND(color_val >> 8);    // Send high byte of color
        SPI_SEND(color_val & 0xFF);  // Send low byte of color
    }
}

unsigned short fixBits(unsigned short n) {
    // Mask to extract the first 5 bits (16-bit number)
    unsigned short first5_mask = 0xF800; // 1111100000000000
    unsigned short last5_mask = 0x001F;  // 0000000000011111

    // Extract the first 5 bits
    unsigned short first5_bits = (n & first5_mask) >> 11; // Right shift to align with the last 5 bits
    // Extract the last 5 bits
    unsigned short last5_bits = n & last5_mask;

    // Clear the first 5 bits and the last 5 bits in the original number
    n &= ~first5_mask; // Clear the first 5 bits
    n &= ~last5_mask;  // Clear the last 5 bits

    // Place the extracted first 5 bits into the last 5 bits position
    n |= first5_bits;

    // Place the extracted last 5 bits into the first 5 bits position
    n |= (last5_bits << 11);

    return n;
}

void PrintMisa(){
//Row 1
WritePixel(1,0,fixBits(0xf756));
WritePixel(2,0,fixBits(0xf756));
WritePixel(5,0,fixBits(0xe690));
WritePixel(6,0,fixBits(0xe690));
WritePixel(7,0,fixBits(0xe690));
WritePixel(8,0,fixBits(0xe690));
WritePixel(9,0,fixBits(0xe690));
WritePixel(10,0,fixBits(0xe690));
WritePixel(13,0,fixBits(0xf756));
WritePixel(14,0,fixBits(0xf756));
//Row2
WritePixel(0,1,fixBits(0xf756));
WritePixel(1,1,fixBits(0xf756));
WritePixel(2,1,fixBits(0xc58b));
WritePixel(3,1,fixBits(0x91c3));
WritePixel(4,1,fixBits(0xe690));
WritePixel(5,1,fixBits(0xe690));
WritePixel(6,1,fixBits(0xf756));
WritePixel(7,1,fixBits(0xf756));
WritePixel(8,1,fixBits(0xf756));
WritePixel(9,1,fixBits(0xf756));
WritePixel(10,1,fixBits(0xe690));
WritePixel(11,1,fixBits(0xe690));
WritePixel(12,1,fixBits(0x91c3));
WritePixel(13,1,fixBits(0xc58b));
WritePixel(14,1,fixBits(0xf756));
WritePixel(15,1,fixBits(0xf756));
//Row 3
WritePixel(0,2,fixBits(0xf756));
WritePixel(1,2,fixBits(0xf712));
WritePixel(2,2,fixBits(0x91c3));
WritePixel(3,2,fixBits(0xe690));
WritePixel(4,2,fixBits(0xe690));
WritePixel(5,2,fixBits(0xe690));
WritePixel(6,2,fixBits(0xe690));
WritePixel(7,2,fixBits(0xf756));
WritePixel(8,2,fixBits(0xf756));
WritePixel(9,2,fixBits(0xe690));
WritePixel(10,2,fixBits(0xe690));
WritePixel(11,2,fixBits(0xe690));
WritePixel(12,2,fixBits(0xe690));
WritePixel(13,2,fixBits(0x91c3));
WritePixel(14,2,fixBits(0xf712));
WritePixel(15,2,fixBits(0xf756));
//Row 4
WritePixel(0,3,fixBits(0xf712));
WritePixel(1,3,fixBits(0xc58b));
WritePixel(2,3,fixBits(0xac08));
WritePixel(3,3,fixBits(0xf756));
WritePixel(4,3,fixBits(0xf756));
WritePixel(5,3,fixBits(0xe690));
WritePixel(6,3,fixBits(0xbd51));
WritePixel(7,3,fixBits(0xe690));
WritePixel(8,3,fixBits(0xe690));
WritePixel(9,3,fixBits(0xbd51));
WritePixel(10,3,fixBits(0xe690));
WritePixel(11,3,fixBits(0xf756));
WritePixel(12,3,fixBits(0xf756));
WritePixel(13,3,fixBits(0xac08));
WritePixel(14,3,fixBits(0xc58b));
WritePixel(15,3,fixBits(0xf712));
//Row 5
WritePixel(0,4,fixBits(0xf712));
WritePixel(2,4,fixBits(0xc58b));
WritePixel(3,4,fixBits(0xe690));
WritePixel(4,4,fixBits(0xf756));
WritePixel(5,4,fixBits(0xbd51));
WritePixel(6,4,fixBits(0xffdd));
WritePixel(7,4,fixBits(0xbd51));
WritePixel(8,4,fixBits(0xbd51));
WritePixel(9,4,fixBits(0xffdd));
WritePixel(10,4,fixBits(0xbd51));
WritePixel(11,4,fixBits(0xf756));
WritePixel(12,4,fixBits(0xe690));
WritePixel(13,4,fixBits(0xc58b));
WritePixel(15,4,fixBits(0xf712));
//Row 6
WritePixel(2,5,fixBits(0xf712));
WritePixel(3,5,fixBits(0xc58b));
WritePixel(4,5,fixBits(0xe690));
WritePixel(5,5,fixBits(0xffdd));
WritePixel(6,5,fixBits(0x0000));
WritePixel(7,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));
WritePixel(9,5,fixBits(0x0000));
WritePixel(10,5,fixBits(0xffdd));
WritePixel(11,5,fixBits(0xe690));
WritePixel(12,5,fixBits(0xc58b));
WritePixel(12,5,fixBits(0xf712));
//Row 7
WritePixel(2,6,fixBits(0xf712));
WritePixel(3,6,fixBits(0xc58b));
WritePixel(4,6,fixBits(0xbd51));
WritePixel(5,6,fixBits(0x0000));
WritePixel(6,6,fixBits(0xd443));
WritePixel(7,6,fixBits(0xffdd));
WritePixel(8,6,fixBits(0xffdd));
WritePixel(9,6,fixBits(0xd443));
WritePixel(10,6,fixBits(0x0000));
WritePixel(11,6,fixBits(0xbd51));
WritePixel(12,6,fixBits(0xc58b));
WritePixel(13,6,fixBits(0xf712));
//Row 8
WritePixel(3,7,fixBits(0xf712));
WritePixel(4,7,fixBits(0xc58b));
WritePixel(5,7,fixBits(0xffdd));
WritePixel(6,7,fixBits(0xffdd));
WritePixel(7,7,fixBits(0xffdd));
WritePixel(8,7,fixBits(0xffdd));
WritePixel(9,7,fixBits(0xffdd));
WritePixel(10,7,fixBits(0xffdd));
WritePixel(11,7,fixBits(0xc58b));
WritePixel(12,7,fixBits(0xf712));

//Row 9
WritePixel(3,8,fixBits(0xac08));
WritePixel(4,8,fixBits(0xf712));
WritePixel(5,8,fixBits(0xbd51));
WritePixel(6,8,fixBits(0x0000));
WritePixel(7,8,fixBits(0xbd51));
WritePixel(8,8,fixBits(0xbd51));
WritePixel(9,8,fixBits(0x0000));
WritePixel(10,8,fixBits(0xbd51));
WritePixel(11,8,fixBits(0xf712));
WritePixel(12,8,fixBits(0xac08));

//Row 10
WritePixel(3,9,fixBits(0xac08));
WritePixel(4,9,fixBits(0xac08));
WritePixel(5,9,fixBits(0xffdd));
WritePixel(6,9,fixBits(0x2900));
WritePixel(7,9,fixBits(0xffdd));
WritePixel(8,9,fixBits(0xffdd));
WritePixel(9,9,fixBits(0x2900));
WritePixel(10,9,fixBits(0xffdd));
WritePixel(11,9,fixBits(0xac08));
WritePixel(12,9,fixBits(0xac08));
//Row 11
WritePixel(4,10,fixBits(0xac08));
WritePixel(5,10,fixBits(0x6b6c));
WritePixel(6,10,fixBits(0x2900));
WritePixel(7,10,fixBits(0x2900));
WritePixel(8,10,fixBits(0x2900));
WritePixel(9,10,fixBits(0x2900));
WritePixel(10,10,fixBits(0x6b6c));
WritePixel(11,10,fixBits(0xac08));
//Row 12
WritePixel(4,11,fixBits(0xffdd));
WritePixel(5,11,fixBits(0x6b6c));
WritePixel(6,11,fixBits(0x0000));
WritePixel(7,11,fixBits(0x2900));
WritePixel(8,11,fixBits(0x2900));
WritePixel(9,11,fixBits(0x0000));
WritePixel(10,11,fixBits(0x6b6c));
WritePixel(11,11,fixBits(0xffdd));
//Row 13
WritePixel(5,12,fixBits(0x000));
WritePixel(6,12,fixBits(0x000));
WritePixel(7,12,fixBits(0x000));
WritePixel(8,12,fixBits(0x000));
WritePixel(9,12,fixBits(0x000));
//Row 14
WritePixel(6,13,fixBits(0xbd72));
WritePixel(9,13,fixBits(0xbd72));
//Row 15
WritePixel(6,14,fixBits(0x0000));
WritePixel(9,14,fixBits(0x0000));
}

void MisaIdle1(){
WritePixel(9,14,fixBits(0xffff));
WritePixel(9,13,fixBits(0x0000));
WritePixel(6,14,fixBits(0000));
WritePixel(6,13,fixBits(0xbd72));

WritePixel(5,9,fixBits(0x6b6c));
WritePixel(10,9,fixBits(0x6b6c));
WritePixel(5,11,fixBits(0xffff));
WritePixel(10,11,fixBits(0x0fff));
}
void MisaIdle2(){
WritePixel(9,14,fixBits(0x0000));
WritePixel(9,13,fixBits(0xbd72));
WritePixel(6,14,fixBits(0xffff));
WritePixel(6,13,fixBits(0x0000));

WritePixel(5,9,fixBits(0xffdd));
WritePixel(10,9,fixBits(0xffdd));
WritePixel(5,11,fixBits(0x6b6c));
WritePixel(10,11,fixBits(0x6b6c));

}

void PrintSelect1(){
WritePixel(2,3,fixBits(0x000));
WritePixel(2,4,fixBits(0x000));
WritePixel(2,5,fixBits(0x000));
WritePixel(2,6,fixBits(0x000));
WritePixel(2,7,fixBits(0x000));
WritePixel(3,4,fixBits(0x000));
WritePixel(3,5,fixBits(0x000));
WritePixel(3,6,fixBits(0x000));
WritePixel(4,5,fixBits(0x000));

WritePixel(2,8,fixBits(0xFFFF));
WritePixel(2,9,fixBits(0xFFFF));
WritePixel(2,10,fixBits(0xFFFF));
WritePixel(2,11,fixBits(0xFFFF));
WritePixel(2,12,fixBits(0xFFFF));
WritePixel(3,9,fixBits(0xFFFF));
WritePixel(3,10,fixBits(0xFFFF));
WritePixel(3,11,fixBits(0xFFFF));
WritePixel(4,10,fixBits(0xFFFF));

}



//Function: Prints Apple Sprite
//Parameters: X and Y cord. Specifies stem.
//Sprite is 4 * 3 in size
void PrintApple(int x_pos, int y_pos){
WritePixel(x_pos-1,y_pos + 2,fixBits(0xb1c6));
WritePixel(x_pos-1,y_pos + 3,fixBits(0xb1c6));
WritePixel(x_pos-1,y_pos + 1,fixBits(0xe186));
WritePixel(x_pos,y_pos + 3,fixBits(0xb1c6));
WritePixel(x_pos,y_pos + 2,fixBits(0xe186));
WritePixel(x_pos,y_pos,fixBits(0x4dc9)); //Stem
WritePixel(x_pos,y_pos + 1,fixBits(0xeb4d));
WritePixel(x_pos+1,y_pos + 3,fixBits(0xe186));
WritePixel(x_pos+1,y_pos +1,fixBits(0xeb4d));
WritePixel(x_pos+1,y_pos + 2,fixBits(0xeb4d));

}

void BiteApple(int x_pos, int y_pos){

WritePixel(x_pos + 1, y_pos +2, fixBits(0xFFFF));

}
//Function: Clears out Apple sprite with white
//Parameters: x and y, should be from stem of last prite
void ClearApple(int x_pos, int y_pos){
WritePixel(x_pos-1,y_pos + 2,fixBits(0xFFFF));
WritePixel(x_pos-1,y_pos + 3,fixBits(0xFFFF));
WritePixel(x_pos-1,y_pos + 1,fixBits(0xFFFF));
WritePixel(x_pos,y_pos + 3,fixBits(0xFFFF));
WritePixel(x_pos,y_pos + 2,fixBits(0xFFFF));
WritePixel(x_pos,y_pos,fixBits(0xFFFF)); //Stem
WritePixel(x_pos,y_pos + 1,fixBits(0xFFFF));
WritePixel(x_pos+1,y_pos + 3,fixBits(0xFFFF));
WritePixel(x_pos+1,y_pos +1,fixBits(0xFFFF));
WritePixel(x_pos+1,y_pos + 2,fixBits(0xFFFF));

}


//Function: Prints Cake at X Y pos
//Sprite is 5 * 6, starts at cherry, 3 left 2 right
void PrintCake(int x_pos, int y_pos){
    WritePixel(x_pos - 2,y_pos + 3,fixBits(0xf7b8));
    WritePixel(x_pos -1,y_pos + 3,fixBits(0xf7b8));
    WritePixel(x_pos,y_pos + 3,fixBits(0xf698));
    WritePixel(x_pos,y_pos + 2,fixBits(0xf698));
    WritePixel(x_pos,y_pos + 1,fixBits(0xf698));
    WritePixel(x_pos -1,y_pos + 1,fixBits(0xf698));
    WritePixel(x_pos - 2,y_pos + 1,fixBits(0xf698));
    WritePixel(x_pos - 3,y_pos + 1,fixBits(0xf698));
    WritePixel(x_pos,y_pos,fixBits(0xf180)); //Cherry
    WritePixel(x_pos,y_pos + 2,fixBits(0xf180));
    WritePixel(x_pos - 2,y_pos + 2,fixBits(0xf180));
    
    //Plate
    WritePixel(x_pos - 4,y_pos + 4,fixBits(0xbdb6));
    WritePixel(x_pos - 2,y_pos + 4,fixBits(0xbdb6));
    WritePixel(x_pos - 3,y_pos + 4,fixBits(0xbdb6));
    WritePixel(x_pos - 2,y_pos + 5,fixBits(0xbdb6));
    WritePixel(x_pos - 1,y_pos + 5,fixBits(0xbdb6));
    WritePixel(x_pos - 1,y_pos + 4,fixBits(0xe6fb));
    WritePixel(x_pos,y_pos + 4,fixBits(0xe6fb));
    WritePixel(x_pos + 1,y_pos+4,fixBits(0xe6fb));
    WritePixel(x_pos,y_pos + 5,fixBits(0xe6fb));
}

void EatCake(int x_pos, int y_pos){
    WritePixel(x_pos - 2,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos -1,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 2,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos -1,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos - 3,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos,fixBits(0xFFFF)); //Cherry
    WritePixel(x_pos,y_pos + 2,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 2,fixBits(0xFFFF));
}

void ClearCake(int x_pos, int y_pos){
    WritePixel(x_pos - 2,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos -1,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 3,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 2,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos -1,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos - 3,y_pos + 1,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos,fixBits(0xFFFF)); //Cherry
    WritePixel(x_pos,y_pos + 2,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 2,fixBits(0xFFFF));
    
    //Plate
    WritePixel(x_pos - 4,y_pos + 4,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 4,fixBits(0xFFFF));
    WritePixel(x_pos - 3,y_pos + 4,fixBits(0xFFFF));
    WritePixel(x_pos - 2,y_pos + 5,fixBits(0xFFFF));
    WritePixel(x_pos - 1,y_pos + 5,fixBits(0xFFFF));
    WritePixel(x_pos - 1,y_pos + 4,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 4,fixBits(0xFFFF));
    WritePixel(x_pos + 1,y_pos+4,fixBits(0xFFFF));
    WritePixel(x_pos,y_pos + 5,fixBits(0xFFFF));
}

void PrintMisaHappy(){

//Row 6 Fix

WritePixel(5,5,fixBits(0x0000));
WritePixel(6,5,fixBits(0xffdd));
WritePixel(7,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));
WritePixel(9,5,fixBits(0xffdd));
WritePixel(10,5,fixBits(0x0000));

//Row 7 fix
WritePixel(4,6,fixBits(0x0000));
WritePixel(5,6,fixBits(0xffdd));
WritePixel(6,6,fixBits(0x0000));

WritePixel(9,6,fixBits(0x0000));
WritePixel(10,6,fixBits(0xffdd));
WritePixel(11,6,fixBits(0x0000));

}
//Function: Prints angry face over Misa
//Notes: Use after MisaPrint
void PrintMisaAngry(){

WritePixel(6,6,fixBits(0x0000));
WritePixel(9,6,fixBits(0x0000));
WritePixel(6,5,fixBits(0xffdd));
WritePixel(9,5,fixBits(0xffdd));
WritePixel(6,7,fixBits(0x4ade));
WritePixel(9,7,fixBits(0x4ade));
WritePixel(6,8,fixBits(0x4ade));
WritePixel(9,8,fixBits(0x4ade));

}
void PrintSelect2(){

WritePixel(2,8,fixBits(0x000));
WritePixel(2,9,fixBits(0x000));
WritePixel(2,10,fixBits(0x000));
WritePixel(2,11,fixBits(0x000));
WritePixel(2,12,fixBits(0x000));

WritePixel(3,9,fixBits(0x000));
WritePixel(3,10,fixBits(0x000));
WritePixel(3,11,fixBits(0x000));

WritePixel(4,10,fixBits(0x000));

WritePixel(2,3,fixBits(0xFFFF));
WritePixel(2,4,fixBits(0xFFFF));
WritePixel(2,5,fixBits(0xFFFF));
WritePixel(2,6,fixBits(0xFFFF));
WritePixel(2,7,fixBits(0xFFFF));
WritePixel(3,4,fixBits(0xFFFF));
WritePixel(3,5,fixBits(0xFFFF));
WritePixel(3,6,fixBits(0xFFFF));
WritePixel(4,5,fixBits(0xFFFF));


}
//5*4
void PrintBasket(int x_pos, int y_pos){
WritePixel(x_pos - 2, y_pos + 3, fixBits(0xacc8));
WritePixel(x_pos + 2, y_pos + 3, fixBits(0xacc8));
WritePixel(x_pos, y_pos, fixBits(0xacc8));
WritePixel(x_pos + 1, y_pos, fixBits(0xacc8));
WritePixel(x_pos-1, y_pos, fixBits(0xacc8));

WritePixel(x_pos - 2, y_pos + 2, fixBits(0xbd6b));
WritePixel(x_pos + 2, y_pos + 2, fixBits(0xbd6b));
WritePixel(x_pos -1 , y_pos + 3, fixBits(0xbd6b));
WritePixel(x_pos  , y_pos + 3, fixBits(0xbd6b));
WritePixel(x_pos +1 , y_pos + 3, fixBits(0xbd6b));

WritePixel(x_pos - 2, y_pos + 1, fixBits(0xc5ae));
WritePixel(x_pos + 2, y_pos + 1, fixBits(0xc5ae));
WritePixel(x_pos -1 , y_pos + 2, fixBits(0xc5ae));
WritePixel(x_pos  , y_pos + 2, fixBits(0xc5ae));
WritePixel(x_pos +1 , y_pos + 2, fixBits(0xc5ae));

}

void ClearBasket(int x_pos, int y_pos){
    WritePixel(x_pos - 2, y_pos + 3, fixBits(0xFFFF));
WritePixel(x_pos + 2, y_pos + 3, fixBits(0xFFFF));
WritePixel(x_pos, y_pos, fixBits(0xFFFF));
WritePixel(x_pos + 1, y_pos, fixBits(0xFFFF));
WritePixel(x_pos-1, y_pos, fixBits(0xFFFF));

WritePixel(x_pos - 2, y_pos + 2, fixBits(0xFFFF));
WritePixel(x_pos + 2, y_pos + 2, fixBits(0xFFFF));
WritePixel(x_pos -1 , y_pos + 3, fixBits(0xFFFF));
WritePixel(x_pos  , y_pos + 3, fixBits(0xFFFF));
WritePixel(x_pos +1 , y_pos + 3, fixBits(0xFFFF));

WritePixel(x_pos - 2, y_pos + 1, fixBits(0xFFFF));
WritePixel(x_pos + 2, y_pos + 1, fixBits(0xFFFF));
WritePixel(x_pos -1 , y_pos + 2, fixBits(0xFFFF));
WritePixel(x_pos  , y_pos + 2, fixBits(0xFFFF));
WritePixel(x_pos +1 , y_pos + 2, fixBits(0xFFFF));
}

void ClearSelect1and2(){

WritePixel(2,3,fixBits(0xFFFF));
WritePixel(2,4,fixBits(0xFFFF));
WritePixel(2,5,fixBits(0xFFFF));
WritePixel(2,6,fixBits(0xFFFF));
WritePixel(2,7,fixBits(0xFFFF));
WritePixel(3,4,fixBits(0xFFFF));
WritePixel(3,5,fixBits(0xFFFF));
WritePixel(3,6,fixBits(0xFFFF));
WritePixel(4,5,fixBits(0xFFFF));

WritePixel(2,8,fixBits(0xFFFF));
WritePixel(2,9,fixBits(0xFFFF));
WritePixel(2,10,fixBits(0xFFFF));
WritePixel(2,11,fixBits(0xFFFF));
WritePixel(2,12,fixBits(0xFFFF));
WritePixel(3,9,fixBits(0xFFFF));
WritePixel(3,10,fixBits(0xFFFF));
WritePixel(3,11,fixBits(0xFFFF));
WritePixel(4,10,fixBits(0xFFFF));

}

void PrintA(){
WritePixel(5,3,fixBits(0x497a));
WritePixel(5,4,fixBits(0x497a));
WritePixel(5,5,fixBits(0x497a));
WritePixel(5,6,fixBits(0x497a));
WritePixel(5,7,fixBits(0x497a));
WritePixel(5,8,fixBits(0x497a));
WritePixel(5,9,fixBits(0x497a));
WritePixel(5,10,fixBits(0x497a));
WritePixel(5,11,fixBits(0x497a));
WritePixel(5,12,fixBits(0x497a));

WritePixel(6,3,fixBits(0x497a));
WritePixel(7,3,fixBits(0x497a));
WritePixel(8,3,fixBits(0x497a));
WritePixel(9,3,fixBits(0x497a));
WritePixel(10,3,fixBits(0x497a));

WritePixel(10,3,fixBits(0x497a));
WritePixel(10,4,fixBits(0x497a));
WritePixel(10,5,fixBits(0x497a));
WritePixel(10,6,fixBits(0x497a));
WritePixel(10,7,fixBits(0x497a));
WritePixel(10,8,fixBits(0x497a));
WritePixel(10,9,fixBits(0x497a));
WritePixel(10,10,fixBits(0x497a));
WritePixel(10,11,fixBits(0x497a));
WritePixel(10,12,fixBits(0x497a));

WritePixel(6,7,fixBits(0x497a));
WritePixel(7,7,fixBits(0x497a));
WritePixel(8,7,fixBits(0x497a));
WritePixel(9,7,fixBits(0x497a));
WritePixel(10,7,fixBits(0x497a));


}
void ClearA(){

WritePixel(5,3,fixBits(0xFFFF));
WritePixel(5,4,fixBits(0xFFFF));
WritePixel(5,5,fixBits(0xFFFF));
WritePixel(5,6,fixBits(0xFFFF));
WritePixel(5,7,fixBits(0xFFFF));
WritePixel(5,8,fixBits(0xFFFF));
WritePixel(5,9,fixBits(0xFFFF));
WritePixel(5,10,fixBits(0xFFFF));
WritePixel(5,11,fixBits(0xFFFF));
WritePixel(5,12,fixBits(0xFFFF));

WritePixel(6,3,fixBits(0xFFFF));
WritePixel(7,3,fixBits(0xFFFF));
WritePixel(8,3,fixBits(0xFFFF));
WritePixel(9,3,fixBits(0xFFFF));
WritePixel(10,3,fixBits(0xFFFF));

WritePixel(10,3,fixBits(0xFFFF));
WritePixel(10,4,fixBits(0xFFFF));
WritePixel(10,5,fixBits(0xFFFF));
WritePixel(10,6,fixBits(0xFFFF));
WritePixel(10,7,fixBits(0xFFFF));
WritePixel(10,8,fixBits(0xFFFF));
WritePixel(10,9,fixBits(0xFFFF));
WritePixel(10,10,fixBits(0xFFFF));
WritePixel(10,11,fixBits(0xFFFF));
WritePixel(10,12,fixBits(0xFFFF));

WritePixel(6,7,fixBits(0xFFFF));
WritePixel(7,7,fixBits(0xFFFF));
WritePixel(8,7,fixBits(0xFFFF));
WritePixel(9,7,fixBits(0xFFFF));
WritePixel(10,7,fixBits(0xFFFF));

}

void PrintB(){
WritePixel(5,3,fixBits(0x497a));
WritePixel(5,4,fixBits(0x497a));
WritePixel(5,5,fixBits(0x497a));
WritePixel(5,6,fixBits(0x497a));
WritePixel(5,7,fixBits(0x497a));
WritePixel(5,8,fixBits(0x497a));
WritePixel(5,9,fixBits(0x497a));
WritePixel(5,10,fixBits(0x497a));
WritePixel(5,11,fixBits(0x497a));

WritePixel(6,3,fixBits(0x497a));
WritePixel(7,3,fixBits(0x497a));
WritePixel(8,3,fixBits(0x497a));
WritePixel(9,3,fixBits(0x497a));
WritePixel(10,3,fixBits(0x497a));

WritePixel(10,4,fixBits(0x497a));
WritePixel(10,5,fixBits(0x497a));
WritePixel(10,6,fixBits(0x497a));
WritePixel(10,7,fixBits(0x497a));
WritePixel(10,8,fixBits(0x497a));
WritePixel(10,9,fixBits(0x497a));
WritePixel(10,10,fixBits(0x497a));
WritePixel(10,11,fixBits(0x497a));


WritePixel(6,7,fixBits(0x497a));
WritePixel(7,7,fixBits(0x497a));
WritePixel(8,7,fixBits(0x497a));
WritePixel(9,7,fixBits(0x497a));

WritePixel(6,11,fixBits(0x497a));
WritePixel(7,11,fixBits(0x497a));
WritePixel(8,11,fixBits(0x497a));
WritePixel(9,11,fixBits(0x497a));
WritePixel(10,11,fixBits(0x497a));

}

void ClearB(){
WritePixel(5,3,fixBits(0xFFFF));
WritePixel(5,4,fixBits(0xFFFF));
WritePixel(5,5,fixBits(0xFFFF));
WritePixel(5,6,fixBits(0xFFFF));
WritePixel(5,7,fixBits(0xFFFF));
WritePixel(5,8,fixBits(0xFFFF));
WritePixel(5,9,fixBits(0xFFFF));
WritePixel(5,10,fixBits(0xFFFF));
WritePixel(5,11,fixBits(0xFFFF));

WritePixel(6,3,fixBits(0xFFFF));
WritePixel(7,3,fixBits(0xFFFF));
WritePixel(8,3,fixBits(0xFFFF));
WritePixel(9,3,fixBits(0xFFFF));
WritePixel(10,3,fixBits(0xFFFF));

WritePixel(10,4,fixBits(0xFFFF));
WritePixel(10,5,fixBits(0xFFFF));
WritePixel(10,6,fixBits(0xFFFF));
WritePixel(10,7,fixBits(0xFFFF));
WritePixel(10,8,fixBits(0xFFFF));
WritePixel(10,9,fixBits(0xFFFF));
WritePixel(10,10,fixBits(0xFFFF));
WritePixel(10,11,fixBits(0xFFFF));


WritePixel(6,7,fixBits(0xFFFF));
WritePixel(7,7,fixBits(0xFFFF));
WritePixel(8,7,fixBits(0xFFFF));
WritePixel(9,7,fixBits(0xFFFF));

WritePixel(6,11,fixBits(0xFFFF));
WritePixel(7,11,fixBits(0xFFFF));
WritePixel(8,11,fixBits(0xFFFF));
WritePixel(9,11,fixBits(0xFFFF));
WritePixel(10,11,fixBits(0xFFFF));



}

void PrintCorrect(){

WritePixel(3,2,fixBits(0x06c1));
WritePixel(4,2,fixBits(0x06c1));
WritePixel(5,2,fixBits(0x06c1));
WritePixel(6,2,fixBits(0x06c1));
WritePixel(7,2,fixBits(0x06c1));
WritePixel(8,2,fixBits(0x06c1));
WritePixel(9,2,fixBits(0x06c1));
WritePixel(10,2,fixBits(0x06c1));
WritePixel(11,2,fixBits(0x06c1));
WritePixel(12,2,fixBits(0x06c1));
WritePixel(13,2,fixBits(0x06c1));


WritePixel(3,12,fixBits(0x06c1));
WritePixel(4,12,fixBits(0x06c1));
WritePixel(5,12,fixBits(0x06c1));
WritePixel(6,12,fixBits(0x06c1));
WritePixel(7,12,fixBits(0x06c1));
WritePixel(8,12,fixBits(0x06c1));
WritePixel(9,12,fixBits(0x06c1));
WritePixel(10,12,fixBits(0x06c1));
WritePixel(11,12,fixBits(0x06c1));
WritePixel(12,12,fixBits(0x06c1));
WritePixel(13,12,fixBits(0x06c1));

WritePixel(3,2,fixBits(0x06c1));
WritePixel(3,3,fixBits(0x06c1));
WritePixel(3,4,fixBits(0x06c1));
WritePixel(3,5,fixBits(0x06c1));
WritePixel(3,6,fixBits(0x06c1));
WritePixel(3,7,fixBits(0x06c1));
WritePixel(3,8,fixBits(0x06c1));
WritePixel(3,9,fixBits(0x06c1));
WritePixel(3,10,fixBits(0x06c1));
WritePixel(3,11,fixBits(0x06c1));

WritePixel(13,2,fixBits(0x06c1));
WritePixel(13,3,fixBits(0x06c1));
WritePixel(13,4,fixBits(0x06c1));
WritePixel(13,5,fixBits(0x06c1));
WritePixel(13,6,fixBits(0x06c1));
WritePixel(13,7,fixBits(0x06c1));
WritePixel(13,8,fixBits(0x06c1));
WritePixel(13,9,fixBits(0x06c1));
WritePixel(13,10,fixBits(0x06c1));
WritePixel(13,11,fixBits(0x06c1));



}


void PrintInCorrect(){

WritePixel(3,2,fixBits(0xf801));
WritePixel(4,2,fixBits(0xf801));
WritePixel(5,2,fixBits(0xf801));
WritePixel(6,2,fixBits(0xf801));
WritePixel(7,2,fixBits(0xf801));
WritePixel(8,2,fixBits(0xf801));
WritePixel(9,2,fixBits(0xf801));
WritePixel(10,2,fixBits(0xf801));
WritePixel(11,2,fixBits(0xf801));
WritePixel(12,2,fixBits(0xf801));
WritePixel(13,2,fixBits(0xf801));


WritePixel(3,12,fixBits(0xf801));
WritePixel(4,12,fixBits(0xf801));
WritePixel(5,12,fixBits(0xf801));
WritePixel(6,12,fixBits(0xf801));
WritePixel(7,12,fixBits(0xf801));
WritePixel(8,12,fixBits(0xf801));
WritePixel(9,12,fixBits(0xf801));
WritePixel(10,12,fixBits(0xf801));
WritePixel(11,12,fixBits(0xf801));
WritePixel(12,12,fixBits(0xf801));
WritePixel(13,12,fixBits(0xf801));

WritePixel(3,2,fixBits(0xf801));
WritePixel(3,3,fixBits(0xf801));
WritePixel(3,4,fixBits(0xf801));
WritePixel(3,5,fixBits(0xf801));
WritePixel(3,6,fixBits(0xf801));
WritePixel(3,7,fixBits(0xf801));
WritePixel(3,8,fixBits(0xf801));
WritePixel(3,9,fixBits(0xf801));
WritePixel(3,10,fixBits(0xf801));
WritePixel(3,11,fixBits(0xf801));

WritePixel(13,2,fixBits(0xf801));
WritePixel(13,3,fixBits(0xf801));
WritePixel(13,4,fixBits(0xf801));
WritePixel(13,5,fixBits(0xf801));
WritePixel(13,6,fixBits(0xf801));
WritePixel(13,7,fixBits(0xf801));
WritePixel(13,8,fixBits(0xf801));
WritePixel(13,9,fixBits(0xf801));
WritePixel(13,10,fixBits(0xf801));
WritePixel(13,11,fixBits(0xf801));



}
void ClearCheck(){

WritePixel(3,2,fixBits(0xFFFF));
WritePixel(4,2,fixBits(0xFFFF));
WritePixel(5,2,fixBits(0xFFFF));
WritePixel(6,2,fixBits(0xFFFF));
WritePixel(7,2,fixBits(0xFFFF));
WritePixel(8,2,fixBits(0xFFFF));
WritePixel(9,2,fixBits(0xFFFF));
WritePixel(10,2,fixBits(0xFFFF));
WritePixel(11,2,fixBits(0xFFFF));
WritePixel(12,2,fixBits(0xFFFF));
WritePixel(13,2,fixBits(0xFFFF));


WritePixel(3,12,fixBits(0xFFFF));
WritePixel(4,12,fixBits(0xFFFF));
WritePixel(5,12,fixBits(0xFFFF));
WritePixel(6,12,fixBits(0xFFFF));
WritePixel(7,12,fixBits(0xFFFF));
WritePixel(8,12,fixBits(0xFFFF));
WritePixel(9,12,fixBits(0xFFFF));
WritePixel(10,12,fixBits(0xFFFF));
WritePixel(11,12,fixBits(0xFFFF));
WritePixel(12,12,fixBits(0xFFFF));
WritePixel(13,12,fixBits(0xFFFF));

WritePixel(3,2,fixBits(0xFFFF));
WritePixel(3,3,fixBits(0xFFFF));
WritePixel(3,4,fixBits(0xFFFF));
WritePixel(3,5,fixBits(0xFFFF));
WritePixel(3,6,fixBits(0xFFFF));
WritePixel(3,7,fixBits(0xFFFF));
WritePixel(3,8,fixBits(0xFFFF));
WritePixel(3,9,fixBits(0xFFFF));
WritePixel(3,10,fixBits(0xFFFF));
WritePixel(3,11,fixBits(0xFFFF));

WritePixel(13,2,fixBits(0xFFFF));
WritePixel(13,3,fixBits(0xFFFF));
WritePixel(13,4,fixBits(0xFFFF));
WritePixel(13,5,fixBits(0xFFFF));
WritePixel(13,6,fixBits(0xFFFF));
WritePixel(13,7,fixBits(0xFFFF));
WritePixel(13,8,fixBits(0xFFFF));
WritePixel(13,9,fixBits(0xFFFF));
WritePixel(13,10,fixBits(0xFFFF));
WritePixel(13,11,fixBits(0xFFFF));

}


//takes in x and y
void RyukHead(int x, int y){
//Black
WritePixel(x,y,fixBits(0x0841));
WritePixel(x,y+1,fixBits(0x0841));
WritePixel(x -1,y,fixBits(0x0841));
WritePixel(x -2,y,fixBits(0x0841));
WritePixel(x -3,y,fixBits(0x0841));
WritePixel(x -4,y,fixBits(0x0841));
WritePixel(x-2,y+1,fixBits(0x0841));
WritePixel(x-2,y+2,fixBits(0x0841));
WritePixel(x-3,y+2,fixBits(0x0841));
WritePixel(x-1,y+2,fixBits(0x0841));
WritePixel(x-2,y+3,fixBits(0x0841));
WritePixel(x+2,y+1,fixBits(0x0841));
//Grey
WritePixel(x-1,y+1,fixBits(0xd69a));
WritePixel(x+1,y+1,fixBits(0xd69a));
WritePixel(x,y+2,fixBits(0xd69a));
WritePixel(x,y+3,fixBits(0xd69a));
WritePixel(x+1,y+3,fixBits(0xd69a));
WritePixel(x+1,y+4,fixBits(0xd69a));
WritePixel(x+2,y+5,fixBits(0xd69a));

WritePixel(x+2,y+2,fixBits(0xef5c));
WritePixel(x+1,y,fixBits(0xef5c));
WritePixel(x+2,y,fixBits(0xef5c));
WritePixel(x+3,y+2,fixBits(0xef5c));
WritePixel(x+3,y+1,fixBits(0xef5c));
WritePixel(x+3,y,fixBits(0xef5c));
WritePixel(x+3,y+2,fixBits(0xa2aa));

WritePixel(x+1,y+2,fixBits(0x622d));
WritePixel(x+2,y+3,fixBits(0x622d));
WritePixel(x+3,y+3,fixBits(0x622d));
WritePixel(x+2,y+4,fixBits(0x622d));
WritePixel(x+3,y+5,fixBits(0x622d));

WritePixel(x,y-1,fixBits(0x6b4d));
WritePixel(x-1,y-1,fixBits(0x6b4d));
WritePixel(x-2,y-1,fixBits(0x6b4d));
WritePixel(x-3,y-1,fixBits(0x6b4d));
WritePixel(x+1,y-1,fixBits(0x6b4d));
WritePixel(x+2,y-1,fixBits(0x6b4d));
WritePixel(x+3,y-1,fixBits(0x6b4d));

WritePixel(x,y-2,fixBits(0x6b4d));
WritePixel(x-1,y-2,fixBits(0x6b4d));
WritePixel(x-3,y-2,fixBits(0x6b4d));
WritePixel(x+1,y-2,fixBits(0x6b4d));
WritePixel(x+2,y-2,fixBits(0x6b4d));
WritePixel(x+3,y-2,fixBits(0x6b4d));
WritePixel(x+3,y-2,fixBits(0x6b4d));
WritePixel(x+2,y-3,fixBits(0x6b4d));



}

void ClearHead(int x, int y){
WritePixel(x,y,fixBits(0xFFFF));
WritePixel(x,y+1,fixBits(0xFFFF));
WritePixel(x -1,y,fixBits(0xFFFF));
WritePixel(x -2,y,fixBits(0xFFFF));
WritePixel(x -3,y,fixBits(0xFFFF));
WritePixel(x -4,y,fixBits(0xFFFF));
WritePixel(x-2,y+1,fixBits(0xFFFF));
WritePixel(x-2,y+2,fixBits(0xFFFF));
WritePixel(x-3,y+2,fixBits(0xFFFF));
WritePixel(x-1,y+2,fixBits(0xFFFF));
WritePixel(x-2,y+3,fixBits(0xFFFF));
WritePixel(x+2,y+1,fixBits(0xFFFF));
//Grey
WritePixel(x-1,y+1,fixBits(0xFFFF));
WritePixel(x+1,y+1,fixBits(0xFFFF));
WritePixel(x,y+2,fixBits(0xFFFF));
WritePixel(x,y+3,fixBits(0xFFFF));
WritePixel(x+1,y+3,fixBits(0xFFFF));
WritePixel(x+1,y+4,fixBits(0xFFFF));
WritePixel(x+2,y+5,fixBits(0xFFFF));

WritePixel(x+2,y+2,fixBits(0xFFFF));
WritePixel(x+1,y,fixBits(0xFFFF));
WritePixel(x+2,y,fixBits(0xFFFF));
WritePixel(x+3,y+2,fixBits(0xFFFF));
WritePixel(x+3,y+1,fixBits(0xFFFF));
WritePixel(x+3,y,fixBits(0xFFFF));
WritePixel(x+3,y+2,fixBits(0xFFFF));

WritePixel(x+1,y+2,fixBits(0xFFFF));
WritePixel(x+2,y+3,fixBits(0xFFFF));
WritePixel(x+3,y+3,fixBits(0xFFFF));
WritePixel(x+2,y+4,fixBits(0xFFFF));
WritePixel(x+3,y+5,fixBits(0xFFFF));

WritePixel(x,y-1,fixBits(0xFFFF));
WritePixel(x-1,y-1,fixBits(0xFFFF));
WritePixel(x-2,y-1,fixBits(0xFFFF));
WritePixel(x-3,y-1,fixBits(0xFFFF));
WritePixel(x+1,y-1,fixBits(0xFFFF));
WritePixel(x+2,y-1,fixBits(0xFFFF));
WritePixel(x+3,y-1,fixBits(0xFFFF));

WritePixel(x,y-2,fixBits(0xFFFF));
WritePixel(x-1,y-2,fixBits(0xFFFF));
WritePixel(x-3,y-2,fixBits(0xFFFF));
WritePixel(x+1,y-2,fixBits(0xFFFF));
WritePixel(x+2,y-2,fixBits(0xFFFF));
WritePixel(x+3,y-2,fixBits(0xFFFF));
WritePixel(x+3,y-2,fixBits(0xFFFF));
WritePixel(x+2,y-3,fixBits(0xFFFF));

}
//Symetric
void PrintHappy(int x, int y){
//Row 1
WritePixel(x,y,fixBits(0x37cc) );
WritePixel(x-1,y,fixBits(0x37cc));
WritePixel(x+1,y,fixBits(0x37cc));

//Row 2
WritePixel(x-2, y + 1, fixBits(0x37cc));
WritePixel(x-1, y + 1, fixBits(0x37cc));
WritePixel(x, y + 1, fixBits(0x37cc));
WritePixel(x+1, y + 1, fixBits(0x37cc));
WritePixel(x+2, y + 1, fixBits(0x37cc));

//Row 3
WritePixel(x-2, y + 2, fixBits(0x37cc));
WritePixel(x-1, y + 2, fixBits(0x0000));
WritePixel(x, y + 2, fixBits(0x37cc));
WritePixel(x+1, y + 2, fixBits(0x0000));
WritePixel(x+2, y + 2, fixBits(0x37cc));

//Row 4
WritePixel(x-2, y + 3, fixBits(0x0000));
WritePixel(x-1, y + 3, fixBits(0x37cc));
WritePixel(x, y + 3, fixBits(0x37cc));
WritePixel(x+1, y + 3, fixBits(0x37cc));
WritePixel(x+2, y + 3, fixBits(0x0000));

//Row 5 
WritePixel(x-1, y + 4, fixBits(0x0000));
WritePixel(x, y + 4, fixBits(0x0000));
WritePixel(x+1, y + 4, fixBits(0x0000));

}



void PrintHungry(int x , int y ){

//Row 1
WritePixel(x,y,fixBits(0xd0e4) );
WritePixel(x-1,y,fixBits(0xd0e4));
WritePixel(x+1,y,fixBits(0xd0e4));

//Row 2
WritePixel(x-2, y + 1, fixBits(0x0000));
WritePixel(x-1, y + 1, fixBits(0x0000));
WritePixel(x, y + 1, fixBits(0xd0e4));
WritePixel(x+1, y + 1, fixBits(0x0000));
WritePixel(x+2, y + 1, fixBits(0x0000));

//Row 3
WritePixel(x-2, y + 2, fixBits(0xd0e4));
WritePixel(x-1, y + 2, fixBits(0xd0e4));
WritePixel(x, y + 2, fixBits(0xd0e4));
WritePixel(x+1, y + 2, fixBits(0xd0e4));
WritePixel(x+2, y + 2, fixBits(0xd0e4));

//Row 4
WritePixel(x-2, y + 3, fixBits(0xd0e4));
WritePixel(x-1, y + 3, fixBits(0x0000));
WritePixel(x, y + 3, fixBits(0xd0e4));
WritePixel(x+1, y + 3, fixBits(0x0000));
WritePixel(x+2, y + 3, fixBits(0xd0e4));

//Row 5 
WritePixel(x-1, y + 3, fixBits(0xd0e4));
WritePixel(x, y + 3, fixBits(0x0000));
WritePixel(x+1, y + 3, fixBits(0xd0e4));


}

void PrintSad(int x, int y){

//Row 1
WritePixel(x,y,fixBits(0x0000) );
WritePixel(x-1,y,fixBits(0x521f));
WritePixel(x+1,y,fixBits(0x0000));

//Row 2
WritePixel(x-2, y + 1, fixBits(0x0000));
WritePixel(x-1, y + 1, fixBits(0x521f));
WritePixel(x, y + 1, fixBits(0x521f));
WritePixel(x+1, y + 1, fixBits(0x521f));
WritePixel(x+2, y + 1, fixBits(0x0000));

//Row 3
WritePixel(x-2, y + 2, fixBits(0x521f));
WritePixel(x-1, y + 2, fixBits(0x521f));
WritePixel(x, y + 2, fixBits(0x521f));
WritePixel(x+1, y + 2, fixBits(0x521f));
WritePixel(x+2, y + 2, fixBits(0x521f));

//Row 4
WritePixel(x-2, y + 3, fixBits(0x0000));
WritePixel(x-1, y + 3, fixBits(0x0000));
WritePixel(x, y + 3, fixBits(0x521f));
WritePixel(x+1, y + 3, fixBits(0x0000));
WritePixel(x+2, y + 3, fixBits(0x0000));

//Row 5 
WritePixel(x-1, y + 4, fixBits(0x521f));
WritePixel(x, y + 4, fixBits(0x521f));
WritePixel(x+1, y + 4, fixBits(0x521f));


}

void print1(int x, int y){

WritePixel(x,y,fixBits(0xFFFF));
WritePixel(x,y + 1,fixBits(0x0000));
WritePixel(x,y + 2,fixBits(0x0000));
WritePixel(x,y + 3,fixBits(0x0000));

}

void clear1(int x, int y){

WritePixel(x,y,fixBits(0xFFFF));
WritePixel(x,y + 1,fixBits(0xFFFF));
WritePixel(x,y + 2,fixBits(0xFFFF));
WritePixel(x,y + 3,fixBits(0xFFFF));

}

void print2(int x, int y){

WritePixel(x, y, fixBits(0x0000));
WritePixel(x + 1, y, fixBits(0x0000));
WritePixel(x + 1, y + 1, fixBits(0x0000));
WritePixel(x , y + 2, fixBits(0x0000));
WritePixel(x , y + 3, fixBits(0x0000));
WritePixel(x + 1 , y + 3, fixBits(0x0000));


}

void clear2(int x, int y){

WritePixel(x, y, fixBits(0xFFFF));
WritePixel(x + 1, y, fixBits(0xFFFF));
WritePixel(x + 1, y + 1, fixBits(0xFFFF));
WritePixel(x , y + 2, fixBits(0xFFFF));
WritePixel(x , y + 3, fixBits(0xFFFF));
WritePixel(x + 1 , y + 3, fixBits(0xFFFF));

}

void PrintEgg(){
//Row 1
WritePixel(7,3,fixBits(0x0000));
WritePixel(8,3,fixBits(0x0000));
WritePixel(9,3,fixBits(0x0000));
//Row 2
WritePixel(5,4,fixBits(0x0000));
WritePixel(6,4,0x0000);
WritePixel(7,4,fixBits(0xefb5));
WritePixel(8,4,fixBits(0xefb5));
WritePixel(9,4,fixBits(0xefb5));
WritePixel(10,4,fixBits(0x0000));
WritePixel(11,4,0x0000);
//Row 3
WritePixel(4,5,0x0000);
WritePixel(5,5,fixBits(0xef7a));
WritePixel(6,5,fixBits(0xefb5));
WritePixel(7,5,fixBits(0xefb5));
WritePixel(8,5,fixBits(0xefb5));
WritePixel(9,5,fixBits(0xefb5));
WritePixel(10,5,fixBits(0xef7a));
WritePixel(11,5,fixBits(0xef7a));
WritePixel(12,5,0x0000);
//Row 4 
WritePixel(4,6,0x0000);
WritePixel(5,6,fixBits(0xef7a));
WritePixel(6,6,fixBits(0xefb5));
WritePixel(7,6,fixBits(0xefb5));
WritePixel(8,6,fixBits(0xefb5));
WritePixel(9,6,fixBits(0xefb5));
WritePixel(10,6,fixBits(0xefb5));
WritePixel(11,6,fixBits(0xef7a));
WritePixel(12,6,0x0000);
//Row 5
WritePixel(3,7,0x0000);
WritePixel(4,7,0xefb5);
WritePixel(5,7,fixBits(0xefb5));
WritePixel(6,7,fixBits(0xefb5));
WritePixel(7,7,fixBits(0xefb5));
WritePixel(8,7,fixBits(0xefb5));
WritePixel(9,7,fixBits(0xefb5));
WritePixel(10,7,fixBits(0xefb5));
WritePixel(11,7,fixBits(0xef7a));
WritePixel(12,7,fixBits(0xef7a));
WritePixel(13,7,0x0000);
//Row 6
WritePixel(3,8,0x0000);
WritePixel(4,8,0xefb5);
WritePixel(5,8,fixBits(0xefb5));
WritePixel(6,8,fixBits(0xefb5));
WritePixel(7,8,fixBits(0xefb5));
WritePixel(8,8,fixBits(0xefb5));
WritePixel(9,8,fixBits(0xefb5));
WritePixel(10,8,fixBits(0xefb5));
WritePixel(11,8,fixBits(0xefbf));
WritePixel(12,8,fixBits(0xefb5));
WritePixel(13,8,0x0000);
//Row 7
WritePixel(3,9,0x0000);
WritePixel(4,9,0xefb5);
WritePixel(5,9,fixBits(0xefb5));
WritePixel(6,9,fixBits(0xefb5));
WritePixel(7,9,fixBits(0xefb5));
WritePixel(8,9,fixBits(0xefb5));
WritePixel(9,9,fixBits(0xefb5));
WritePixel(10,9,fixBits(0xefb5));
WritePixel(11,9,fixBits(0xefbf));
WritePixel(12,9,fixBits(0xefb5));
WritePixel(13,9,0x0000);
//Row 8
WritePixel(3,10,0x0000);
WritePixel(4,10,0xefb5);
WritePixel(5,10,fixBits(0xefb5));
WritePixel(6,10,fixBits(0xefb5));
WritePixel(7,10,fixBits(0xefb5));
WritePixel(8,10,fixBits(0xefb5));
WritePixel(9,10,fixBits(0xefb5));
WritePixel(10,10,fixBits(0xefb5));
WritePixel(11,10,fixBits(0xefbf));
WritePixel(12,10,fixBits(0xefb5));
WritePixel(13,10,0x0000);
//Row 9
WritePixel(3,11,0x0000);
WritePixel(4,11,0xefb5);
WritePixel(5,11,fixBits(0xefb5));
WritePixel(6,11,fixBits(0xefb5));
WritePixel(7,11,fixBits(0xefb5));
WritePixel(8,11,fixBits(0xefb5));
WritePixel(9,11,fixBits(0xefb5));
WritePixel(10,11,fixBits(0xefb5));
WritePixel(11,11,fixBits(0xefbf));
WritePixel(12,11,fixBits(0xefb5));
WritePixel(13,11,0x0000);
//Row 10
WritePixel(4,12,0x0000);
WritePixel(5,12,fixBits(0xef7a));
WritePixel(6,12,fixBits(0xefb5));
WritePixel(7,12,fixBits(0xefb5));
WritePixel(8,12,fixBits(0xefb5));
WritePixel(9,12,fixBits(0xefb5));
WritePixel(10,12,fixBits(0xefb5));
WritePixel(11,12,fixBits(0xef7a));
WritePixel(12,12,0x0000);
//Row 11
WritePixel(4,13,0x0000);
WritePixel(5,13,fixBits(0xef7a));
WritePixel(6,13,fixBits(0xefb5));
WritePixel(7,13,fixBits(0xefb5));
WritePixel(8,13,fixBits(0xefb5));
WritePixel(9,13,fixBits(0xefb5));
WritePixel(10,13,fixBits(0xef7a));
WritePixel(11,13,fixBits(0xef7a));
WritePixel(12,13,0x0000);
//Row 12
WritePixel(5,14,fixBits(0x0000));
WritePixel(6,14,0x0000);
WritePixel(7,14,fixBits(0xefb5));
WritePixel(8,14,fixBits(0xefb5));
WritePixel(9,14,fixBits(0xefb5));
WritePixel(10,14,fixBits(0x0000));
WritePixel(11,14,0x0000);
//Row 13
WritePixel(7,15,fixBits(0x0000));
WritePixel(8,15,fixBits(0x0000));
WritePixel(9,15,fixBits(0x0000));
}

void PrintKiara(){
//Row 1
WritePixel(5,0,fixBits(0x4208));
WritePixel(6,0,fixBits(0x4208));
WritePixel(7,0,fixBits(0x4208));
WritePixel(8,0,fixBits(0x4208));
WritePixel(9,0,fixBits(0x4208));
WritePixel(10,0,fixBits(0x4208));
//Row 2 
WritePixel(4,1,fixBits(0x4208));
WritePixel(5,1,fixBits(0x83d1));
WritePixel(6,1,fixBits(0xFFFF));
WritePixel(7,1,fixBits(0x4208));
WritePixel(8,1,fixBits(0x4208));
WritePixel(9,1,fixBits(0x83d1));
WritePixel(10,1,fixBits(0x83d1));
WritePixel(11,1,fixBits(0x4208));
//Row 3
WritePixel(3,2,fixBits(0x4208));
WritePixel(4,2,fixBits(0x4208));
WritePixel(5,2,fixBits(0xFFFF));
WritePixel(6,2,fixBits(0xFFFF));
WritePixel(7,2,fixBits(0xFFFF));
WritePixel(8,2,fixBits(0x4208));
WritePixel(9,2,fixBits(0x4208));
WritePixel(10,2,fixBits(0x83d1));
WritePixel(11,2,fixBits(0x4208));
WritePixel(12,2,fixBits(0x4208));
//Row 4
WritePixel(3,3,fixBits(0x83d1));
WritePixel(4,3,fixBits(0x83d1));
WritePixel(5,3,fixBits(0xFFFF));
WritePixel(6,3,fixBits(0xacb0));
WritePixel(7,3,fixBits(0xFFFF));
WritePixel(8,3,fixBits(0x4208));
WritePixel(9,3,fixBits(0xacb0));
WritePixel(10,3,fixBits(0x4208));
WritePixel(11,3,fixBits(0x83d1));
WritePixel(12,3,fixBits(0x83d1));
//Row 5
WritePixel(3,4,fixBits(0x4208));
WritePixel(4,4,fixBits(0x4208));
WritePixel(5,4,fixBits(0xacb0));
WritePixel(6,4,fixBits(0xffdd));
WritePixel(7,4,fixBits(0xacb0));
WritePixel(8,4,fixBits(0xacb0));
WritePixel(9,4,fixBits(0xffdd));
WritePixel(10,4,fixBits(0xacb0));
WritePixel(11,4,fixBits(0x4208));
WritePixel(12,4,fixBits(0x4208));
//Row 6
WritePixel(2,5,fixBits(0x0000));
WritePixel(3,5,fixBits(0x4208));
WritePixel(4,5,fixBits(0x4208));
WritePixel(5,5,fixBits(0x0000));
WritePixel(6,5,fixBits(0x0000));
WritePixel(7,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));
WritePixel(9,5,fixBits(0x0000));
WritePixel(10,5,fixBits(0x0000));
WritePixel(11,5,fixBits(0x4208));
WritePixel(12,5,fixBits(0x4208));
WritePixel(13,5,fixBits(0x0000));
//Row 7
WritePixel(1,6,fixBits(0x0000));
WritePixel(3,6,fixBits(0x0000));
WritePixel(4,6,fixBits(0x4208));
WritePixel(5,6,fixBits(0xffdd));
WritePixel(6,6,fixBits(0xc3ba));
WritePixel(7,6,fixBits(0xffdd));
WritePixel(8,6,fixBits(0xffdd));
WritePixel(9,6,fixBits(0xc3ba));
WritePixel(10,6,fixBits(0xffdd));
WritePixel(11,6,fixBits(0x4208));
WritePixel(12,6,fixBits(0x0000));
WritePixel(14,6,fixBits(0x0000));
//Row 8
WritePixel(2,7,fixBits(0x0000));
WritePixel(3,7,fixBits(0x0000));
WritePixel(4,7,fixBits(0x0000));
WritePixel(5,7,fixBits(0xffdd));
WritePixel(6,7,fixBits(0xffdd));
WritePixel(7,7,fixBits(0xffdd));
WritePixel(8,7,fixBits(0xffdd));
WritePixel(9,7,fixBits(0xffdd));
WritePixel(10,7,fixBits(0xffdd));
WritePixel(11,7,fixBits(0x0000));
WritePixel(12,7,fixBits(0x0000));
WritePixel(13,7,fixBits(0x0000));
//Row 9
WritePixel(1,8,fixBits(0x0000));
WritePixel(2,8,fixBits(0x0000));
WritePixel(3,8,fixBits(0x0000));
WritePixel(4,8,fixBits(0x0000));
WritePixel(5,8,fixBits(0xacb0));
WritePixel(6,8,fixBits(0x0000));
WritePixel(7,8,fixBits(0xacb0));
WritePixel(8,8,fixBits(0xacb0));
WritePixel(9,8,fixBits(0x0000));
WritePixel(10,8,fixBits(0xacb0));
WritePixel(11,8,fixBits(0x0000));
WritePixel(12,8,fixBits(0x0000));
WritePixel(13,8,fixBits(0x0000));
WritePixel(14,8,fixBits(0x0000));
//Row 10
WritePixel(3,9,fixBits(0x0000));
WritePixel(5,9,fixBits(0x83d1));
WritePixel(6,9,fixBits(0xFFFF));
WritePixel(7,9,fixBits(0xFFFF));
WritePixel(8,9,fixBits(0xFFFF));
WritePixel(9,9,fixBits(0xFFFF));
WritePixel(10,9,fixBits(0x83d1));
WritePixel(12,9,fixBits(0x0000));
//Row 11
WritePixel(2,10,fixBits(0x0000));
WritePixel(5,10,fixBits(0xffdd));
WritePixel(6,10,fixBits(0x83d1));
WritePixel(7,10,fixBits(0xFFFF));
WritePixel(8,10,fixBits(0xFFFF));
WritePixel(9,10,fixBits(0x83d1));
WritePixel(10,10,fixBits(0xffdd));
WritePixel(13,10,fixBits(0x0000));
//Row 12
WritePixel(4,11,fixBits(0xffdd));
WritePixel(5,11,fixBits(0xffdd));
WritePixel(6,11,fixBits(0xacb0));
WritePixel(7,11,fixBits(0xffdd));
WritePixel(8,11,fixBits(0xffdd));
WritePixel(9,11,fixBits(0xacb0));
WritePixel(10,11,fixBits(0xffdd));
WritePixel(11,11,fixBits(0xffdd));
//Row 13
WritePixel(6,12,fixBits(0x0000));
WritePixel(7,12,fixBits(0x0000));
WritePixel(8,12,fixBits(0x0000));
WritePixel(9,12,fixBits(0x0000));
//Row 14
WritePixel(6,13,fixBits(0x0000));
WritePixel(7,13,fixBits(0x0000));
WritePixel(8,13,fixBits(0x0000));
WritePixel(9,13,fixBits(0x0000));
//Row 15
WritePixel(6,14,fixBits(0x0000));
WritePixel(9,14,fixBits(0x0000));


}

void PrintKiaraHappy(){

WritePixel(5,6,fixBits(0x0000));
WritePixel(6,5,fixBits(0xffdd));
WritePixel(7,6,fixBits(0x0000));
WritePixel(8,6,fixBits(0x0000));
WritePixel(9,5,fixBits(0xffdd));
WritePixel(10,6,fixBits(0x0000));

}

void PrintKiaraSad(){

WritePixel(5,5,fixBits(0xffdd));
WritePixel(6,5,fixBits(0xffdd));
WritePixel(7,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));
WritePixel(9,5,fixBits(0xffdd));
WritePixel(10,5,fixBits(0xffdd));

WritePixel(5,6,fixBits(0x0000));
WritePixel(6,6,fixBits(0x0000));
WritePixel(9,6,fixBits(0x0000));
WritePixel(10,6,fixBits(0x0000));

WritePixel(6,7,fixBits(0x41fe));
WritePixel(9,7,fixBits(0x41fe));
WritePixel(6,8,fixBits(0x41fe));
WritePixel(9,8,fixBits(0x41fe));
}

void KiaraIdle1(){

    WritePixel(4,10,fixBits(0xffdd));
    WritePixel(5,11,0xFFFF);
    WritePixel(4,11,0xFFFF);

    WritePixel(9,14,0xFFFF);

    WritePixel(6,14,0x0000);
    WritePixel(10,11,fixBits(0xffdd));
    WritePixel(11,11,fixBits(0xffdd));
    WritePixel(11,10,fixBits(0xFFFF));
}

void KiaraIdle2(){

    WritePixel(4,10,fixBits(0xffff));
    WritePixel(5,11,0xffdd);
    WritePixel(4,11,0xffdd);

    WritePixel(9,14,0x0000);

    WritePixel(6,14,0xffff);
    WritePixel(10,11,fixBits(0xffff));
    WritePixel(11,11,fixBits(0xffff));
    WritePixel(11,10,fixBits(0xffdd));

}

void PrintAlice(){
//Row 1
WritePixel(5,0,fixBits(0x52ab));
WritePixel(6,0,fixBits(0x52ab));
WritePixel(7,0,fixBits(0x52ab));
WritePixel(8,0,fixBits(0x52ab));
//Row 2
WritePixel(3,1,fixBits(0x52ab));
WritePixel(4,1,fixBits(0x52ab));
WritePixel(5,1,fixBits(0x52ab));
WritePixel(6,1,fixBits(0x52ab));
WritePixel(7,1,fixBits(0x52ab));
WritePixel(8,1,fixBits(0x52ab));
WritePixel(9,1,fixBits(0x52ab));
WritePixel(10,1,fixBits(0x52ab));
//Row 3
WritePixel(3,2,fixBits(0x52ab));
WritePixel(4,2,fixBits(0x52ab));
WritePixel(5,2,fixBits(0x318a));
WritePixel(6,2,fixBits(0x52ab));
WritePixel(7,2,fixBits(0x52ab));
WritePixel(8,2,fixBits(0x318a));
WritePixel(9,2,fixBits(0x52ab));
WritePixel(10,2,fixBits(0x52ab));
//Row 4
WritePixel(2,3,fixBits(0x318a));
WritePixel(3,3,fixBits(0x52ab));
WritePixel(4,3,fixBits(0x318a));
WritePixel(5,3,fixBits(0xacb0));
WritePixel(6,3,fixBits(0xffdd));
WritePixel(7,3,fixBits(0xffdd));
WritePixel(8,3,fixBits(0xacb0));
WritePixel(9,3,fixBits(0x318a));
WritePixel(10,3,fixBits(0x52ab));
WritePixel(11,3,fixBits(0x318a));
//Row 5
WritePixel(2,4,fixBits(0xad56));
WritePixel(3,4,fixBits(0xad56));
WritePixel(4,4,fixBits(0xacb0));
WritePixel(5,4,fixBits(0xffdd));
WritePixel(6,4,fixBits(0xffdd));
WritePixel(7,4,fixBits(0xffdd));
WritePixel(8,4,fixBits(0xffdd));
WritePixel(9,4,fixBits(0xacb0));
WritePixel(10,4,fixBits(0xad56));
WritePixel(11,4,fixBits(0xad56));
//Row 6
WritePixel(2,5,fixBits(0xad56));
WritePixel(3,5,fixBits(0xad56));
WritePixel(4,5,fixBits(0xffdd));
WritePixel(5,5,fixBits(0x0000));
WritePixel(6,5,fixBits(0xffdd));
WritePixel(7,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0x0000));
WritePixel(9,5,fixBits(0xffdd));
WritePixel(10,5,fixBits(0xad56));
WritePixel(11,5,fixBits(0xad56));
//Row 7
WritePixel(2,6,fixBits(0x318a));
WritePixel(3,6,fixBits(0xad56));
WritePixel(4,6,fixBits(0x0000));
WritePixel(5,6,fixBits(0x6f32));
WritePixel(6,6,fixBits(0xffdd));
WritePixel(7,6,fixBits(0xffdd));
WritePixel(8,6,fixBits(0x6f32));
WritePixel(9,6,fixBits(0x0000));
WritePixel(10,6,fixBits(0xad56));
WritePixel(11,6,fixBits(0x318a));
//Row 8
WritePixel(2,7,fixBits(0x318a));
WritePixel(3,7,fixBits(0xad56));
WritePixel(4,7,fixBits(0xc5f2));
WritePixel(5,7,fixBits(0xcf52));
WritePixel(6,7,fixBits(0xffdd));
WritePixel(7,7,fixBits(0xffdd));
WritePixel(8,7,fixBits(0xc5f2));
WritePixel(9,7,fixBits(0xcf52));
WritePixel(11,7,fixBits(0x318a));
WritePixel(10,7,fixBits(0xad56));
//Row 9
WritePixel(2,8,fixBits(0x318a));
WritePixel(3,8,fixBits(0x318a));
WritePixel(4,8,fixBits(0x5559));
WritePixel(5,8,fixBits(0x9e1a));
WritePixel(6,8,fixBits(0xacb0));
WritePixel(7,8,fixBits(0xacb0));
WritePixel(8,8,fixBits(0x9e1a));
WritePixel(9,8,fixBits(0x5559));
WritePixel(10,8,fixBits(0x318a));
WritePixel(11,8,fixBits(0x318a));
//Row 10
WritePixel(2,9,fixBits(0x318a));
WritePixel(3,9,fixBits(0x865d));
WritePixel(4,9,fixBits(0x5559));
WritePixel(5,9,fixBits(0xbf3e));
WritePixel(6,9,fixBits(0xacb0));
WritePixel(7,9,fixBits(0xacb0));
WritePixel(8,9,fixBits(0xbf3e));
WritePixel(9,9,fixBits(0x5559));
WritePixel(10,9,fixBits(0x865d));
WritePixel(11,9,fixBits(0x318a));
//Row 11

WritePixel(3,10,fixBits(0x318a));
WritePixel(4,10,fixBits(0xffdd));
WritePixel(5,10,fixBits(0x7e7e));
WritePixel(6,10,fixBits(0xd75e));
WritePixel(7,10,fixBits(0xd75e));
WritePixel(8,10,fixBits(0x7e7e));
WritePixel(9,10,fixBits(0xffdd));
WritePixel(10,10,fixBits(0x318a));
//Row 12
WritePixel(3,11,fixBits(0xFFFF));
WritePixel(4,11,fixBits(0xffdd));
WritePixel(5,11,fixBits(0x4c34));
WritePixel(6,11,fixBits(0xd75e));
WritePixel(7,11,fixBits(0xd75e));
WritePixel(8,11,fixBits(0x4c34));
WritePixel(9,11,fixBits(0xffdd));
WritePixel(10,11,fixBits(0xFFFF));
//Row 13
WritePixel(4,12,fixBits(0x7e7e));
WritePixel(5,12,fixBits(0x7e7e));
WritePixel(6,12,fixBits(0xc6dc));
WritePixel(7,12,fixBits(0xc6dc));
WritePixel(8,12,fixBits(0x7e7e));
WritePixel(9,12,fixBits(0x7e7e));
//Row 14
WritePixel(4,13,fixBits(0x7e7e));
WritePixel(5,13,fixBits(0xc6dc));
WritePixel(6,13,fixBits(0xc6dc));
WritePixel(7,13,fixBits(0xc6dc));
WritePixel(8,13,fixBits(0xc6dc));
WritePixel(9,13,fixBits(0x7e7e));
//Row 15
WritePixel(5,14,fixBits(0x0000));
WritePixel(8,14,fixBits(0x0000));

}

void AliceHappy(){

WritePixel(5,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));

WritePixel(5,6,fixBits(0x0000));
WritePixel(8,6,fixBits(0x0000));

WritePixel(4,7,fixBits(0xf20d));
WritePixel(5,7,fixBits(0xf20d));

WritePixel(8,7,fixBits(0xf20d));
WritePixel(9,7,fixBits(0xf20d));


}

void AliceSad(){

WritePixel(5,5,fixBits(0xffdd));
WritePixel(8,5,fixBits(0xffdd));

WritePixel(5,6,fixBits(0x0000));
WritePixel(8,6,fixBits(0x0000));

WritePixel(5,7,fixBits(0x2bbe));
WritePixel(8,7,fixBits(0x2bbe));
WritePixel(5,8,fixBits(0x2bbe));
WritePixel(8,8,fixBits(0x2bbe));



}





#endif