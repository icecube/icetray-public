#include "I3Vectors.h"
#include <dataclasses/ScintKey.h>

void register_I3VectorScintKey()
{
  register_i3vector_of<ScintKey>("ScintKey");
}