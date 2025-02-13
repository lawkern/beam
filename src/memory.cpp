/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// arena_new allocates a new fixed-size memory arena.
// This should be called once at startup for each arena used by the program.
static memarena arena_new(memsize size)
{
   memarena result = {};

   result.base = (u8 *)platform_allocate(size);
   if(result.base)
   {
      result.size = size;
   }
   else
   {
      platform_log("ERROR: Failed to allocate arena.\n");
   }

   return(result);
}

#define arena_struct(arena, type) (type *)arena_allocate((arena), sizeof(type))
#define arena_array(arena, type, count) (type *)arena_allocate((arena), sizeof(type) * (count))

// arena_allocate allocates the requested number of bytes from a give arena.
// If not enough space is available, it returns 0.
static void *arena_allocate(memarena *arena, memsize size)
{
   void *result = 0;
   if(size <= (arena->size - arena->used))
   {
      result = (u8 *)arena->base + arena->used;
      arena->used += size;
   }
   return(result);
}

// arena_reset deallocates everything from a given arena.
// Note that the memory buffer itself is not cleared in any way.
static void arena_reset(memarena *arena)
{
   arena->used = 0;
}
