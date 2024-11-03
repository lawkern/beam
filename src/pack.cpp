/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>

static void convert_obj(FILE *out, char *filename)
{
   char path[256] = {};
   snprintf(path, sizeof(path), "./data/%s", filename);

   FILE *file = fopen(path, "r");
   if(file)
   {
      char prefix[256] = {};
      for(int index = 0; filename[index] && filename[index] != '.'; ++index)
      {
         prefix[index] = filename[index];
      }

      char line[256];

      // Vertices
      fprintf(out, "vec3 %s_vertices[] = {\n", prefix);
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'v' && line[1] == ' ')
         {
            float x, y, z;
            sscanf(line, "v %f %f %f\n", &x, &y, &z);
            fprintf(out, "   {%ff, %ff, %ff},\n", x, y, z);
         }
      }
      fprintf(out, "};\n\n");
      fseek(file, 0, SEEK_SET);

      // Texcoords
      fprintf(out, "vec2 %s_texcoords[] = {\n", prefix);
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'v' && line[1] == 't' && line[2] == ' ')
         {
            float x, y;
            sscanf(line, "vt %f %f\n", &x, &y);
            fprintf(out, "   {%ff, %ff},\n", x, y);
         }
      }
      fprintf(out, "};\n\n");
      fseek(file, 0, SEEK_SET);

      // Normals
      fprintf(out, "vec3 %s_normals[] = {\n", prefix);
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
         {
            float x, y, z;
            sscanf(line, "vn %f %f %f\n", &x, &y, &z);
            fprintf(out, "   {%ff, %ff, %ff},\n", x, y, z);
         }
      }
      fprintf(out, "};\n\n");
      fseek(file, 0, SEEK_SET);

      // Faces
      fprintf(out, "mesh_asset_face %s_faces[] = {\n", prefix);
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'f' && line[1] == ' ')
         {
            int v0, t0, n0;
            int v1, t1, n1;
            int v2, t2, n2;
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &v0, &t0, &n0,
                   &v1, &t1, &n1,
                   &v2, &t2, &n2);
            fprintf(out, "   {{%d, %d, %d}, {%d, %d, %d}, {%d, %d, %d}, 0xFFFFFFFF},\n",
                    v0 - 1, v1 - 1, v2 - 1,
                    t0 - 1, t1 - 1, t2 - 1,
                    n0 - 1, n1 - 1, n2 - 1);
         }
      }
      fprintf(out, "};\n\n");
      fseek(file, 0, SEEK_SET);

      fclose(file);
   }
}

int main(int argument_count, char **arguments)
{
   FILE *out = fopen("./src/assets.cpp", "w");

   fprintf(out, "/* /////////////////////////////////////////////////////////////////////////// */\n");
   fprintf(out, "/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */\n");
   fprintf(out, "/* /////////////////////////////////////////////////////////////////////////// */\n");
   fprintf(out, "\n");

   for(int index = 1; index < argument_count; ++index)
   {
      convert_obj(out, arguments[index]);
   }

   fclose(out);

   return(0);
}
