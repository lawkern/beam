/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

vec3 cube_vertices[] = {
   {1.000000f, -1.000000f, 1.000000f},
   {1.000000f, -1.000000f, -1.000000f},
   {-1.000000f, -1.000000f, 1.000000f},
   {-1.000000f, -1.000000f, -1.000000f},
   {1.000000f, 1.000000f, 1.000000f},
   {1.000000f, 1.000000f, -1.000000f},
   {-1.000000f, 1.000000f, 1.000000f},
   {-1.000000f, 1.000000f, -1.000000f},
};

vec2 cube_texcoords[] = {
   {0.875000f, 0.500000f},
   {0.625000f, 0.750000f},
   {0.625000f, 0.500000f},
   {0.375000f, 1.000000f},
   {0.375000f, 0.750000f},
   {0.625000f, 0.000000f},
   {0.375000f, 0.250000f},
   {0.375000f, 0.000000f},
   {0.375000f, 0.500000f},
   {0.125000f, 0.750000f},
   {0.125000f, 0.500000f},
   {0.625000f, 0.250000f},
   {0.875000f, 0.750000f},
   {0.625000f, 1.000000f},
};

vec3 cube_normals[] = {
   {-0.000000f, -0.000000f, 1.000000f},
   {-1.000000f, -0.000000f, -0.000000f},
   {-0.000000f, 1.000000f, -0.000000f},
   {-0.000000f, -0.000000f, -1.000000f},
   {-0.000000f, -1.000000f, -0.000000f},
   {1.000000f, -0.000000f, -0.000000f},
};

mesh_asset_face cube_faces[] = {
   {{4, 2, 0}, {0, 1, 2}, {0, 0, 0}, 0xFFFFFFFF},
   {{2, 7, 3}, {1, 3, 4}, {1, 1, 1}, 0xFFFFFFFF},
   {{6, 5, 7}, {5, 6, 7}, {2, 2, 2}, 0xFFFFFFFF},
   {{1, 7, 5}, {8, 9, 10}, {3, 3, 3}, 0xFFFFFFFF},
   {{0, 3, 1}, {2, 4, 8}, {4, 4, 4}, 0xFFFFFFFF},
   {{4, 1, 5}, {11, 8, 6}, {5, 5, 5}, 0xFFFFFFFF},
   {{4, 6, 2}, {0, 12, 1}, {0, 0, 0}, 0xFFFFFFFF},
   {{2, 6, 7}, {1, 13, 3}, {1, 1, 1}, 0xFFFFFFFF},
   {{6, 4, 5}, {5, 11, 6}, {2, 2, 2}, 0xFFFFFFFF},
   {{1, 3, 7}, {8, 4, 9}, {3, 3, 3}, 0xFFFFFFFF},
   {{0, 2, 3}, {2, 1, 4}, {4, 4, 4}, 0xFFFFFFFF},
   {{4, 0, 1}, {11, 2, 8}, {5, 5, 5}, 0xFFFFFFFF},
};
