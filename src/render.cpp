/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

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
      texture.memory[texture.width*y + x] = color;

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
      texture.memory[texture.width*y + x] = color;

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
   x0 = MINIMUM(MAXIMUM(x0, 0), texture.width - 1);
   y0 = MINIMUM(MAXIMUM(y0, 0), texture.height - 1);
   x1 = MINIMUM(MAXIMUM(x1, 0), texture.width - 1);
   y1 = MINIMUM(MAXIMUM(y1, 0), texture.height - 1);

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

static void draw_triangle(game_texture texture, render_triangle triangle)
{
   vec2 v0 = triangle.vertices[0];
   vec2 v1 = triangle.vertices[1];
   vec2 v2 = triangle.vertices[2];

   draw_line(texture, v0.x, v0.y, v1.x, v1.y, triangle.color);
   draw_line(texture, v1.x, v1.y, v2.x, v2.y, triangle.color);
   draw_line(texture, v2.x, v2.y, v0.x, v0.y, triangle.color);
}
