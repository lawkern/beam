/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "platform.h"

PLATFORM_LOG(plog)
{
   va_list arguments;
   va_start(arguments, fmt);
   {
      SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, arguments);
   }
   va_end(arguments);
}

PLATFORM_ALLOCATE(pallocate)
{
   return SDL_calloc(1, size);
}

PLATFORM_DEALLOCATE(pdeallocate)
{
   SDL_free(memory);
}
