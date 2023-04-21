#pragma once

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
using Eigen::Vector2f;

class Body {
public:
  Body() {}
  Body(Vector2f pos, Vector2f velocity,
       float mass, float radius);

  void step(const float dt);
  void apply_force(const Vector2f& force);
  Vector2f force_with(const Body& other) const;

  sf::Transform get_transform() const;

private:
  Vector2f x_, v_, force_;
  float mass_, radius_;
};
