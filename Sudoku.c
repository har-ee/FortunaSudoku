#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "ruota.h"
#include "themes.h"

#define CELLSIZE 25 /*Odd number means text aligned in centre, should be > 7 */
#define TOPLEFTX (LCDHEIGHT/2) - ((9*(CELLSIZE+1))/2) - 1
#define TOPLEFTY (LCDWIDTH/2) - ((9*(CELLSIZE+1))/2) - 1
#define PUZZLE_HARDCODED 0

/* In order to set difficulty or reduce load time */
#define MAX_MISSING_NUMBERS 60
#define MIN_MISSING_NUMBERS 48

/* drawing methods */
void drawMenu(int selected);
void drawMenuBoxOutline(int box,uint16_t col);
void drawPuzzle();
void drawPointer();
void updatePointer(uint8_t oldx,uint8_t oldy);
void fillBoxOutline(uint8_t x, uint8_t y, uint16_t col);
void fillCell(uint8_t x,uint8_t y, uint16_t col);
void fillBackground(uint16_t col);
void fillGridBackground(uint16_t col);
void drawNumber(uint8_t x,uint8_t y, uint8_t number);

/* coordinate methods */
uint8_t getBoxX(uint8_t i);
uint8_t getBoxY(uint8_t i);
uint8_t getCellX(uint8_t i);
uint8_t getCellY(uint8_t j);

/* solution checking methods */
uint8_t checkSolved();
uint8_t checkSet(uint8_t set[9]);
uint8_t checkSquare(uint8_t x, uint8_t y, int a);
uint8_t checkRowandColumn(uint8_t x, uint8_t y, int a);
uint8_t solveSudoku(uint32_t breakAt, uint8_t fillPuzzle);
uint8_t randomSolveSudoku(uint32_t breakAt,uint8_t fillPuzzle);
void generatePuzzle();

void menu();
void runGame();

int8_t pointerx =0;
int8_t pointery =0;
struct Theme theme = defaulttheme;
int8_t currentTheme;
  
uint16_t EEMEM eepromseed;
int8_t EEMEM storedMatrix[9][9];
int8_t EEMEM storedboolMatrix[9][9];
uint16_t EEMEM savedTheme;

/*    SOLVED GRID
  {1,5,2,6,3,4,8,7,9},
  {6,8,3,7,9,1,2,5,4},
  {9,4,7,2,8,5,6,3,1},
  {5,2,9,8,4,3,1,6,7},
  {7,3,6,5,1,2,4,9,8},
  {8,1,4,9,7,6,5,2,3},
  {2,7,8,4,6,9,3,1,5},
  {3,9,5,1,2,8,7,4,6},
  {4,6,1,3,5,7,9,8,2}
*/

/*int8_t blankMatrix[9][9]= {
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}
};*/

int8_t numberMatrix[9][9]= {
  {1,5,0,6,3,0,0,7,9},
  {6,0,0,7,0,1,0,0,0},
  {0,0,7,2,8,0,0,3,1},
  {0,0,9,8,0,3,0,6,0},
  {0,3,0,0,1,0,0,9,0},
  {0,1,0,9,0,6,5,0,0},
  {2,7,0,0,6,9,3,0,0},
  {0,0,0,1,0,8,0,0,6},
  {4,6,0,0,5,7,0,8,2}
    
};

/*Any non 0's are editable by the player.*/
int8_t boolMatrix[9][9] =
{
  {1,5,0,6,3,0,0,7,9},
  {6,0,0,7,0,1,0,0,0},
  {0,0,7,2,8,0,0,3,1},
  {0,0,9,8,0,3,0,6,0},
  {0,3,0,0,1,0,0,9,0},
  {0,1,0,9,0,6,5,0,0},
  {2,7,0,0,6,9,3,0,0},
  {0,0,0,1,0,8,0,0,6},
  {4,6,0,0,5,7,0,8,2}
    
};


int main(){ 
  
  uint16_t seed = eeprom_read_word(&eepromseed);
  eeprom_write_word(&eepromseed, seed + 1);
  srand(seed); /* by using the eeprom, I can get a different seed each time */

  init_lcd();
  set_orientation(West);
  os_init_ruota();
  
  menu();

  return 0;
}

