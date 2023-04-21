#include "body.h"

#define G 100.1

Body::Body(Vector2f pos, Vector2f velocity,
           float mass, float radius) :
  x_(pos), v_(velocity), mass_(mass), radius_(radius)
{}

void Body::step(const float dt) {
  // F = ma
  v_ += (force_ / mass_) * dt;
  x_ += v_ * dt;

  // Reset resultant force
  force_.x() = 0.0;
  force_.y() = 0.0;
}

void Body::apply_force(const Vector2f& df) { force_ += df; }

Vector2f Body::force_with(const Body& other) const {
  // Get dist vec
  Vector2f force = other.x_ - x_;
  const float distance = force.norm();
  force *= G * mass_ * other.mass_ / (distance*distance*distance);
  return force;
}

sf::Transform Body::get_transform() const {
  sf::Transform tr;
  tr.translate(x_.x(), x_.y());
  tr.scale(radius_, radius_);
  return tr;
}

