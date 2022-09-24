#pragma once
enum Color
{
    BLACK,
    NAVY,
    GREEN,
    TEAL,
    MAROON,
    PURPLE,
    OLIVE,
    SILVER,
    GREY,
    BLUE,
    LIME,
    AQUA,
    RED,
    PINK,
    YELLOW,
    WHITE
};

/* clear console content */
void clear(void);
/* reset foreground and background color */
void reset(void);
/* set foreground color */
void setcolor(enum Color color);
/* set background color */
void setbgcolor(enum Color color);
/* set cursor position */
void setcurpos(int x, int y);