void runGame(){
  
  fillBackground(theme.background);
  drawPuzzle();
  drawPointer();
  for(;;){
    
    scan_encoder();
    
    int delta = os_enc_delta();
    if(delta){
      if(!boolMatrix[pointery][pointerx]){
        numberMatrix[pointery][pointerx] += delta;
        if(numberMatrix[pointery][pointerx]<0) numberMatrix[pointery][pointerx] = 9;
        numberMatrix[pointery][pointerx] = numberMatrix[pointery][pointerx]%10;
        
        if(numberMatrix[pointery][pointerx]){
          display_color(theme.editable_numbers,theme.cell_background);
          drawNumber(pointerx,pointery,numberMatrix[pointery][pointerx]);
        } else {
          fillCell(pointerx,pointery,theme.cell_background);
        }
      }
    }
    
    scan_switches();
    if (get_switch_press(_BV(SWS))) {
      uint8_t oldx = pointerx;
      uint8_t oldy = pointery;
      pointery+=1;
      pointery = pointery%9;
      updatePointer(oldx,oldy);
    }
    if (get_switch_press(_BV(SWN))) {
      uint8_t oldx = pointerx;
      uint8_t oldy = pointery;
      pointery-=1;
      if(pointery <0) pointery = 8; 
      updatePointer(oldx,oldy);
    }
    
   if (get_switch_press(_BV(SWE))) {
      uint8_t oldx = pointerx;
      uint8_t oldy = pointery;
      pointerx+=1;
      pointerx = pointerx%9;
      updatePointer(oldx,oldy);
    }
    
   if (get_switch_press(_BV(SWW))) {
      uint8_t oldx = pointerx;
      uint8_t oldy = pointery;
      pointerx-=1;
      if(pointerx <0) pointerx = 8; 
      updatePointer(oldx,oldy);
    }
    
    if (get_switch_press(_BV(SWC))) {
      if(checkSolved()){
        break;
      } else {
        fillGridBackground(RED);
        display_color(WHITE,RED);
        display_string_xy("Incorrect solution, but progress saved!",LCDHEIGHT/2-115,LCDWIDTH -9);
        eeprom_update_block((void*)numberMatrix, (const void*)storedMatrix, 81*sizeof(int8_t));
        eeprom_update_block((void*)boolMatrix, (const void*)storedboolMatrix, 81*sizeof(int8_t));
        fillGridBackground(RED);
        display_string_xy("Incorrect solution, but progress saved!",LCDHEIGHT/2-115,LCDWIDTH -9);
        drawPuzzle();
        drawPointer();


      }
    }
  }
  fillBackground(GREEN);
  drawPuzzle();
  display_color(theme.editable_numbers,LIME_GREEN);
  display_string_xy("Congratulations! You have completed this Sudoku!",LCDHEIGHT/2-24*6,LCDWIDTH/2);
  for(;;){
    scan_switches();
    if (get_switch_press(_BV(SWC))) {
      break;
    }
  }
}

void menu(){
  uint8_t i;
  uint8_t j;
  uint8_t bool;
  currentTheme = eeprom_read_word(&savedTheme);
  if(currentTheme<0 || currentTheme >= NUM_THEMES) currentTheme = 0;
  theme = themeList[currentTheme];

        
  int8_t selected;
  selected = 0;
  drawMenu(selected);
  for(;;){
    scan_switches();
    
    if (get_switch_press(_BV(SWS))) {
      drawMenuBoxOutline(selected,theme.cell_frame);
      selected++;
      selected = selected%3;
      drawMenuBoxOutline(selected,theme.cursor_border);
    }
    
    if (get_switch_press(_BV(SWN))) {
      drawMenuBoxOutline(selected,theme.cell_frame);
      selected--;
      if(selected<0){
        selected = 2;
      }
      drawMenuBoxOutline(selected,theme.cursor_border);
    }
    
    if (get_switch_press(_BV(SWC))) {
      if(!selected){
        if(!PUZZLE_HARDCODED){
          generatePuzzle();
        }
        runGame();
        drawMenu(selected);
      } else if (selected == 1){        
        eeprom_read_block((void*)numberMatrix, (const void*)storedMatrix, 81*sizeof(selected));
        bool = 1;
        for(i=0;i<9;i++){
          for(j=0;j<9;j++){
            if(numberMatrix[i][j] < 0 || numberMatrix[i][j] > 9) bool = 0;
          }
        }
        if(bool){
          eeprom_read_block((void*)boolMatrix, (const void*)storedboolMatrix, 81*sizeof(selected));
          runGame();
          drawMenu(selected);
        } else {
          display_string_xy("No save data to load.",LCDHEIGHT/2-60,LCDWIDTH/2);
        }

      } else {
        currentTheme++;
        currentTheme = currentTheme % NUM_THEMES ;
        eeprom_write_word(&savedTheme, currentTheme);
        theme = themeList[currentTheme];
        drawMenu(selected);    
      }
    }
    
  }
}

