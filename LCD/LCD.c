#include <string.h>
#include <stdlib.h>

#include"ST7735.h"

/*
 * Converts an unsigned integer to a character so it can be displayed
 */
unsigned char hex2char(unsigned int hex)  {
    if(hex >=0x00 && hex <=0x09)
    {
        return (hex+0x30);
    }
    else if (hex >=0x0A && hex <=0x0F)
    {
        return (hex+0x37);
    }
    else
    {
        return 0xFF;
    }
}

/*
 * Write a string to the LCD display
 *
 * param msg: The string to be displayed
 * param numChars: The size of the input string (use strlen)
 * param yPosition: The y corridinate of the start of the string
 *                  some useful y positions:
 *                  0-10 for right near the top
 *                  50-60 is near the middle
 *                  100-110 is near the bottom
 *
 * param char_colour: The colour of the characters
 * param bg_colour: The colour of the background behind each character
 * param fontSize: The size of the string
 */
void writeLCD(char* msg, size_t numChars, int yPosition, unsigned int char_colour, unsigned int bg_colour, int fontSize) {

    int x = 10;
    int i;

    for(i = 0; i < numChars; i++) {

        ST7735_DrawChar(x, yPosition, msg[i], char_colour, bg_colour, fontSize);
        if(fontSize == 1) {

            x = x + 6;
        }

        else if(fontSize == 2) {

            x = x + 13;
        }
        else /*(fontsize = 3)*/ {

            x = x + 18;
        }
    }
}

/*
 * Remove a line of text from the LCD display. Useful for flashing
 *
 * param length: The length of the line to clear
 * param yPosition: The y position of the start of the line
 * param fontSize: the size of the font you are clearing
 */
void clearLCDLines(int yPosition, size_t length, int fontSize) {

    char *clear = " ";

    writeLCD(clear, strlen(clear), yPosition, ST7735_BLACK, ST7735_BLACK, fontSize);
}

/*
 * Initialise the LCD display by setting the colour of the display, the
 * initTAB, and orientation
 *
 * param displayColour: The initTAB. Can use any of INITR_BLACKTAB,
 * INITR_GREENTAB, or INITR_REDTAB
 *
 *
 * param rotation: The orientation of the screen. Valid range is 0 to 3
 */
void LCDinit(unsigned int displayColour, int rotation) {

    // refer to LCD function reference_v1.0.pdf
    // ***note that LCD initilization takes more than 1 sec***
    ST7735_InitR(INITR_BLACKTAB);
    ST7735_FillScreen(displayColour);
    ST7735_SetRotation(rotation);
}

/*
 * displays any 2-DIGIT number you give it on the LCD in the format 01, 02, 03,..., 99
 *
 * param num: The number to display. To display 0, enter -1.
 * param x and y: position on the screen
 * param char_colour: the colour of the number
 * param bg_colour: The background colour of the number
 * param fontsize: The size of the number being displayed.
 */
void display2dgNum(int num, int x, int y, unsigned int char_colour, unsigned int bg_colour, int fontSize) {

    int dg_1;
    int spacing = 3;

    /* determine the proper spacing */
    if(fontSize == 2) {

        spacing = 13;
    }
    else if(fontSize == 3) {

        spacing = 20;
    }

    if(num <= 100) {

        num++;
        dg_1 = num % 10;
    }
    if(num < 10) {

        ST7735_DrawChar(x + spacing, y, hex2char(num), ST7735_GREEN, ST7735_BLACK, fontSize);
        ST7735_DrawChar(x, y, '0', ST7735_GREEN, ST7735_BLACK, fontSize);
    }
    else if(num >= 10 && num < 20) {

        ST7735_DrawChar(x, y, hex2char(1), ST7735_GREEN, ST7735_BLACK, fontSize);
        ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
    }
    else if(num >= 20 && num < 30) {

        ST7735_DrawChar(x, y, hex2char(2), ST7735_GREEN, ST7735_BLACK, fontSize);
        ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
    }
    else if(num >= 30 && num < 40) {

       if(num > 30) {

           ST7735_DrawChar(x, y, hex2char(3), ST7735_GREEN, ST7735_BLACK, fontSize);
           ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
       }
       else {

           ST7735_DrawChar(x, y, hex2char(3), ST7735_GREEN, ST7735_BLACK, fontSize);
           ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
       }
   }
   else if(num >= 40 && num < 50) {

        ST7735_DrawChar(x, y, hex2char(4), ST7735_GREEN, ST7735_BLACK, fontSize);
        ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
   else if(num >= 50 && num < 60) {

        ST7735_DrawChar(x, y, hex2char(5), ST7735_GREEN, ST7735_BLACK, fontSize);
        ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
   else if(num >= 60 && num < 70) {

       ST7735_DrawChar(x, y, hex2char(6), ST7735_GREEN, ST7735_BLACK, fontSize);
       ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
   else if(num >= 70 && num < 80) {

       ST7735_DrawChar(x, y, hex2char(7), ST7735_GREEN, ST7735_BLACK, fontSize);
       ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
   else if(num >= 80 && num < 90) {

       ST7735_DrawChar(x, y, hex2char(8), ST7735_GREEN, ST7735_BLACK, fontSize);
       ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
   else if(num >= 90 && num < 100) {

       ST7735_DrawChar(x, y, hex2char(9), ST7735_GREEN, ST7735_BLACK, fontSize);
       ST7735_DrawChar(x + spacing, y, hex2char(dg_1), ST7735_GREEN, ST7735_BLACK, fontSize);
   }
}

/*
 * Displays any number in hexidecimal up to 4095 in the format 001, 002,.., 0A, 0B,..., FFF
 *
 * param num: The number to display in hex
 * param x: The x coordinate of the number. Some useful values for x are:
 *          10: First letter is at the left most side of the screen
 *
 * param y: The y coordinate of the number. Some useful values for y are:
 *          0-10: near the top
 *          50-60: near the middle
 *          100-110: near the bottom
 *
 * param char_colour: The colour of the character. You can choose from any of
 *                      the following macros defined in ST7735.h:
 *                      ST7735_BLACK
 *                      ST7735_BLUE
 *                      ST7735_RED
 *                      ST7735_GREEN
 *                      ST7735_CYAN
 *                      ST7735_MAGENTA
 *                      ST7735_YELLOW
 *                      ST7735_WHITE
 *
 * param bg_colour: The colour of the background on which the character sits.
 *                  Use any of the colours from the char_colour selection.
 *
 * param fontsize: The size of each character on the display. Valid values
 *                  are 1 - 3
 *
 */
void display12BitHex(int num, int x, int y, unsigned int char_colour, unsigned int bg_colour, int fontSize) {

    int i;
    int hex[3];
    /* Good practice not to edit paramters. A number to help along the */
    /* the conversion to hex */
    int hexConv = num;
    /*
     * hex[2] -> MSB of hex digit
     * hex[0] -> LSB of hex digit
     */

    /* Convert num to Hex */
    hex[0] = hexConv % 16;
    hexConv = hexConv / 16; /* integer division */
    hex[1] = hexConv % 16;
    hexConv = hexConv / 16;
    hex[2] = hexConv % 16;

    for(i = 2; i >= 0; i--) {

        ST7735_DrawChar(x, y, hex2char(hex[i]), char_colour, bg_colour, fontSize);

        if(fontSize == 1) {

            x = x + 9;
        }

        else if(fontSize == 2) {

            x = x + 13;
        }
        else /*(fontsize = 3)*/ {

            x = x + 18;
        }
    }
}
