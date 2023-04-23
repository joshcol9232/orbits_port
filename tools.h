#pragma once

#include <Eigen/Dense>
using Eigen::Vector2f;

// Utility functions
namespace tools {
  float radius_of_sphere(const double volume);

  float volume_of_sphere(const double radius);

  float circular_orbit_speed(const float host_mass, const float radius);

  Vector2f get_components(const float magnitude, const float angle);
}  // namespace tools