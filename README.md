# FortunaSudoku

== Description ==
The aim of this project was to create a well-polished application for the LaFortuna to be entered in the category of best application.

== Running the game ==
The game can be ran using Klaus's universal makefile. This is included in the repository and so with your Fortuna connected to your computer and everything installed it can be flashed by typing make in the directory. 
You may need to switch the Fortuna to battery mode and back after flashing for the game to not flicker.

== Controls ==
The controls are very simple:
=== Menu Controls ===
* The up and down arrow keys are used to navigate.
* The centre button is used to select
* The rotary encoder can be used when hovering over the New Game option to alter the difficulty.

=== In-game Controls ===
* The up and down arrow keys are used to navigate.
* When the centre button is pressed, the current grid is saved to eeprom and the grid is checked to see whether it is the solution or not.
* When the current pointed to cell is editable the rotary encoder can be used to change the value of the number in this cell.

== Sudoku rules ==
*Each row and column should contain all of the numbers 1 through 9.
*Each 3*3 subgrid should contain all of the numbers 1 through 9.

== Puzzle Generation ==
All puzzles generated are true Sudoku puzzles in that they have a single unique solved state.
The method that I use to generate a puzzle:
# Start with a blank grid
# Use a backtracking algorithm to solve this grid, filling cells with a random order of numbers.
# Use another algorithm to remove random cells, then checking whether the puzzle still has only one solution. This stops at some point in the range of (20 + difficulty*2.8) ≤ number of missing cells ≤ (20 + difficulty*4).
In order to get a different seed for the random function, I read a int from eeprom to use as the seed, this is then incremented and re-written to eeprom.

== Themes ==
In-game, the theme can be changed through the menu. At the time of me creating this page, there are 7 different themes which come pre-loaded on the game (Thanks to Rayna Bozhkova for creating some of these!)- I am happy to add any themes which people make.
A int is stored in eeprom to indicate the selected theme, meaning if the Fortuna is turned off and on, the theme will be remembered.

Themes can be created very easily be editing the themes.h file.
When creating the theme just follow the format of the other adding themes (theme_name should be less than 11 characters), then increase the NUM_THEMES value and add the theme to the themeList.
Most of the colors used in the themes are from Klaus's svgrgb656.h file which is in the \lcd folder.

== Saving/loading game progress ==
The game can be saved by pressing the middle button. This saves the numberMatrix and the boolMatrix to eeprom. 
The same game (assuming it has not been overwritten in eeprom by another saved game or another application using eeprom) can then be loaded by pressing the 'Load Game' button on the menu.
In the case that the eeprom has been overwritten, the load button will show the user an error message when clicked and the user will be able to create a new game.

== Using your own grid ==
If you would like to supply your own Sudoku grid, rather than using a generated one:
# Open the Sudoku.c file in a text editor.
# Replace numberMatrix with the grid that you would like to use (0's are blank tiles).
# Update boolMatrix to set the editable and noneditable numbers (You can copy the values of numberMatrix if you want only the ones defined in numberMatrix editable and others noneditable.
# Change the value of PUZZLE_HARDCODED to 1.
# In the menu, select new game (Don't worry about the difficulty!).

== Changing the display size of the grid ==
If for some reason you would like to change the display size of the grid, you can do so by altering the value of CELLSIZE in Sudoku.c.
25 is the default cell-size and the largest that fits in the screen, 8 is the smallest before the text starts overlapping.
