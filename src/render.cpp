/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

static render_command *push_command(game_context *game, render_command_kind kind)
{
   assert(game->render_command_count < game->render_command_count_max);

   render_command *result = game->render_commands + game->render_command_count++;
   result->kind = kind;

   return(result);
}

static void push_clear(game_context *game, u32 color)
{
   render_command *command = push_command(game, RENDERCOMMAND_CLEAR);
   command->color = color;
}

static void push_triangle(game_context *game, int triangle_index)
{
   render_command *command = push_command(game, RENDERCOMMAND_TRIANGLE);
   command->index = triangle_index;
}

////////////////////////////////////////////////////////////////////////////////

static void draw_pixel_safely(game_texture texture, int x, int y, u32 color)
{
   // NOTE: This is super slow! We can do better testing for valid pixels.

   if(x >= 0 && x < texture.width && y >= 0 && y < texture.height)
   {
      texture.memory[texture.width*y + x] = color;
   }
}

static void clear(game_texture texture, u32 color)
{
   for(int y = 0; y < texture.height; ++y)
   {
      for(int x = 0; x < texture.width; ++x)
      {
         texture.memory[texture.width*y + x] = color;
      }
   }
}

static void draw_line_up(game_texture texture, int x0, int y0, int x1, int y1, u32 color)
{
   int dx = x1 - x0;
   int dy = y1 - y0;
   int xi = 1;
   if(dx < 0)
   {
      xi = -1;
      dx = -dx;
   }

   int difference = 2*dx - dy;
   int x = x0;

   for(int y = y0; y <= y1; ++y)
   {
      draw_pixel_safely(texture, x, y, color);

      if(difference > 0)
      {
         x += xi;
         difference += 2*(dx - dy);
      }
      else
      {
         difference += 2*dx;
      }
   }
}

static void draw_line_down(game_texture texture, int x0, int y0, int x1, int y1, u32 color)
{
   int dx = x1 - x0;
   int dy = y1 - y0;
   int yi = 1;
   if(dy < 0)
   {
      yi = -1;
      dy = -dy;
   }

   int difference = 2*dy - dx;
   int y = y0;

   for(int x = x0; x <= x1; ++x)
   {
      draw_pixel_safely(texture, x, y, color);

      if(difference > 0)
      {
         y += yi;
         difference += 2*(dy - dx);
      }
      else
      {
         difference += 2*dy;
      }
   }
}

static void draw_line(game_texture texture, int x0, int y0, int x1, int y1, u32 color)
{
   // x0 = MINIMUM(MAXIMUM(x0, 0), texture.width - 1);
   // y0 = MINIMUM(MAXIMUM(y0, 0), texture.height - 1);
   // x1 = MINIMUM(MAXIMUM(x1, 0), texture.width - 1);
   // y1 = MINIMUM(MAXIMUM(y1, 0), texture.height - 1);

   int xmin = MINIMUM(x0, x1);
   int ymin = MINIMUM(y0, y1);

   int xmax = MAXIMUM(x0, x1);
   int ymax = MAXIMUM(y0, y1);

   if(xmax >= 0 && ymax >= 0 && xmin < texture.width && ymin < texture.height)
   {
      if(absolute_value(y1 - y0) < absolute_value(x1 - x0))
      {
         if(x0 > x1)
         {
            draw_line_down(texture, x1, y1, x0, y0, color);
         }
         else
         {
            draw_line_down(texture, x0, y0, x1, y1, color);
         }
      }
      else
      {
         if(y0 > y1)
         {
            draw_line_up(texture, x1, y1, x0, y0, color);
         }
         else
         {
            draw_line_up(texture, x0, y0, x1, y1, color);
         }
      }
   }
}

static int orient2d(vec2i a, vec2i b, vec2i c)
{
   int result = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
   return(result);
}

static bool is_top_left(vec2i start, vec2i end)
{
   vec2i edge = end - start;

   bool is_top_edge = (edge.y == 0 && edge.x < 0);
   bool is_left_edge = (edge.y < 0);

   return(is_top_edge || is_left_edge);
}

