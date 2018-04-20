#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"
#include "rotary.h"

#define TOPLEFTX 135
#define TOPLEFTY 75

volatile uint8_t squarex;
volatile uint8_t squarey;

volatile uint8_t numberMatrix[9][9];
uint8_t boolMatrix[9][9];

int main(){
  
  
}

void drawPuzzle(){
  
  //Background Square
  struct rectangle rect;
  rect.left = TOPLEFTX;
  rect.right = TOPLEFTX+91;
  rect.top = TOPLEFTY;
  rect.bottom = TOPLEFTY + 91;
  fill_rectangle(rect,BLACK)
  
  for(int i =0;i<9;i++){
    for(int j=0;j<9;j++){
      
        //Number white box
        rect.left = getBoxX(i);
        rect.right = rect.left + 8;
        rect.top = getBoxY(j);
        rect.bottom = rect.top + 8;
        fill_rectangle(rect,WHITE);
        
        //Number
        if(boolMatrix[i][j]){
          char buffer[2];
          itoa(numberMatrix[i][j],buffer);
          display_string_xy(buffer, (getBoxX(i) + 2 ), (getBoxY(i)+1));
        }
    }
  }
  
  
  
}

uint8_t getBoxX(uint8_t i){
  return TOPLEFTX + (10*i) + 1;
}

uint8_t getBoxY(uint8_t j){
  return TOPLEFTY + (10*j) + 1;
}

uint8_t checkSolved(){
  uint8_t set[9];

  //Checks all rows & columns
  for(int i =0;i<9;i++){
    if(!checkSet(numberMatrix[i])){ return 0;}    
    for(int j = 0;j<9;j++){
      set[j] = numberMatrix[j][i];
    } 
    if(!checkSet(set)){ return 0;}
  }

  //Checks each square 
  // TODO: Not do it this way
  for(int i = 0;i<3;i++){
    for(int j = 0;j<3;j++){
      for(int k =0;k<3;k++{
        for(int l =0;l<3;l++{
          set[k+l]=numberMatrix[(3*i)+k][(3*j)+l];
        }
      }
      if(!checkSet(set)){ return 0;}
    }
  }
}

uint8_t checkSet(uint8_t set[9]){
  uint8_t sum = 0;
  for(int i=0;i<9;i++){
    sum += set[i];
  }
  if(sum != 45) {return 0;}
  for(int i=0;i<8;i++){
    for(int j=i+1;j<9;j++){
      if(set[j] == set[i]) {return 0;}
    }
  }
  return 1;
}