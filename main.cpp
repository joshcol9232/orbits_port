#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "body.h"

using Eigen::Vector2f;



int main() {
  std::vector<Body> bodies;
  bodies.emplace_back(Vector2f(100.0, 100.0),
                      Vector2f(10.0, 0.0),
                      1000.0,
                      10.0);
  bodies.emplace_back(Vector2f(100.0, 200.0),
                      Vector2f(10.0, 0.0),
                      1000.0,
                      10.0);

  // Create assets
  sf::CircleShape body_shape(1.0f);
  body_shape.setPointCount(200);

  // create the window
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Orbits");

  sf::Clock delta_clock;
  float dt;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      
    }

    // Update gravity
    Vector2f grav;
    for (uint32_t i = 0; i < bodies.size()-1; ++i) {
      for (uint32_t j = i+1; j < bodies.size(); ++j) {
        grav = bodies[i].force_with(bodies[j]);
        bodies[i].apply_force(grav);
        // Equal and opposite force
        bodies[j].apply_force(-grav);
      }
    }

    // Euler step
    for (auto& body : bodies) {
      body.step(dt);
    }

    // Draw
    window.clear(sf::Color::Black);

    for (const auto& body : bodies) {
      window.draw(body_shape, body.get_transform());
    }

    // end the current frame
    window.display();

    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }

  return 0;
}
