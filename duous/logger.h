#pragma once
#include <stdlib.h>

#define DEPAREN(X) ESC(ISH X)
#define ISH(...) ISH __VA_ARGS__
#define ESC(...) ESC_(__VA_ARGS__)
#define ESC_(...) VAN##__VA_ARGS__
#define VANISH

#define _LOGFMT(tagfmt, msgfmt)     \
    do                              \
    {                               \
        char tag[50];               \
        char msg[200];              \
        snprintf(tag, 50, tagfmt);  \
        snprintf(msg, 100, msgfmt); \
        _log(tag, msg);             \
    } while (0)

#define LOGFMT(tagfmt, msgfmt) _LOGFMT(DEPAREN(tagfmt), DEPAREN(msgfmt))
#define SERVERTAG() ("SERVER-%x", server->id)
#define GAMETAG() ("GAME-%x", game->id)
#define LUATAG() ("Lua-Engine")
#define APPTAG() ("APPLICATION-%x", application->id)

void _log(const char *tag, const char *message);