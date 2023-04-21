#include "CollisionResolver.h"

#include <iostream>

#include <Eigen/Dense>
using Eigen::Vector2f;

void CollisionResolver::process_pair(const size_t idx_a, const size_t idx_b,
                                     const Body& a, const Body& b,
                                     const float distance) {
  // Check if the two bodies are intersecting
  const float radius_sum = a.get_radius() + b.get_radius();
  if (distance < radius_sum) {
    std::cout << "COLLIDING" << std::endl;
    pairs_colliding_.push_back({idx_a, idx_b});
    // marked_[idx_a] = true;
    // marked_[idx_b] = true;
  }
}

void CollisionResolver::apply_collisions(std::vector<Body>& bodies) {
  // For each pair, resolve collision
  // For each colliding pair there will be one body eliminated in the collision
  std::vector<Body> new_bodies;
  // NOTE: Almost impossible for three bodies to be colliding in a frame
  // for inelasic collisions. May need to worry about this if changed to
  // elastic.

  for (const auto& pair : pairs_colliding_) {
    std::cout << "Doing collision between pair: " << pair[0] << ", " << pair[1] << std::endl;
    bodies[pair[0]].inelastic_collide_with(bodies[pair[1]]);
    new_bodies.push_back(bodies[pair[0]]);
  }

  if (pairs_colliding_.size() > 0) {
    bodies.resize(new_bodies.size());
    for (size_t i = 0; i < new_bodies.size(); ++i) {
      bodies[i] = new_bodies[i];
    }

    std::cout << "Bodies size: " << bodies.size() << std::endl;
  }
}

void CollisionResolver::clear() {
  pairs_colliding_.clear();
  for (size_t idx = 0; idx < marked_.size(); ++idx) {
    marked_[idx] = false;
  }
}
