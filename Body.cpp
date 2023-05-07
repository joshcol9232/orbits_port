#include "Body.h"

#include <cmath>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"

#include <iostream>


// #define WALL_BOUNCE

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

namespace {

// 2D "cross product" ie. determenant of packed matrix
inline float cross2d(const Vector2f& a, const Vector2f& b) {
  return a.x() * b.y() - b.x() * a.y();
}

}

void Body::elastic_collide_with(Body& other, const float distance, const float dt) {
  correct_overlap_with(other, distance);

  // --- Resolve collision ---
  const Vector2f dist_vec = other.x_ - x_;
  const Vector2f v_diff = other.v_ - v_;
  const float total_mass = mass_ + other.mass_;

  // Work out tangent component for friction later
  Vector2f tangent_normal = dist_vec / distance;
  { // rotate 90 degrees CW
    const float x_tmp = tangent_normal.x();
    tangent_normal.x() = tangent_normal.y();
    tangent_normal.y() = -x_tmp;
  }

  float vel_mul_dist_along_collision_normal = v_diff.dot(dist_vec);
  const float dist_sqr = std::pow(radius_ + other.radius_, 2); // distance * distance;

  // save change in velocity along collision normal for friction
  const float dv_0_along_normal = (2 * other.mass_ / total_mass) *
                    (vel_mul_dist_along_collision_normal / dist_sqr);

  const Vector2f dv_0 = dv_0_along_normal * dist_vec;

  const float dv_1_along_normal = - (2 * mass_ / total_mass) *
                    (vel_mul_dist_along_collision_normal / dist_sqr);
  const Vector2f dv_1 = dv_1_along_normal * dist_vec;

#ifdef DEBUG
  std::cout << "dv_0: (" << dv_0.x() << ", " << dv_0.y() << ")" << std::endl;
  std::cout << "dv_1: (" << dv_1.x() << ", " << dv_1.y() << ")" << std::endl;
#endif


  // TODO: Need a function that rotates vector depending on direction of travel.
  //       So friction is applied in opposite direction

  std::cout << "tangent_normal: " << tangent_normal << std::endl;
  // Friction is the normal force * coefficient.
  // F_f = mu F_n
  // Find force from impulse in collision. F = dp/dt
  // dp = dv * m
  // dt from frame time
  // Also force will be equal & opposite
  // j = f dt
  const float impulse_along_normal_0 = dv_0_along_normal * mass_;
  const float impulse_along_normal_1 = dv_1_along_normal * other.mass_;
  // std::cout << "Force along normal: " << force_along_normal << std::endl;
  // // Friction applied to opposite direction to velocity diff.
  Vector2f friction_impulse_0 = tangent_normal * impulse_along_normal_0 * FRICTION;
  Vector2f friction_impulse_1 = tangent_normal * impulse_along_normal_1 * FRICTION;

  // If dv along vector is -ve, * -
  const float current_diff_v_along_tangent = v_diff.dot(tangent_normal);
  if (current_diff_v_along_tangent > 0) {
    friction_impulse_0 *= -1.0;
    friction_impulse_1 *= -1.0;
  }

  v_ += friction_impulse_0 / mass_;
  other.v_ += friction_impulse_1 / other.mass_;

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
