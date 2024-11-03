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
   u32 color;
};
