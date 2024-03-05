#ifndef LCD_H 
#define LCD_H 

/*
functions for interacting with 16x2 lcd over  
*/

/*
desc: initializes lcd pins, prepares lcd for data, must be called before anything else
parameters: N/A 
output: N/A
*/
void lcd_init();

/*
desc: writes character buffer to lcd screen
parameters: 
  buffer: string to write to lcd display (must be 32 bytes long)
output: writes input buffer to lcd 
*/
void lcd_write(char*);

#endif