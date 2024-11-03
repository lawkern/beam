#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// IMPORTANT: Mesh vertices are stored using a right-hand coordinate system,
// where the positive x-direction is forward and positive z is up.

struct mesh_asset_face
{
   int vertex_indices[3];
   int texcoord_indices[3];
   int normal_indices[3];
   u32 color;
};

struct mesh_asset
{
   int vertex_count;
   vec3 *vertices;

   int texcoord_count;
   vec2 *texcoords;

   int normal_count;
   vec3 *normals;

   int face_count;
   mesh_asset_face *faces;
};
