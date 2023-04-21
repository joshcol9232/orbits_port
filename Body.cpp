#include "Body.h"
#include <cmath>
#include <iostream>

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
  circle_mesh.setPosition(x_.x(), x_.y());
  circle_mesh.setScale(radius_, radius_);
  circle_mesh.setFillColor(sf::Color::White);
  window.draw(circle_mesh);
}

void Body::apply_force(const Vector2f& df) { force_ += df; }

Vector2f Body::displacement_between(const Body& other) const {
  return other.x_ - x_;
}

// NOTE: Distance written to here for use in collisions
Vector2f Body::force_with(const Body& other, float& distance) const {
  // Get dist vec
  Vector2f force = displacement_between(other);
  distance = force.norm();
  force *= G * mass_ * other.mass_ / (distance*distance*distance);
  return force;
}

void Body::inelastic_collide_with(const Body& other) {
  // Conservation of momentum
  const float total_mass = mass_ + other.mass_;
  const Vector2f total_momentum = mass_ * v_ + other.mass_ * other.v_;
  radius_ = radius_of_sphere(total_mass/PLANET_DENSITY);
  // Use centre of mass as new position
  x_.x() = (x_.x() * mass_ + other.x_.x() * other.mass_)/total_mass;
  x_.y() = (x_.y() * mass_ + other.x_.y() * other.mass_)/total_mass;
  std::cout << "New position: " << x_.x() << ", " << x_.y() << std::endl;

  v_ = total_momentum/total_mass;   // Inelastic collision
  mass_ = total_mass;
}
