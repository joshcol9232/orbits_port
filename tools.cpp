#include "tools.h"
#include <cmath>

#include "common.h"

namespace tools {
  float radius_of_sphere(const double volume) {
    return std::pow((3.0 * volume)/(4.0 * M_PI), 1.0/3.0);
  }

  float volume_of_sphere(const double radius) {
    return (4.0/3.0) * M_PI * std::pow(radius, 3);
  }

  // Returns the magnitude of the velocity (speed) needed for a circular orbit around another planet
  // Orbit is circular when the kinetic energy does not change.
  // K = GMm/2r  -- Derived from centripetal force (in circular motion) = gravitational force
  // GMm/2r = 1/2 mv^2
  // GM/2r = 1/2 v^2
  // sqrt(GM/r) = v
  float circular_orbit_speed(const float host_mass, const float radius) {
    return std::sqrt(G * host_mass/radius);
  }

  Vector2f get_components(const float magnitude, const float angle) {
    return magnitude * Vector2f(std::cos(angle), std::sin(angle));
  }

}  // namespace tools