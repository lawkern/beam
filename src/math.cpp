/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include <math.h>

static vec3 operator+(vec3 a, vec3 b)
{
   vec3 result;
   result.x = a.x + b.x;
   result.y = a.y + b.y;

   return(result);
}

static vec3 operator*(vec3 vector, float scalar)
{
   vec3 result;
   result.x = vector.x * scalar;
   result.y = vector.y * scalar;

   return(result);
}

static int absolute_value(int value)
{
   return abs(value);
}
