#include "check_value.h"
#include <iostream>

int check_value(int value, int min, int max)
{
    if (value >= min && value <= max)
    {
        return 1; // true
    }
    else
    {
        return 0; // false
    }
}

int is_colour_valid(int colour)
{
    int res = check_value(colour, 0, 0xffffff);
    if(res==0)
        std::cout << "colour isn't valid\n";
    return res;
}

int is_led_sel_valid(int led)
{
    int res = check_value(led, 0, 3);
    if(res==0)
        std::cout << "LED selection isn't valid\n";
    return res;
}

int is_cycle_speed_valid(int speed)
{
    int res = check_value(speed, 0, 10);
    if(res==0)
        std::cout << "cycle speed isn't valid\n";
    return res;
}

int is_breathe_speed_valid(int speed)
{
    int res = check_value(speed, 0, 10);
    if(res==0)
        std::cout << "breathe speed isn't valid\n";
    return res;
}

int is_brightness_valid(int brightness)
{
    int res = check_value(brightness, 0, 100);
    if(res==0)
        std::cout << "brightness isn't valid\n";
    return res;
}
