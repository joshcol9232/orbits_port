#pragma once

#include "Body.h"
#include <vector>
#include <array>

// Idea is: During calculation of gravity, also work out which
//          bodies are colliding. Once processed, apply collisions

class CollisionResolver {
public:
  CollisionResolver(size_t bodies_size) :
    marked_(bodies_size, false), pairs_colliding_{} 
  {}

  void process_pair(const size_t idx_a, const size_t idx_b,
                    const Body& a, const Body& b, const float distance);
  void apply_collisions(std::vector<Body>& bodies);

  void clear();

private:
  std::vector<std::array<size_t, 2>> pairs_colliding_;
  // Bodies marked as colliding
  std::vector<bool> marked_;
};
