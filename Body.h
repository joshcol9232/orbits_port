#pragma once

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
using Eigen::Vector2f;

class Body {
public:
  Body() {}
  Body(Vector2f pos, Vector2f velocity, float radius);
  Body(Vector2f pos, Vector2f velocity,
       float radius, float mass);

  void draw(sf::RenderWindow& window, sf::CircleShape& circle_mesh) const;

  void step(const float dt);
  void apply_force(const Vector2f& force);
  Vector2f displacement_between(const Body& other) const;
  Vector2f force_with(const Body& other, float& distance) const;
  void elastic_collide_with(Body& other, const float distance);  // NOTE: Consumes other body

  void get_transform(sf::Transform& tr) const;
  float get_radius() const { return radius_; }

private:
  Vector2f x_, v_, force_;
  float mass_, radius_;
};
