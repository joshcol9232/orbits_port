#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "Body.h"

using Eigen::Vector2f;

// Utils
void spawn_square_of_planets(
  std::vector<Body>& bodies,
  Vector2f top_left,
  const size_t w,
  const size_t h,
  const float gap,
  const float rad
) {
  for (size_t i = 0; i < w; ++i) {
    for (size_t j = 0; j < h; ++j) {
      bodies.emplace_back(Vector2f(top_left.x() + static_cast<float>(i) * gap,
                                   top_left.y() + static_cast<float>(j) * gap),
                          Vector2f(0.0, 0.0),
                          rad);
    }
  }
}

void spawn_random_planets(std::vector<Body>& bodies, const size_t num, const float rad) {
  for (size_t n = 0; n < num; ++n) {
    Vector2f pos = Vector2f::Random();
    pos.x() = (pos.x() + 1.0) * static_cast<float>(SCREEN_WIDTH)/2.0;
    pos.y() = (pos.y() + 1.0) * static_cast<float>(SCREEN_HEIGHT)/2.0;
    bodies.emplace_back(pos, Vector2f::Zero(), rad);
  }
}

int main() {
  std::vector<Body> bodies;
  // bodies.emplace_back(Vector2f(100.0, 200.0),
  //                     Vector2f(100.0, 100.0),
  //                     10.0);
  // bodies.emplace_back(Vector2f(100.1, 600.0),
  //                     Vector2f(100.0, -100.0),
  //                     10.0);
  // bodies.emplace_back(Vector2f(500.0, 400.0),
  //                     Vector2f(-100.0, 0.0),
  //                     10.0);

  // spawn_square_of_planets(bodies, Vector2f(100.0, 100.0),
  //                         3, 3, 50.0, 10.0);

  spawn_random_planets(bodies, 10, 50.0);

  // THREE BODIES INTERSECTING
  // bodies.emplace_back(Vector2f(static_cast<float>(SCREEN_WIDTH)/2.0 - 30.0,
  //                              static_cast<float>(SCREEN_HEIGHT)/2.0),
  //                     Vector2f::Zero(),
  //                     100.0);
  // bodies.emplace_back(Vector2f(static_cast<float>(SCREEN_WIDTH)/2.0 + 30.0,
  //                              static_cast<float>(SCREEN_HEIGHT)/2.0),
  //                     Vector2f::Zero(),
  //                     100.0);
  // bodies.emplace_back(Vector2f(static_cast<float>(SCREEN_WIDTH)/2.0,
  //                              static_cast<float>(SCREEN_HEIGHT)/2.0 - 30.0),
  //                     Vector2f::Zero(),
  //                     100.0);

  // Create assets
  sf::CircleShape body_shape(1.0f, 200);
  body_shape.setFillColor(sf::Color::White);

  // create the window
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Orbits");
  window.setVerticalSyncEnabled(true);

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

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
      Body& a = bodies[i];
      for (size_t j = i+1; j < bodies.size(); ++j) {
        Body& b = bodies[j];

        grav = a.force_with(b, dist);
        a.apply_force(grav);
        // Equal and opposite force
        b.apply_force(-grav);

        // Process collisions
        const float radius_sum = a.get_radius() + b.get_radius();
        if (dist < radius_sum) { a.elastic_collide_with(b, dist); }
      }
    }

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

    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }

  return 0;
}
