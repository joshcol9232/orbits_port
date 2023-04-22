#include "Body.h"

#include <cmath>
#include <Eigen/Dense>

#include "common.h"

#ifdef DEBUG
#include <iostream>
#endif

#define WALL_BOUNCE

using Eigen::Vector2f;

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

  // Wall bouncing
#ifdef WALL_BOUNCE
  if (x_.x() < radius_) {
    v_.x() *= -1;
    x_.x() = radius_;
  }
  if (x_.x() > SCREEN_WIDTH - radius_) {
    v_.x() *= -1;
    x_.x() = SCREEN_WIDTH - radius_;
  }

  if (x_.y() < radius_) {
    v_.y() *= -1;
    x_.y() = radius_;
  }
  if (x_.y() > SCREEN_HEIGHT - radius_) {
    v_.y() *= -1;
    x_.y() = SCREEN_HEIGHT - radius_;
  }
#endif

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

// NOTE: Distance is written to here for use in collisions
Vector2f Body::force_with(const Body& other, float& distance) const {
  // Get dist vec
  Vector2f force = other.x_ - x_;
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

  v_ += dv_0 * COLLISION_DAMPING;
  other.v_ += dv_1 * COLLISION_DAMPING;

  // ---
  // Move the bodies apart so they are not overlapping (this would cause issues)
  const Vector2f norm = dist_vec / distance;  // Normal to collision
  // Amount of overlap:
  const Vector2f half_dx = 0.5 * (distance - radius_ - other.radius_) * norm;
  // Each needs to be shifted by 1/2 dx r_hat
  x_ += half_dx;
  other.x_ -= half_dx;
}
