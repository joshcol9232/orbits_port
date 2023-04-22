#include "CollisionResolver.h"

#include <iostream>

#include <Eigen/Dense>
using Eigen::Vector2f;

void CollisionResolver::process_pair(const size_t idx_a, const size_t idx_b,
                                     Body& a, Body& b,
                                     const float distance) const {
  // Check if the two bodies are intersecting
  const float radius_sum = a.get_radius() + b.get_radius();
  if (distance < radius_sum) {
    std::cout << "COLLIDING" << std::endl;
    a.elastic_collide_with(b);
  }
}
