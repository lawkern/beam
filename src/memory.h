#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#define KILOBYTES(n) (1024L * (n))
#define MEGABYTES(n) (1024L * KILOBYTES(n))
#define GIGABYTES(n) (1024L * MEGABYTES(n))

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