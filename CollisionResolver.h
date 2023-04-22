#pragma once

#include "Body.h"
#include <vector>
#include <array>

// Idea is: During calculation of gravity, also work out which
//          bodies are colliding. Once processed, apply collisions

class CollisionResolver {
public:
  //CollisionResolver(size_t bodies_size) :
    // marked_(bodies_size, false), pairs_colliding_{} 
  //{}
  CollisionResolver() {}

  void process_pair(const size_t idx_a, const size_t idx_b,
                    Body& a, Body& b, const float distance) const;

private:
};