void drawMenuBoxOutline(int box, uint16_t col){
  rectangle rect;
  rect.top = 88 + 45*box;
  rect.bottom = rect.top + 1;
  rect.left = LCDHEIGHT/2 - 77;
  rect.right = LCDHEIGHT/2 + 77;
  fill_rectangle(rect, col);
  
  rect.top+= 28;
  rect.bottom = rect.top + 1;
  fill_rectangle(rect, col);

  rect.right = rect.left+1;
  rect.top -=28;
  fill_rectangle(rect, col);
  
  rect.right = LCDHEIGHT/2 + 77;
  rect.left = rect.right - 1;
  fill_rectangle(rect, col);
  
}

void drawMenu(int selected){
  uint8_t x;
  uint8_t y;
  
  x = LCDHEIGHT/2 - (5.5*6);
  y = 40;
  fillBackground(theme.cell_frame);
 // fillGridBackground(theme.cell_frame);
  display_color(theme.noneditable_numbers,theme.cell_frame);
  display_string_xy("S U D O K U",x,y);
  
  rectangle rect;  
  display_color(theme.editable_numbers,theme.cell_background);
  rect.left = LCDHEIGHT/2 - 75;
  rect.right = LCDHEIGHT/2 + 75;  
  rect.top = 90;
  rect.bottom = 115;
  fill_rectangle(rect, theme.cell_background);
  display_string_xy("New Game",LCDHEIGHT/2 - (8/2)*6,rect.top+9);

  rect.top = 135;
  rect.bottom = 160;
  fill_rectangle(rect, theme.cell_background);
  display_string_xy("Load Game",LCDHEIGHT/2 - 27,rect.top+9);

  
  rect.top = 180;
  rect.bottom = 205;
  fill_rectangle(rect, theme.cell_background);
  display_string_xy("Theme: ",LCDHEIGHT/2 - 21 - (3* 7),rect.top+9);

  display_string_xy(theme.theme_name,LCDHEIGHT/2 + 21 - (3* 7),180+9);

  drawMenuBoxOutline(selected,theme.cursor_border);
  

}

/* Used to either populate and solve the puzzle or find the number of solutions
   Uses numberMatrix as the sudoku puzzle to solve
   In order to be efficient, can set to stop at a certain threshold of found solutions*/
uint8_t randomSolveSudoku(uint32_t breakAt,uint8_t fillPuzzle){
  uint8_t i;
  uint8_t j;
  uint8_t k;
  uint8_t a;
  uint32_t b = 0;
  uint32_t numSolutions = 0;

  
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      /* if square is already full, try next square */
      if(!numberMatrix[i][j]){
        
        k = (uint8_t) (rand()%9) + 1; /*Allows for different random sudoku grids to be generated*/
        for(a=k;a<k+9;a++) {
          if(checkRowandColumn(i,j,(a%9)+1) && checkSquare(i,j,(a%9)+1)){
            numberMatrix[i][j] = (a%9)+1;
            b = solveSudoku((breakAt - numSolutions),fillPuzzle);

            if(b){
              numSolutions += b;
              if(numSolutions >= breakAt){
                if(!fillPuzzle) numberMatrix[i][j] = 0;
                return numSolutions;
              }
            }

          }
        }
        numberMatrix[i][j] = 0;
        return numSolutions;
      }
    } 
  }
  return 1;  
}

uint8_t solveSudoku(uint32_t breakAt,uint8_t fillPuzzle){
  uint8_t i;
  uint8_t j;
  uint8_t k;
  uint8_t a;
  uint32_t b = 0;
  uint32_t numSolutions = 0;

  
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      /* if square is already full, try next square */
      if(!numberMatrix[i][j]){
        
        k = (uint8_t) (rand()%9) + 1; /*Allows for different random sudoku grids to be generated*/
        for(a=k;a<k+9;a++) {
          if(checkRowandColumn(i,j,(a%9)+1) && checkSquare(i,j,(a%9)+1)){
            numberMatrix[i][j] = (a%9)+1;
            b = solveSudoku((breakAt - numSolutions),fillPuzzle);

            if(b){
              numSolutions += b;
              if(numSolutions >= breakAt){
                if(!fillPuzzle) numberMatrix[i][j] = 0;
                return numSolutions;
              }
            }

          }
        }
        numberMatrix[i][j] = 0;
        return numSolutions;
      }
    } 
  }
  return 1;  
}

