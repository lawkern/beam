#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// IMPORTANT: World space and camera space are defined in terms of a
// right-handed coordinate system, where +x is forward, +z is up, and -y is
// right.

// IMPORTANT: Screen space (and textures) uses a left-handed coordinate system,
// where -z is forward, -y is up, and +x is right.

// IMPORTANT: Matrices are stored in row-major order.

struct vec2i
{
   int x;
   int y;
};

union vec2
{
   struct {float x, y;};
   struct {float u, v;};
};

union vec3
{
   struct {float x, y, z;};
   struct {vec2 xy; float z_;};
   struct {float x_; vec2 yz;};
   struct {float r, g, b;};
};

union vec4
{
   struct {float x, y, z, w;};
   struct {vec3 xyz; float w_;};
   struct {float r, g, b, a;};
};

union mat4
{
   vec4 rows[4];
   float e[4][4];
};
