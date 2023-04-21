#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "Body.h"
#include "CollisionResolver.h"

using Eigen::Vector2f;



int main() {
  std::vector<Body> bodies;
  bodies.emplace_back(Vector2f(100.0, 100.0),
                      Vector2f(10.0, 0.0),
                      10.0);
  bodies.emplace_back(Vector2f(100.1, 200.0),
                      Vector2f(10.0, 0.0),
                      10.0);

  // Create assets
  sf::CircleShape body_shape(1.0f, 200);
  body_shape.setFillColor(sf::Color::White);

  // create the window
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Orbits");
  window.setVerticalSyncEnabled(true);

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

  CollisionResolver collision_res(bodies.size());

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

    }

    // Update gravity & collisions context
    Vector2f grav;
    float dist;
    for (size_t i = 0; i < bodies.size()-1; ++i) {
      for (size_t j = i+1; j < bodies.size(); ++j) {
        grav = bodies[i].force_with(bodies[j], dist);
        bodies[i].apply_force(grav);
        // Equal and opposite force
        bodies[j].apply_force(-grav);

        collision_res.process_pair(i, j, bodies[i], bodies[j], dist);
      }
    }

    // Apply collisions
    collision_res.apply_collisions(bodies);

    // Euler step
    for (auto& body : bodies) {
      body.step(dt);
    }

    // Draw
    window.clear(sf::Color::Black);

    for (const auto& body : bodies) {
      body.draw(window, body_shape);
    }

    // end the current frame
    window.display();

    collision_res.clear();
    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }

  return 0;
}
