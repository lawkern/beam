#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#define KILOBYTES(n) (1024LL * (n))
#define MEGABYTES(n) (1024LL * KILOBYTES(n))
#define GIGABYTES(n) (1024LL * MEGABYTES(n))

struct memarena
{
   u8 *base;
   memsize size;
   memsize used;
};

struct memmarker
{
   memarena *arena;
   memsize used;
};
