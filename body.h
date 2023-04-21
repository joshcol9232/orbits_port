#ifndef BODY_H
#define BODY_H

#include <Eigen/Dense>
using Eigen::Vector2f;

class Body {
public:
  Body() {}
  Body(Vector2f pos, Vector2f velocity,
       float mass, float radius);

  Body(const Body&) = delete;
  Body& operator=(const Body&) = delete;
  Body(Body&&) = delete;
  Body& operator=(Body&&) = delete;

  void step(const float dt);
  void apply_force(const Vector2f& force);
  const Vector2f& get_position() const { return x_; }
  Vector2f force_with(const Body& other) const;

  Vector2f x_, v_, force_;
  float mass_, radius_;
};

#endif // BODY_H
