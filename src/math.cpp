/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include <math.h>

// NOTE: Our trig functions are defined in terms of turns, when the typical
// range of rotation 0 to tau maps to 0 to 1. This choice was mainly made to
// make inspecting angles of rotation more intuitive: e.g. 90 degrees maps to
// 0.25 turns instead of 1.570796... (tau/4) radians.

#define TAU32 6.283185f
static float sine(float turns)
{
   float result = sinf(TAU32 * turns);
   return(result);
}

static float cosine(float turns)
{
   float result = cosf(TAU32 * turns);
   return(result);
}

static float tangent(float turns)
{
   float result = tanf(TAU32 * turns);
   return(result);
}
#undef TAU32

static int absolute_value(int value)
{
   return abs(value);
}

static float square_root(float value)
{
   return sqrtf(value);
}

////////////////////////////////////////////////////////////////////////////////

static vec2 v2(float x, float y)
{
   vec2 result;
   result.x = x;
   result.y = y;

   return(result);
}

static vec3 v3(float x, float y, float z)
{
   vec3 result;
   result.x = x;
   result.y = y;
   result.z = z;

   return(result);
}

static vec4 v4(vec3 xyz, float w)
{
   vec4 result;
   result.xyz = xyz;
   result.w = w;

   return(result);
}

static vec2 operator+(vec2 a, vec2 b)
{
   vec2 result = {a.x + b.x, a.y + b.y};
   return(result);
}
static vec2 operator-(vec2 a, vec2 b)
{
   vec2 result = {a.x - b.x, a.y - b.y};
   return(result);
}
static vec2 operator*(vec2 v, float s)
{
   vec2 result = {s*v.x, s*v.y};
   return(result);
}
static vec2 operator/(vec2 v, float s)
{
   float inv = 1.0f / s;
   vec2 result = {inv*v.x, inv*v.y};
   return(result);
}

static vec3 operator+(vec3 a, vec3 b)
{
   vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
   return(result);
}
static vec3 operator-(vec3 a, vec3 b)
{
   vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
   return(result);
}
static vec3 operator*(vec3 v, float s)
{
   vec3 result = {s*v.x, s*v.y, s*v.z};
   return(result);
}
static vec3 operator/(vec3 v, float s)
{
   float inv = 1.0f / s;
   vec3 result = {inv*v.x, inv*v.y, inv*v.z};
   return(result);
}

static vec4 operator+(vec4 a, vec4 b)
{
   vec4 result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
   return(result);
}
static vec4 operator-(vec4 a, vec4 b)
{
   vec4 result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
   return(result);
}
static vec4 operator*(vec4 v, float s)
{
   vec4 result = {s*v.x, s*v.y, s*v.z, s*v.w};
   return(result);
}
static vec4 operator/(vec4 v, float s)
{
   float inv = 1.0f / s;
   vec4 result = {inv*v.x, inv*v.y, inv*v.z, inv*v.w};
   return(result);
}

static vec2 operator*(float s, vec2 v) {return(v * s); }
static vec2 operator+=(vec2 &a, vec2 b) { a = a + b; return(a); }
static vec2 operator-=(vec2 &a, vec2 b) { a = a - b; return(a); }
static vec2 operator*=(vec2 &v, float s) { v = v * s; return(v); }
static vec2 operator/=(vec2 &v, float s) { v = v / s; return(v); }

static vec3 operator*(float s, vec3 v) {return(v * s); }
static vec3 operator+=(vec3 &a, vec3 b) { a = a + b; return(a); }
static vec3 operator-=(vec3 &a, vec3 b) { a = a - b; return(a); }
static vec3 operator*=(vec3 &v, float s) { v = v * s; return(v); }
static vec3 operator/=(vec3 &v, float s) { v = v / s; return(v); }

static vec4 operator*(float s, vec4 v) {return(v * s); }
static vec4 operator+=(vec4 &a, vec4 b) { a = a + b; return(a); }
static vec4 operator-=(vec4 &a, vec4 b) { a = a - b; return(a); }
static vec4 operator*=(vec4 &v, float s) { v = v * s; return(v); }
static vec4 operator/=(vec4 &v, float s) { v = v / s; return(v); }

static float length(vec2 v)
{
   return square_root(v.x*v.x + v.y*v.y);
}

static float length(vec3 v)
{
   return square_root(v.x*v.x + v.y*v.y + v.z*v.z);
}

static vec2 normalize(vec2 v)
{
   vec2 result = {};

   float len = length(v);
   if(len != 0.0f) result = v / len;

   return(result);
}

static vec3 normalize(vec3 v)
{
   vec3 result = {};

   float len = length(v);
   if(len != 0.0f) result = v / len;

   return(result);
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

static mat4 make_rotationx(float turns)
{
   float c = cosine(turns);
   float s = sine(turns);
   float n = -s;

   mat4 result = {{
      {1, 0, 0, 0},
      {0, c, n, 0},
      {0, s, c, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_rotationy(float turns)
{
   float c = cosine(turns);
   float s = sine(turns);
   float n = -s;

   mat4 result = {{
      {c, 0, s, 0},
      {0, 1, 0, 0},
      {n, 0, c, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

static mat4 make_rotationz(float turns)
{
   float c = cosine(turns);
   float s = sine(turns);
   float n = -s;

   mat4 result = {{
      {c, n, 0, 0},
      {s, c, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
   }};

   return(result);
}

////////////////////////////////////////////////////////////////////////////////

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
