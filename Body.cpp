#include "Body.h"

#include <cmath>
#include <Eigen/Dense>

#ifdef DEBUG
#include <iostream>
#endif

using Eigen::Vector2f;

#define G 0.01
#define PLANET_DENSITY 1000.0
#define DAMPING 0.9

// Utility functions
namespace {
  float radius_of_sphere(const double volume) {
    return std::pow((3.0 * volume)/(4.0 * M_PI), 1.0/3.0);
  }

  float volume_of_sphere(const double radius) {
    return (4.0/3.0) * M_PI * radius * radius * radius;
  }
}  // namespace


Body::Body(Vector2f pos, Vector2f velocity, float radius) :
  Body(pos, velocity, radius, volume_of_sphere(radius) * PLANET_DENSITY)
{}

Body::Body(Vector2f pos, Vector2f velocity,
           float radius, float mass) :
  x_(pos), v_(velocity), mass_(mass), radius_(radius), force_(0.0, 0.0)
{}

void Body::step(const float dt) {
  // F = ma
  v_ += force_ * dt/mass_;
  x_ += v_ * dt;

  // Reset resultant force
  force_.x() = 0.0;
  force_.y() = 0.0;
}

void Body::draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const {
  circle_mesh.setScale(1.0, 1.0);
  circle_mesh.setOrigin(1.0, 1.0);

  circle_mesh.setScale(radius_, radius_);
  circle_mesh.setPosition(x_.x(), x_.y());
  circle_mesh.setFillColor(sf::Color::White);
  window.draw(circle_mesh);
}

void Body::apply_force(const Vector2f& df) { force_ += df; }

Vector2f Body::displacement_between(const Body& other) const {
  return other.x_ - x_;
}

// NOTE: Distance is written to here for use in collisions
Vector2f Body::force_with(const Body& other, float& distance) const {
  // Get dist vec
  Vector2f force = displacement_between(other);
  distance = force.norm();
  force *= G * mass_ * other.mass_ / (distance*distance*distance);
  return force;
}

void Body::elastic_collide_with(Body& other, const float distance) {
  // --- Resolve collision ---
  const Vector2f dist_vec = other.x_ - x_;
  const float total_mass = mass_ + other.mass_;

  const float vel_along_collision_normal = (other.v_ - v_).dot(dist_vec);
  const float dist_sqr = distance * distance;

  const Vector2f dv_0 = - (2 * other.mass_ / total_mass) *
                          (vel_along_collision_normal / dist_sqr) * -dist_vec;

  const Vector2f dv_1 = - (2 * mass_ / total_mass) *
                          (vel_along_collision_normal / dist_sqr) * dist_vec;

#ifdef DEBUG
  std::cout << "dv_0: (" << dv_0.x() << ", " << dv_0.y() << ")" << std::endl;
  std::cout << "dv_1: (" << dv_1.x() << ", " << dv_1.y() << ")" << std::endl;
#endif

  v_ += dv_0 * DAMPING;
  other.v_ += dv_1 * DAMPING;

  // ---
  // Move the bodies apart so they are not overlapping (this would cause issues)
  const Vector2f norm = dist_vec / distance;  // Normal to collision
  // Amount of overlap:
  const Vector2f half_dx = 0.5 * (distance - radius_ - other.radius_) * norm;
  // Each needs to be shifted by 1/2 dx r_hat
  x_ += half_dx;
  other.x_ -= half_dx;
}
