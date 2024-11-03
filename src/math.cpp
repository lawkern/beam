/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include <math.h>

static vec4 v4(vec3 xyz, float w)
{
   vec4 result;
   result.xyz = xyz;
   result.w = w;

   return(result);
}

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

////////////////////////////////////////////////////////////////////////////////

static mat4 make_identity(void)
{
   mat4 result = {{
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_translation(float x, float y, float z)
{
   mat4 result = {{
      {1, 0, 0, x},
      {0, 1, 0, y},
      {0, 0, 1, z},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_scale(float x, float y, float z)
{
   mat4 result = {{
      {x, 0, 0, 0},
      {0, y, 0, 0},
      {0, 0, z, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_rotationx(float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   float n = -s;

   mat4 result = {{
      {1, 0, 0, 0},
      {0, c, n, 0},
      {0, s, c, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_rotationy(float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   float n = -s;

   mat4 result = {{
      {c, 0, s, 0},
      {0, 1, 0, 0},
      {n, 0, c, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_rotationz(float angle)
{
   float c = cosf(angle);
   float s = sinf(angle);
   float n = -s;

   mat4 result = {{
      {c, n, 0, 0},
      {s, c, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static vec4 mul(mat4 m, vec4 v)
{
   vec4 result;
   result.x = m.e[0][0]*v.x + m.e[0][1]*v.y + m.e[0][2]*v.z + m.e[0][3]*v.w;
   result.y = m.e[1][0]*v.x + m.e[1][1]*v.y + m.e[1][2]*v.z + m.e[1][3]*v.w;
   result.z = m.e[2][0]*v.x + m.e[2][1]*v.y + m.e[2][2]*v.z + m.e[2][3]*v.w;
   result.w = m.e[3][0]*v.x + m.e[3][1]*v.y + m.e[3][2]*v.z + m.e[3][3]*v.w;

   return(result);
}

static vec4 operator*(mat4 m, vec4 v) { return mul(m, v); }

static mat4 mul(mat4 a, mat4 b)
{
   mat4 result = {};
   for(int row = 0; row < 4; ++row)
   {
      for(int col = 0; col < 4; ++col)
      {
         for(int index = 0; index < 4; ++index)
         {
            result.e[row][col] += a.e[row][index]*b.e[index][col];
         }
      }
   }
   return(result);
}

static mat4 operator*(mat4 a, mat4 b) { return mul(a, b); }
static mat4 operator*=(mat4 &a, mat4 b) {a = mul(a, b); return(a);}
