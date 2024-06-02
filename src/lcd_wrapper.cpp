#include<LiquidCrystal_I2C.h>
#include<Arduino.h>
/**
 * Initialize LCD
 *
 * Everything you need to initialize your LCD display, place here.
 * This function should be called in function setup().
 */
LiquidCrystal_I2C lcd(0x27, 16, 2);
void lcd_init(){
    lcd.init();
    lcd.begin(16,2);
    lcd.backlight();
    lcd.clear ();
}


/**
 * Clears the LCD display
 *
 * Clears content of the LCD display. After the call is display empty.
 */
void lcd_clear(){
    lcd.clear();
}


/**
 * Set cursor to given posititon
 *
 * Sets cursor to given position based on y and x coordinates.
 * @param y the row
 * @param x the column
 */
void lcd_set_cursor(int y, int x){
    lcd.setCursor(x,y);
}


/**
 * Print text on LCD display
 *
 * Prints the givent text on the display on the current
 * cursor position.
 * @param text the text to print
 */
void lcd_print(char* text){
    lcd.print(text);
}


/**
 * Set cursor to given positition and print text
 *
 * Sets the cursor to given position based on y and x coorindates
 * and then prints the given text.
 * @param y the row
 * @param x the column
 * @param text the text to print
 */
void lcd_print_at(int y, int x, char* text){
    lcd_set_cursor(y,x);
    lcd_print(text);
}



/**
 * Turns LCD backlight on/off.
 *
 *  Turns LCD backlight on or off based on the `state` parameter:
 *  - if set to true, backlight will be turned on
 *  - if set to false, backlight will be turned off
 *
 * @param state state of the backlight
 */
void lcd_backlight(bool state){
    lcd.setBacklight(state);
}