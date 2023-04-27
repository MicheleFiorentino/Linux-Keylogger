#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED
#include <linux/input.h>

//value meanings
#define PRESSED 1
#define RELEASED 0

//special cases
#define CTRL_L 200      
#define SHIFT_L 201     
#define SHIFT_R 202     
#define ALT_L 203       
#define CAPSLOCK 204        

//define values for special cases
#define VAL_CTRL_L 1        //0000 0001
#define VAL_SHIFT_L 2       //0000 0010
#define VAL_SHIFT_R  4      //0000 0100
#define VAL_ALT_L 8         //0000 1000
#define VAL_CAPSLOCK 16     //0001 0000

//contains combinations of special cases, es. CTRL_L + ALT_L = 0000 1001
char status = 0;        

//CAPSLOCK is toggled everytime the button is pressed
char is_caps_on = 0;

unsigned char dict[] = {
    0,      //RESERVED
    27,     //ESC
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    8,      //BACKSPACE
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',    
    ']',
    13,     //ENTER
    CTRL_L,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    39,     //APOSTROPHE
    '`',     //GRAVE 
    SHIFT_L,
    92,     //BACKSLASH
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '-',
    SHIFT_R,
    '*',
    ALT_L,
    ' ',
    CAPSLOCK
};

//it verify if ch is a special character, returning its status
//otherwise, it returns 0 (no effect)
unsigned char getNewStatus(unsigned char ch){
    switch(ch){
        case CTRL_L:
            return VAL_CTRL_L;      //0000 0001
            break;
        case SHIFT_L:
            return VAL_SHIFT_L;     //0000 0010
            break;
        case SHIFT_R:
            return VAL_SHIFT_R;     //0000 0100
            break;
        case ALT_L:
            return VAL_ALT_L;       //0000 1000
            break;
        case CAPSLOCK:
            return VAL_CAPSLOCK;    //0001 0000
            break;
        default:
            return 0;    //no effect
            break;
    }
}

unsigned char getShiftChar(unsigned char ch){
    if(ch >= 97 && ch <= 122)   //from lowercase to uppercase
        return ch-32;
    
    if(ch >= 65 && ch <= 90)   //from uppercase to lowercase
        return ch+32;
    
    //find SHIFT_L + ch
    switch(ch){
        case '`': return 126; break;    //` -> ˜
        case '1': return '!'; break;
        case '2': return '@'; break;
        case '3': return '#'; break;
        case '4': return '$'; break;
        case '5': return '%'; break;
        case '6': return 94; break;     //6 -> ^
        case '7': return '&'; break;
        case '8': return '*'; break;
        case '9': return '('; break;
        case '0': return ')'; break;
        case '-': return '_'; break;
        case '=': return '+'; break;    
        case '[': return '{'; break;
        case ']': return '}'; break;
        case ';': return ':'; break;
        case '\'': return '"'; break;
        case '\\': return '|'; break;
        case '<': return '>'; break;
        case ',': return '<'; break;
        case '.': return '>'; break;
        case '/': return '?'; break;
        default: return ch;
    }
}

char getCodeMeaning(int code, int value){
    unsigned char ch = dict[code];

    //verifying statuses that could change ch
    if(value == PRESSED){
        status += getNewStatus(ch);
        if(ch == CAPSLOCK)
            is_caps_on = !(is_caps_on);
    }
    else{      //RELEASED
        status -= getNewStatus(ch);
    }

    //change lowercase to uppercase if CAPSLOCK is on
    if(is_caps_on && ch >= 97 && ch <= 122)
        ch -= 32;
    
    switch(status){
        case VAL_SHIFT_L:
            ch = getShiftChar(ch);
            break;
    }

    return ch;
}

#endif