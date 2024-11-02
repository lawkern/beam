/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file is the platform API implementation based on C standard
// library functions. This exists to make it easier to get up and running on a
// new platform when porting. Ideally each port eventually replaces this with
// the equivalent platform_win32.cpp, platform_macos.cpp, etc.

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "platform.h"

PLATFORM_LOG(plog)
{
   va_list arguments;
   va_start(arguments, fmt);
   {
      vprintf(fmt, arguments);
   }
   va_end(arguments);
}

PLATFORM_ALLOCATE(pallocate)
{
   return calloc(1, size);
}

PLATFORM_DEALLOCATE(pdeallocate)
{
   if(memory)
   {
      free(memory);
   }
}
