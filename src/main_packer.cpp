/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This is our half-baked asset packer. Provide it a list of obj file
// names (without the .obj extensions) located in the data directory to generate
// the source code file src/assets.cpp with asset data embedded.

#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "platform_libc.cpp"

#include "memory.h"
#include "memory.cpp"

#include "math.h"

struct wavefront_mtl
{
   char *name;

   vec3 diffuse;
   char diffuse_hex[7];
};

static int gmaterial_count;
static wavefront_mtl gmaterials[256];

// TODO: This should be a hash table keyed on the name, if we ever care.
static wavefront_mtl *get_material_by_name(char *name)
{
   wavefront_mtl *result = 0;
   for(int index = 0; index < gmaterial_count; ++index)
   {
      if(strcmp(name, gmaterials[index].name) == 0)
      {
         result = gmaterials + index;
         break;
      }
   }

   return(result);
}

static void output_obj_code(FILE *out, memarena arena, char *basename)
{
   // NOTE: Reset material array.
   gmaterial_count = 0;

   char path[256] = {};

   // NOTE: Parse material file.
   snprintf(path, sizeof(path), "./data/%s.mtl", basename);

   FILE *file = fopen(path, "r");
   if(file)
   {
      wavefront_mtl *material = 0;

      char line[256];
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'n' &&
            line[1] == 'e' &&
            line[2] == 'w' &&
            line[3] == 'm' &&
            line[4] == 't' &&
            line[5] == 'l' &&
            line[6] == ' ')
         {
            // Material name.
            material = gmaterials + gmaterial_count++;

            char string_buffer[128];
            sscanf(line, "newmtl %s\n", string_buffer);

            memsize size = strlen(string_buffer) + 1;
            material->name = (char *)arena_allocate(&arena, size);
            memcpy(material->name, string_buffer, size);
         }
         else if(line[0] == 'K' && line[1] == 'd' && line[2] == ' ')
         {
            vec3 *color = &material->diffuse;
            sscanf(line, "Kd %f %f %f\n", &color->r, &color->g, &color->b);

            u8 r = (u8)(color->r * 255.0f);
            u8 g = (u8)(color->g * 255.0f);
            u8 b = (u8)(color->b * 255.0f);
            snprintf(material->diffuse_hex, 7, "%02x%02x%02x", r, g, b);
         }
      }

      fclose(file);
   }

   // NOTE: Parse mesh file.
   snprintf(path, sizeof(path), "./data/%s.obj", basename);

   file = fopen(path, "r");
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

      wavefront_mtl *current_material = 0;
      while(fgets(line, sizeof(line), file))
      {
         if(line[0] == 'u' &&
            line[1] == 's' &&
            line[2] == 'e' &&
            line[3] == 'm' &&
            line[4] == 't' &&
            line[5] == 'l' &&
            line[6] == ' ')
         {
            char name[128];
            sscanf(line, "usemtl %s\n", name);
            current_material = get_material_by_name(name);
         }
         else if(line[0] == 'f' && line[1] == ' ')
         {
            int v0, t0, n0;
            int v1, t1, n1;
            int v2, t2, n2;
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &v0, &t0, &n0,
                   &v1, &t1, &n1,
                   &v2, &t2, &n2);


            fprintf(out, "   {{%d, %d, %d}, {%d, %d, %d}, {%d, %d, %d}, 0x%.*sff},\n",
                    v0 - 1, v1 - 1, v2 - 1,
                    t0 - 1, t1 - 1, t2 - 1,
                    n0 - 1, n1 - 1, n2 - 1,
                    6, (current_material) ? current_material->diffuse_hex : "555555");
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
   memarena arena = arena_new(MEGABYTES(256));

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
      output_obj_code(out, arena, arguments[index]);
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
