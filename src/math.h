#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

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
   struct {float r, g, b, a;};
};