static void draw_filled_triangle(game_texture texture, vec2i v0, vec2i v1, vec2i v2, u32 color)
{
   //NOTE: Compute bounding box.
   int xmin = MINIMUM(MINIMUM(v0.x, v1.x), v2.x);
   int ymin = MINIMUM(MINIMUM(v0.y, v1.y), v2.y);

   int xmax = MAXIMUM(MAXIMUM(v0.x, v1.x), v2.x);
   int ymax = MAXIMUM(MAXIMUM(v0.y, v1.y), v2.y);

   // NOTE: Clips against the screen boundaries.
   xmin = MAXIMUM(xmin, 0);
   ymin = MAXIMUM(ymin, 0);

   xmax = MINIMUM(xmax, texture.width - 1);
   ymax = MINIMUM(ymax, texture.height - 1);

   // NOTE: Compute fill rule biases.
   int bias0 = is_top_left(v1, v2) ? 0 : -1;
   int bias1 = is_top_left(v2, v0) ? 0 : -1;
   int bias2 = is_top_left(v0, v1) ? 0 : -1;

   // NOTE: Setup up triangle computations.
   int a01 = v0.y - v1.y;
   int b01 = v1.x - v0.x;

   int a12 = v1.y - v2.y;
   int b12 = v2.x - v1.x;

   int a20 = v2.y - v0.y;
   int b20 = v0.x - v2.x;

   vec2i point = {xmin, ymin};
   int w0_row = orient2d(v1, v2, point) + bias0;
   int w1_row = orient2d(v2, v0, point) + bias1;
   int w2_row = orient2d(v0, v1, point) + bias2;

   // TODO: Subpixel precision/correction.

   // NOTE: Rasterize.
   for(point.y = ymin; point.y <= ymax; point.y++)
   {
      int w0 = w0_row;
      int w1 = w1_row;
      int w2 = w2_row;

      for(point.x = xmin; point.x <= xmax; point.x++)
      {
         if((w0 | w1 | w2) >= 0)
         {
            texture.memory[texture.width*point.y + point.x] = color;
         }

         w0 += a12;
         w1 += a20;
         w2 += a01;
      }

      w0_row += b12;
      w1_row += b20;
      w2_row += b01;
   }
}

static void draw_triangle(game_texture texture, render_triangle triangle)
{
#if 0
   vec2 v0 = triangle.vertices[0].xy;
   vec2 v1 = triangle.vertices[1].xy;
   vec2 v2 = triangle.vertices[2].xy;

   draw_line(texture, v0.x, v0.y, v1.x, v1.y, triangle.color);
   draw_line(texture, v1.x, v1.y, v2.x, v2.y, triangle.color);
   draw_line(texture, v2.x, v2.y, v0.x, v0.y, triangle.color);
#else
   vec2i v0 = {(int)triangle.vertices[0].x, (int)triangle.vertices[0].y};
   vec2i v1 = {(int)triangle.vertices[1].x, (int)triangle.vertices[1].y};
   vec2i v2 = {(int)triangle.vertices[2].x, (int)triangle.vertices[2].y};

   draw_filled_triangle(texture, v0, v1, v2, triangle.color);
#endif
}

static void draw_debug_triangles(game_context *game)
{
   int debug_triangle_count = 30;
   for(int index = 0; index < debug_triangle_count; ++index)
   {
      assert(game->triangle_count < game->triangle_count_max);
      int triangle_index = game->triangle_count++;

      render_triangle *triangle = game->triangles + triangle_index;
      triangle->color = 0x00FF00FF;

      vec2i origin = {30, 30};
      int half_dim = 20;
      int offsetx = index * 15;
      int offsety = 0;

      triangle->vertices[0].x = offsetx + origin.x;
      triangle->vertices[0].y = offsety + origin.y - half_dim;

      triangle->vertices[1].x = offsetx + origin.x - half_dim;
      triangle->vertices[1].y = offsety + origin.y + half_dim;

      triangle->vertices[2].x = offsetx + origin.x + half_dim;
      triangle->vertices[2].y = offsety + origin.y + half_dim;

      push_triangle(game, triangle_index);
   }
}

static plane gfrustum_planes[FRUSTUMPLANE_COUNT];

void initialize_frustum_planes(float aspect_width_over_height, float fov, float near, float far)
{
   float fov_vert = fov;
   float fov_hori = 2.0f * arctangent(aspect_width_over_height * tangent(fov_vert / 2.0f));

   float ch = cosine(0.5f * fov_hori);
   float sh = sine(0.5f * fov_hori);

   float cv = cosine(0.5f * fov_vert);
   float sv = sine(0.5f * fov_vert);

   gfrustum_planes[FRUSTUMPLANE_LEFT].point = v3(0, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_LEFT].normal = v3(sh, ch, 0);

   gfrustum_planes[FRUSTUMPLANE_RIGHT].point = v3(0, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_RIGHT].normal = v3(sh, -ch, 0);

   gfrustum_planes[FRUSTUMPLANE_TOP].point = v3(0, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_TOP].normal = v3(sv, 0, -cv);

   gfrustum_planes[FRUSTUMPLANE_BOTTOM].point = v3(0, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_BOTTOM].normal = v3(sv, 0, cv);

   gfrustum_planes[FRUSTUMPLANE_NEAR].point = v3(near, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_NEAR].normal = v3(1, 0, 0);

   gfrustum_planes[FRUSTUMPLANE_FAR].point = v3(far, 0, 0);
   gfrustum_planes[FRUSTUMPLANE_FAR].normal = v3(-1, 0, 0);
}

