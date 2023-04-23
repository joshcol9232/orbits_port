#include "Body.h"

#include <cmath>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"

#ifdef DEBUG
#include <iostream>
#endif

#define WALL_BOUNCE

using Eigen::Vector2f;

Body::Body(Vector2f pos, Vector2f velocity, float radius) :
  Body(pos, velocity, radius, tools::volume_of_sphere(radius) * PLANET_DENSITY)
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

Vector2f Body::displacement_to(const Body& other) const {
  return other.x_ - x_;
}

// NOTE: Distance is written to here for use in collisions
Vector2f Body::force_with(const Body& other, float& distance) const {
  // Get dist vec
  Vector2f force = other.x_ - x_;
  distance = force.norm();
  force *= G * mass_ * other.mass_ / (distance*distance*distance);
  return force;
}

void Body::elastic_collide_with(Body& other, const float distance) {
  correct_overlap_with(other, distance);

  // --- Resolve collision ---
  const Vector2f dist_vec = other.x_ - x_;
  const float total_mass = mass_ + other.mass_;

  const float vel_along_collision_normal = (other.v_ - v_).dot(dist_vec);
  const float dist_sqr = std::pow(radius_ + other.radius_, 2); // distance * distance;

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
}

void Body::correct_overlap_with(Body& other, const float distance) {
  // Move the bodies apart so they are not overlapping (this would cause issues)
  // NOTE: TODO - Maybe this is causing the spinning - not conserving angular momentum.
  //       Should instead shift the planet's along their trajectory?
  //        

  const Vector2f norm = (other.x_ - x_) / distance;  // Normal to collision
  const float overlap = distance - radius_ - other.radius_;
  // Amount of overlap:
  const Vector2f half_dx = 0.5 * overlap * norm;
  // Each needs to be shifted by 1/2 dx r_hat
  x_ += half_dx;
  other.x_ -= half_dx;
}
