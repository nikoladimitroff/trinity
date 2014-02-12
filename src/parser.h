#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <iostream>
#include <string.h>
#include "SDL_net.h"

using namespace std;

static IPaddress getMasterIp(int argc, char** argv )
{
    IPaddress ip;

    if(SDLNet_ResolveHost(&ip, argv[3], 2222)==-1)
    {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(1);
    }

    return ip;
}

static bool isSlave(char** argv)
{
    if(!strcmp(argv[2],"-s") || !strcmp(argv[2],"--slave"))
    {
        return true;
    }
    return false;
}

static bool isMaster(char** argv)
{
    if(!strcmp(argv[2],"-m") || !strcmp(argv[2],"--master"))
    {
        return true;
    }
    return false;
}

#endif // PARSER_H_INCLUDED