render_polygon make_polygon(mesh_asset *mesh, int face_index)
{
   mesh_asset_face face = mesh->faces[face_index];

   render_polygon result = {};
   result.vertex_count = 3;

   result.vertices[0] = mesh->vertices[face.vertex_indices[0]];
   result.vertices[1] = mesh->vertices[face.vertex_indices[1]];
   result.vertices[2] = mesh->vertices[face.vertex_indices[2]];

   result.texcoords[0] = mesh->texcoords[face.texcoord_indices[0]];
   result.texcoords[1] = mesh->texcoords[face.texcoord_indices[1]];
   result.texcoords[2] = mesh->texcoords[face.texcoord_indices[2]];

   assert(face.normal_indices[0] == face.normal_indices[1] &&
          face.normal_indices[0] == face.normal_indices[2]);

   result.normal = mesh->normals[face.normal_indices[0]];

   return(result);
}

static void clip_polygon_plane(render_polygon *polygon, int plane_index)
{
   vec3 plane_point = gfrustum_planes[plane_index].point;
   vec3 plane_normal = gfrustum_planes[plane_index].normal;

   int inside_count = 0;
   vec3 inside_vertices[countof(polygon->vertices)];
   vec2 inside_texcoords[countof(polygon->texcoords)];

   vec3 *current_vertex = polygon->vertices + 0;
   vec2 *current_texcoord = polygon->texcoords + 0;

   vec3 *previous_vertex = polygon->vertices + (polygon->vertex_count - 1);
   vec2 *previous_texcoord = polygon->texcoords + (polygon->vertex_count - 1);

   // float current_dot = dot(*current_vertex - plane_point, plane_normal);
   float previous_dot = dot(*previous_vertex - plane_point, plane_normal);

   while(current_vertex != (polygon->vertices + polygon->vertex_count))
   {
      float current_dot = dot(*current_vertex - plane_point, plane_normal);

      if((current_dot * previous_dot) < 0)
      {
         float t = previous_dot / (previous_dot - current_dot);
         vec3 intersection_point = lerp(*previous_vertex, *current_vertex, t);
         vec2 interpolated_texcoord = lerp(*previous_texcoord, *current_texcoord, t);

         int index = inside_count++;
         inside_vertices[index] = intersection_point;
         inside_texcoords[index] = interpolated_texcoord;
      }

      if(current_dot > 0)
      {
         int index = inside_count++;
         inside_vertices[index] = *current_vertex;
         inside_texcoords[index] = *current_texcoord;
      }

      // NOTE: Iterate.
      previous_dot = current_dot;

      previous_vertex = current_vertex;
      previous_texcoord = current_texcoord;

      current_vertex++;
      current_texcoord++;
   }

   // NOTE: Store new vertices overtop the old set.
   for(int vertex_index = 0; vertex_index < inside_count; ++vertex_index)
   {
      polygon->vertices[vertex_index] = inside_vertices[vertex_index];
      polygon->texcoords[vertex_index] = inside_texcoords[vertex_index];
   }
   polygon->vertex_count = inside_count;
}

static void clip_polygon(render_polygon *polygon)
{
   clip_polygon_plane(polygon, FRUSTUMPLANE_LEFT);
   clip_polygon_plane(polygon, FRUSTUMPLANE_RIGHT);
   clip_polygon_plane(polygon, FRUSTUMPLANE_TOP);
   clip_polygon_plane(polygon, FRUSTUMPLANE_BOTTOM);
   clip_polygon_plane(polygon, FRUSTUMPLANE_NEAR);
   clip_polygon_plane(polygon, FRUSTUMPLANE_FAR);
}

static void triangles_from_polygon(int *triangle_count, render_triangle *triangles, render_polygon *polygon)
{
   *triangle_count = polygon->vertex_count - 2;

   for(int vertex_index = 0; vertex_index < *triangle_count; ++vertex_index)
   {
      int vertex_index0 = 0;
      int vertex_index1 = vertex_index + 1;
      int vertex_index2 = vertex_index + 2;

      triangles[vertex_index].vertices[0] = polygon->vertices[vertex_index0];
      triangles[vertex_index].vertices[1] = polygon->vertices[vertex_index1];
      triangles[vertex_index].vertices[2] = polygon->vertices[vertex_index2];

      triangles[vertex_index].texcoords[0] = polygon->texcoords[vertex_index0];
      triangles[vertex_index].texcoords[1] = polygon->texcoords[vertex_index1];
      triangles[vertex_index].texcoords[2] = polygon->texcoords[vertex_index2];
   }
}
