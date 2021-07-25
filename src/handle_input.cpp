#include <iostream>
#include <functional>
#include <string>
#include <cstring>
#include "handle_input.h"
#include "check_value.h"


bool is_int(const char * str)
{
    char * check;
    strtol(str, &check, 10);
    return (*check == 0);
}

bool is_hex(const char * str)
{
    char * check;
    strtol(str, &check, 16);
    return (*check == 0);
}

constexpr unsigned int hash(const char *str, int offset = 0)
{
    return !str[offset] ? 5381 : (hash(str, offset+1)*33) ^ str[offset];
}

int check_empty(int argc, char ** argv)
{
    if (argc <= 1)
    {
        return 1;
    }
    return 0;
}

int process_pasture(int argc, char** argv, int * res_buff)
{
    int pasture_colours[] = {1, 4, 0, 0x33d17a, 1, 0x33d17a, 2, 0x3584e4, 3, 0x3584e4};
    std::memcpy(res_buff, pasture_colours, 10*sizeof(int));
    return 0;
}

int process_solid(int argc, char** argv, int * res_buff)
{
    if ((!(argc % 2 == 0)) || (argc <= 2) || (argc > 10))
    {
        std::cout << "wrong number of arguments for solid option.\n";
        return 1;
    }

    res_buff[0] = SOLID_ID;
    res_buff[1] = ((argc - 2) / 2);

    for (int i = 2, j = 2; i < argc; i += 2, j += 2)
    {
        if (!(is_int(argv[i]) && (is_led_sel_valid(strtol(argv[i], NULL, 10)))))
        {
            std::cout << "LED selection needs to be a number from 0 to 3 inclusive.\n";
            return 1;
        }
        if(!(is_hex(argv[i+1]) && (is_colour_valid(strtol(argv[i+1], NULL, 16)))))
        {
            std::cout << "Colour selection needs to be a hex number from 0 to ffffff inclusive.\n";
            return 1;
        }

        res_buff[j]   = strtol(argv[i],   NULL, 10);
        res_buff[j+1] = strtol(argv[i+1], NULL, 16);
    }

    return 0;
}

int check_type_of_input(int argc, char ** argv, int * res_buff)
{
    char * kind_of_input = argv[1];
    int res = 0;

    switch( hash(kind_of_input) ){
    case hash("pasture") :
        std::cout << "pasture was entered\n";
        res = process_pasture(argc, argv, res_buff);
        break;
    case hash("solid") :
        std::cout << "solid was entered\n";
        res = process_solid(argc, argv, res_buff);
        if(res != 0)
            return 1;
        break;
    default:
        std::cout << "unknown input pattern\n" << "take a look at the README.md to see example inputs\n";
        return 1;
    }

    return 0;
}



int process_input(int argc, char** argv, int * res_buff, int * r_val)
{
    int result = 0;
    if (check_empty(argc, argv) != 0)
    {
        std::cerr << "No arguments found for program, stopping early.\n";
        r_val[0] = 1;
        return 1;
    }
    result = check_type_of_input(argc, argv, res_buff);
    if (result)
    {
        r_val[0] = 1;
        return 1;
    }


    return result;
}