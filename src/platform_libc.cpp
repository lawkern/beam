/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "platform.h"

#include <stdarg.h>
#include <stdlib.h>

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
   free(memory);
}
