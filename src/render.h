#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

enum render_command_kind
{
   RENDERCOMMAND_CLEAR,
   RENDERCOMMAND_TRIANGLE,
};

struct render_command
{
   render_command_kind kind;
   union
   {
      u32 color;
      int index;
   };
};

struct render_triangle
{
   vec3 vertices[3];
   vec2 texcoords[3];
   u32 color;
};

struct render_polygon
{
   int vertex_count;
   vec3 vertices[10];
   vec2 texcoords[10];
   vec3 normal;
};

struct plane
{
   vec3 point;
   vec3 normal;
};

enum
{
   FRUSTUMPLANE_LEFT,
   FRUSTUMPLANE_RIGHT,
   FRUSTUMPLANE_TOP,
   FRUSTUMPLANE_BOTTOM,
   FRUSTUMPLANE_NEAR,
   FRUSTUMPLANE_FAR,

   FRUSTUMPLANE_COUNT,
};
