#include "CollisionResolver.h"

#include <iostream>
#include <algorithm>

#include <Eigen/Dense>
using Eigen::Vector2f;

void CollisionResolver::process_pair(const size_t idx_a, const size_t idx_b,
                                     const Body& a, const Body& b,
                                     const float distance) {
  // Check if the two bodies are intersecting
  const float radius_sum = a.get_radius() + b.get_radius();
  if (!marked_[idx_a] && !marked_[idx_b] && distance < radius_sum) {
    std::cout << "COLLIDING" << std::endl;
    pairs_colliding_.push_back({idx_a, idx_b});
    marked_[idx_a] = true;
    marked_[idx_b] = true;
  }
}

std::vector<Body> CollisionResolver::apply_collisions(std::vector<Body> bodies) {
  // For each pair, resolve collision
  // NOTE: Almost impossible for three bodies to be colliding in a frame
  // for inelasic collisions. May need to worry about this if changed to
  // elastic.

  for (const auto& pair : pairs_colliding_) {
    std::cout << "Doing collision between pair: " << pair[0] << ", " << pair[1] << std::endl;
    bodies[pair[0]].inelastic_collide_with(bodies[pair[1]]);
  }

  if (pairs_colliding_.size() > 0) {
    // For each colliding pair there will be one body eliminated in the collision
    std::vector<Body> new_bodies(bodies.size() - pairs_colliding_.size());
    size_t idx = 0;
    size_t b4_idx = 0;
    for (const bool& mark : marked_) {
      if (!mark) {
        new_bodies[idx] = bodies[b4_idx];
        ++idx;
      }
      ++b4_idx;
    }
    
    std::cout << "Bodies size: " << new_bodies.size() << std::endl;
    return new_bodies;
  }

  return bodies;
}

void CollisionResolver::clear() {
  pairs_colliding_.clear();
  for (size_t idx = 0; idx < marked_.size(); ++idx) {
    marked_[idx] = false;
  }
}
