/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE(law): This pseudorandom number generation is based on the version
// described at http://burtleburtle.net/bob/rand/smallprng.html

#define ROTATE(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
static u64 random_value(random_entropy *entropy)
{
   u64 entropy_e = entropy->a - ROTATE(entropy->b, 27);
   entropy->a    = entropy->b ^ ROTATE(entropy->c, 17);
   entropy->b    = entropy->c + entropy->d;
   entropy->c    = entropy->d + entropy_e;
   entropy->d    = entropy_e  + entropy->a;

   return(entropy->d);
}
#undef ROTATE

static random_entropy random_seed(u64 seed)
{
   random_entropy result;
   result.a = 0xf1ea5eed;
   result.b = seed;
   result.c = seed;
   result.d = seed;

   for(u64 index = 0; index < 20; ++index)
   {
      random_value(&result);
   }

   return(result);
}

static u32 random_range(random_entropy *entropy, u32 minimum, u32 maximum)
{
   u64 value = random_value(entropy);
   u32 range = maximum - minimum + 1;

   u32 result = (u32)((value % (u64)range) + (u64)minimum);
   return(result);
}

static float random_unit_interval(random_entropy *entropy)
{
   u64 value = random_value(entropy);
   u64 maximum = UINT64_MAX;

   float result = (float)value / (float)maximum;
   return(result);
}