void generatePuzzle(){

  int i;
  int j;
  int ran1;
  int ran2;
  int oldvalue;
  int kpr;
  uint16_t count;
  
  
  kpr = CLKPR;
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;
  
  fillBackground(BLACK);
  display_color(WHITE,BLACK);
  display_string_xy("Generating a Sudoku puzzle...",LCDHEIGHT/2-16*6,LCDWIDTH/2);
  display_string_xy("Tip: Press the centre button to",LCDHEIGHT/2-16*6,LCDWIDTH*3/4);
  display_string_xy("save game & check your solution!",LCDHEIGHT/2-16*6,LCDWIDTH*3/4 + 7);
  
  /*Sets the puzzle to empty, overriding any hardcoded numbers */
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      numberMatrix[i][j] = 0;
    }
  }
  
  /* Populates grid with a random (complete) sudoku by solving the empty one */
  randomSolveSudoku(1,1);

  count = 0;
  /* Removes random numbers until we either reach MAX_MISSING_NUMBERS or removing
  the next random number would cause puzzle to have 2 solved states */
  for(;;){

    ran1 = (rand()%9);
    ran2 = (rand()%9);
    if(numberMatrix[ran1][ran2]){
      oldvalue = numberMatrix[ran1][ran2];
      numberMatrix[ran1][ran2] = 0;
      if(solveSudoku(2,0)==1){
        count++;
      } else {
        numberMatrix[ran1][ran2] = oldvalue;
        if(count >= MIN_MISSING_NUMBERS) break;
      }
    }
    if(count >= MAX_MISSING_NUMBERS) break;
  }

  /* populates the boolMatrix with our now complete puzzle */
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      boolMatrix[i][j] = numberMatrix[i][j];
    }
  }
  
  
   
  CLKPR = (1 << CLKPCE);
  CLKPR = kpr;
    
}

uint8_t checkSquare(uint8_t x, uint8_t y, int a){
  uint8_t i;
  uint8_t j;
  uint8_t subsquarex = (x/3);
  uint8_t subsquarey = (y/3);
  
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      if(numberMatrix[(3*subsquarex)+i][(3*subsquarey)+j]==a) return 0;
    }
  }
  return 1;
}

uint8_t checkRowandColumn(uint8_t x, uint8_t y, int a){
  uint8_t i;
  for(i=0;i<9;i++){
      if(numberMatrix[i][y]==a) return 0;
      if(numberMatrix[x][i]==a) return 0;
  }
  return 1;
}
  
void drawPointer(){
  fillBoxOutline(pointerx,pointery,theme.cursor_border);
}

void fillBackground(uint16_t col){
  rectangle rect;
  rect.left = 0;
  rect.right = LCDHEIGHT;
  rect.top = 0;
  rect.bottom = LCDWIDTH;
  fill_rectangle(rect,col);
}

//Used to show red briefly for incorrect solution
void fillGridBackground(uint16_t col){
  rectangle rect;
  rect.left = TOPLEFTX;
  rect.right = TOPLEFTX+(9*(CELLSIZE+1))+2;
  rect.top = TOPLEFTY;
  rect.bottom = TOPLEFTY + (9*(CELLSIZE+1))+2;
  fill_rectangle(rect,col);
}

void drawPuzzle(){
  uint8_t i;
  uint8_t j;
  
  /* Background Square */
  rectangle rect;
  rect.left = TOPLEFTX;
  rect.right = TOPLEFTX+(9*(CELLSIZE+1))+2;
  rect.top = TOPLEFTY;
  rect.bottom = TOPLEFTY + (9*(CELLSIZE+1))+2;
  fill_rectangle(rect,theme.cell_frame);
  
  for(j=0;j<9;j++){
    for(i=0;i<9;i++){
      
        /* Number Cells */
        rect.left = getCellX(i);
        rect.right = rect.left + CELLSIZE-1;
        rect.top = getCellY(j);
        rect.bottom = rect.top +CELLSIZE-1;
        fill_rectangle(rect,theme.cell_background);
        
        /* Number */
        if(numberMatrix[j][i]){
          
          if(boolMatrix[j][i]){
            display_color(theme.noneditable_numbers,theme.cell_background);
            char buffer[1];
            itoa(numberMatrix[j][i],buffer,10);
            display_string_xy(buffer, (getCellX(i) + (CELLSIZE/2)-2), (getCellY(j)+((CELLSIZE/2)-3)));
          } else {
            display_color(theme.editable_numbers,theme.cell_background);
            char buffer[1];
            itoa(numberMatrix[j][i],buffer,10);
            display_string_xy(buffer, (getCellX(i) + (CELLSIZE/2)-2), (getCellY(j)+((CELLSIZE/2)-3)));
            
          }
      }
    }
  }
}

