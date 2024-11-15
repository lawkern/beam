/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

static void initialize_entities(game_context *game)
{
   int entity_index = 0;
   for(int index = 0; index < GAMECONTROLLER_COUNT_MAX; ++index)
   {
      assert(entity_index < countof(game->entities));
      entity *e = game->entities + entity_index++;

      e->facing_direction = v3(1, 0, 0);
      e->scale = v3(0.5, 0.5, 0.5);
      e->translation = v3(0, 0, 0);
      e->mesh_index = 1;
      e->active = false;
   }

   for(int index = 0; index < SERVERPLAYER_COUNT_MAX; ++index)
   {
      assert(entity_index < countof(game->entities));
      entity *e = game->entities + entity_index++;

      e->facing_direction = v3(1, 0, 0);
      e->scale = v3(0.5, 0.5, 0.5);
      e->translation = v3(0, 0, 0);
      e->mesh_index = 1;
      e->active = false;
   }

   for(int y = -10; y < 10; ++y)
   {
      for(int x = 2; x < 7; ++x)
      {
         assert(entity_index < countof(game->entities));
         entity *e = game->entities + entity_index++;

         e->translation = v3(5*x, 5*y, 0);
         e->scale = v3(0.5, 0.5, 0.5);
         e->mesh_index = 0;
         e->active = true;
      }
   }
}

static void update_entity(game_context *game, int entity_index, game_texture backbuffer)
{
   entity *e = game->entities + entity_index;
   if(e->active)
   {
      mesh_asset mesh = game->meshes[e->mesh_index];

      mat4 scale = make_scale(e->scale.x, e->scale.y, e->scale.z);
      mat4 rotationx = make_rotationx(e->rotation.x);
      mat4 rotationy = make_rotationy(e->rotation.y);
      mat4 rotationz = make_rotationz(e->rotation.z);
      mat4 translation = make_translation(e->translation.x, e->translation.y, e->translation.z);

      mat4 world = translation * scale * rotationx * rotationy * rotationz;

      for(int face_index = 0; face_index < mesh.face_count; ++face_index)
      {
         render_polygon polygon = make_polygon(&mesh, face_index);
         // clip_polygon(&polygon);

         int clip_triangle_count = 0;
         render_triangle clip_triangles[countof(polygon.vertices)];
         triangles_from_polygon(&clip_triangle_count, clip_triangles, &polygon);

         for(int clip_triangle_index = 0; clip_triangle_index < clip_triangle_count; ++clip_triangle_index)
         {
            render_triangle *clipped_triangle = clip_triangles + clip_triangle_index;

            assert(game->triangle_count < game->triangle_count_max);
            int triangle_index = game->triangle_count++;

            render_triangle *triangle = game->triangles + triangle_index;
            triangle->color = mesh.faces[face_index].color;

            for(int vertex_index = 0; vertex_index < 3; ++vertex_index)
            {
               // vec3 vertex = mesh.vertices[face.vertex_indices[vertex_index]];
               vec3 vertex = clipped_triangle->vertices[vertex_index];
               vertex *= world;
               vertex *= game->view;

               // NOTE: Project into clip coordinates.
               vertex = project(game->projection, vertex);

               // NOTE: Convert to screen coordinates.
               vertex.x *= (backbuffer.width / 2.0f);
               vertex.y *= -(backbuffer.height / 2.0f);

               vertex.x += (backbuffer.width / 2.0f);
               vertex.y += (backbuffer.height / 2.0f);

               triangle->vertices[vertex_index] = vertex;
            }

            push_triangle(game, triangle_index);
         }
      }
   }
}
