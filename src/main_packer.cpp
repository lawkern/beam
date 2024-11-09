/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This is our half-baked asset packer. Provide it a list of obj file
// names (without the .obj extensions) located in the data directory to generate
// the source code file src/assets.cpp with asset data embedded.

#include <stdio.h>

static void output_obj_code(FILE *out, char *basename)
{
   char path[256] = {};
   snprintf(path, sizeof(path), "./data/%s.obj", basename);

   FILE *file = fopen(path, "r");
   if(file)
   {
      char line[256];

      // Vertices
      fprintf(out, "vec3 %s_vertices[] = {\n", basename);
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
      fprintf(out, "vec2 %s_texcoords[] = {\n", basename);
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
      fprintf(out, "vec3 %s_normals[] = {\n", basename);
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
      fprintf(out, "mesh_asset_face %s_faces[] = {\n", basename);
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
            fprintf(out, "   {{%d, %d, %d}, {%d, %d, %d}, {%d, %d, %d}, 0x00FFFFFF},\n",
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

   // NOTE: Output enum of mesh IDs.
   fprintf(out, "enum mesh_id\n");
   fprintf(out, "{\n");
   for(int index = 1; index < argument_count; ++index)
   {
      fprintf(out, "   MESHID_%s,\n", arguments[index]);
   }
   fprintf(out, "};\n\n");

   // NOTE: Output mesh struct literals.
   for(int index = 1; index < argument_count; ++index)
   {
      output_obj_code(out, arguments[index]);
   }

   // NOTE: Output loading function.
   fprintf(out, "static void load_assets(game_context *game)\n");
   fprintf(out, "{\n");
   for(int index = 1; index < argument_count; ++index)
   {
      if(index != 1) fprintf(out, "\n");

      int entity_index = index - 1;
      char *basename = arguments[index];

      fprintf(out, "   assert(%d < countof(game->entities));\n", entity_index);
      fprintf(out, "   game->meshes[%d].vertex_count   = countof(%s_vertices);\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].vertices       = %s_vertices;\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].texcoord_count = countof(%s_texcoords);\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].texcoords      = %s_texcoords;\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].normal_count   = countof(%s_normals);\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].normals        = %s_normals;\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].face_count     = countof(%s_faces);\n", entity_index, basename);
      fprintf(out, "   game->meshes[%d].faces          = %s_faces;\n", entity_index, basename);
   }
   fprintf(out, "}\n");

   fclose(out);

   return(0);
}