void fillCell(uint8_t x,uint8_t y, uint16_t col){
  rectangle rect;
  rect.left = getCellX(x);
  rect.right = rect.left + CELLSIZE-1;
  rect.top = getCellY(y);
  rect.bottom = rect.top +CELLSIZE-1;
  fill_rectangle(rect,col);
}

void drawNumber(uint8_t x,uint8_t y, uint8_t number){
  char buffer[1];
  itoa(number,buffer,10);
  display_string_xy(buffer, (getCellX(x) + (CELLSIZE/2)-2), (getCellY(y)+((CELLSIZE/2)-3)));
}

void updatePointer(uint8_t oldx,uint8_t oldy){
  fillBoxOutline(oldx,oldy, theme.cell_frame);
  fillBoxOutline(pointerx,pointery,theme.cursor_border);
}

void fillBoxOutline(uint8_t x, uint8_t y, uint16_t col){
  rectangle rect;
  
  /*return outline of old box to theme.cell_frame*/
  /* Left wall */
  rect.left = getBoxX(x);
  rect.right = rect.left;
  rect.top = getBoxY(y);
  rect.bottom = rect.top + CELLSIZE + 1;
  fill_rectangle(rect,col);
  
  /* Right wall */
  rect.left = getBoxX(x) + CELLSIZE + 1;
  rect.right = rect.left;
  rect.top = getBoxY(y);
  rect.bottom = rect.top + CELLSIZE + 1;
  fill_rectangle(rect,col);
  
  /* Top wall */
  rect.left = getBoxX(x);
  rect.right = rect.left + CELLSIZE + 1;
  rect.top = getBoxY(y);
  rect.bottom = rect.top;
  fill_rectangle(rect,col);
      
  /* Bottom wall */
  rect.left = getBoxX(x);
  rect.right = rect.left + CELLSIZE + 1;
  rect.top = getBoxY(y) + CELLSIZE + 1;
  rect.bottom = rect.top;
  fill_rectangle(rect,col);
}  

uint8_t getBoxX(uint8_t i){
  return TOPLEFTX + (i*(CELLSIZE+1)) + ((uint8_t) (i/3));
}
 
uint8_t getBoxY(uint8_t i){
  return TOPLEFTY + (i*(CELLSIZE+1))+ ((uint8_t) (i/3));
}
  
uint8_t getCellX(uint8_t i){
  return getBoxX(i) + 1;
}

uint8_t getCellY(uint8_t i){
  return getBoxY(i) + 1;
}

uint8_t checkSolved(){
  uint8_t set[9];
  uint8_t arr[3][3][9];
  
  uint8_t i;
  uint8_t j;

  /* Checks all rows & columns */
  for(i =0;i<9;i++){
    if(!checkSet(numberMatrix[i])){ return 0;}    
    for(j = 0;j<9;j++){
      set[j] = numberMatrix[j][i];
    } 
    if(!checkSet(set)){ return 0;}
  }

  /* Checks each 3*3 square */
  /* I have no idea how I managed to make this code but its great */

  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      arr[i/3][j/3][(3*(i%3))+(j%3)] = numberMatrix[i][j];
    }
  }
  
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
        if(!checkSet(arr[i][j])){return 0;}
      } 
    }  
  return 1;
}

uint8_t checkSet(uint8_t set[9]){
  uint8_t sum = 0;
  uint8_t i;
  uint8_t j;
  for(i=0;i<9;i++){
    sum += set[i];
  }
  if(sum != 45) {return 0;}
  for(i=0;i<8;i++){
    for(j=i+1;j<9;j++){
      if(set[j] == set[i]) {return 0;}
    }
  }
  return 1;
}
