#include "Body.h"
#include <cmath>
#include <iostream>

#include <Eigen/Dense>
using Eigen::Vector2f;

#define G 0.01
#define PLANET_DENSITY 1000.0

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
  x_(pos), v_(velocity), mass_(mass), radius_(radius), force_(0.0, 0.0),
  is_already_colliding_(false)
{}

void Body::step(const float dt) {
  // F = ma
  v_ += force_ * dt/mass_;
  x_ += v_ * dt;

  // Reset resultant force
  force_.x() = 0.0;
  force_.y() = 0.0;
  // Reset collision status
  is_already_colliding_ = false;
}

void Body::draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const {
  circle_mesh.setPosition(x_.x(), x_.y());
  circle_mesh.setScale(radius_, radius_);
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

void Body::elastic_collide_with(Body& other) {
  if (is_already_colliding_ || other.is_already_colliding_) return;

  is_already_colliding_ = true;
  other.is_already_colliding_ = true;
  
  const float total_mass = mass_ + other.mass_;
  const Vector2f dist_vec = other.x_ - x_;
  const float vel_along_collision_normal = (other.v_ - v_).dot(dist_vec);
  const float dist_sqr = dist_vec.squaredNorm();

  const Vector2f dv_0 = - (2 * other.mass_ / total_mass) *
                          (vel_along_collision_normal / dist_sqr) * -dist_vec;

  const Vector2f dv_1 = - (2 * mass_ / total_mass) *
                          (vel_along_collision_normal / dist_sqr) * dist_vec;

  std::cout << "dv_0: (" << dv_0.x() << ", " << dv_0.y() << ")" << std::endl;
  std::cout << "dv_1: (" << dv_1.x() << ", " << dv_1.y() << ")" << std::endl;

  v_ += dv_0;
  other.v_ += dv_1;
}
