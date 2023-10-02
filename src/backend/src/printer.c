#include <stdio.h>
//
#include "printer_constants.h"



void printf_red(const char* str)
{
    printf("%s%s%s", RED, str, DEFAULT_COLOR);
}
void printf_red_on()
{
    printf("%s", RED);
}
void printf_yellow(const char* str)
{
    printf("%s%s%s", YELLOW, str, DEFAULT_COLOR);
}
void printf_yellow_on()
{
    printf("%s", YELLOW);
}
void printf_green(const char* str)
{
    printf("%s%s%s", GREEN, str, DEFAULT_COLOR);
}
void printf_green_on()
{
    printf("%s", GREEN);
}
void printf_blue(const char* str)
{
    printf("%s%s%s", BLUE, str, DEFAULT_COLOR);
}
void printf_blue_on()
{
    printf("%s", BLUE);
}
void printf_cyan(const char* str)
{
    printf("%s%s%s", CYAN, str, DEFAULT_COLOR);
}
void printf_cyan_on()
{
    printf("%s", CYAN);
}
void printf_purple(const char* str)
{
    printf("%s%s%s", PURPLE, str, DEFAULT_COLOR);
}
void printf_purple_on()
{
    printf("%s", PURPLE);
}
void printf_white(const char* str)
{
    printf("%s%s%s", WHITE, str, DEFAULT_COLOR);
}
void printf_white_on()
{
    printf("%s", WHITE);
}
void printf_black(const char* str)
{
    printf("%s%s%s", BLACK, str, DEFAULT_COLOR);
}
void printf_black_on()
{
    printf("%s", BLACK);
}
void printf_color_reset()
{
    printf("%s", DEFAULT_COLOR);
}