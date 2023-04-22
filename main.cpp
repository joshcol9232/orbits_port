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

void spawn_random_planets(std::vector<Body>& bodies,
                          const Vector2f top_left,
                          const Vector2f size,
                          const size_t num,
                          const float rad) {
  for (size_t n = 0; n < num; ++n) {
    Vector2f pos = Vector2f::Random();
    pos.x() = top_left.x() + (pos.x() + 1.0) * size.x()/2.0;
    pos.y() = top_left.y() + (pos.y() + 1.0) * size.y()/2.0;
    bodies.emplace_back(pos, Vector2f::Zero(), rad);
  }
}

int main() {
  srand((unsigned int) time(0));

  std::vector<Body> bodies;
  // bodies.emplace_back(Vector2f(100.0, 200.0),
  //                     Vector2f(100.0, 100.0),
  //                     10.0);
  // bodies.emplace_back(Vector2f(100.0, 600.0),
  //                     Vector2f(100.0, -100.0),
  //                     10.0);
  // bodies.emplace_back(Vector2f(500.0, 400.0),
  //                     Vector2f(-100.0, 0.0),
  //                     10.0);

  // spawn_square_of_planets(bodies, Vector2f(SCREEN_HEIGHT/2.0 + 100.0, SCREEN_HEIGHT/2.0 - 100.0),
  //                         10, 20, 25.0, 5.0);
  // bodies.emplace_back(Vector2f(200.0, 200.0),
  //                     Vector2f(0.0, 0.0),
  //                     50.0);
  // bodies.emplace_back(Vector2f(10000.0, SCREEN_HEIGHT/2.0),
  //                     Vector2f(-2000.0, 0.0),
  //                     20.0);


  spawn_random_planets(bodies, Vector2f(200.0, 200.0), Vector2f(500.0, 500.0), 1000, 5.0);

  // TEST: THREE BODIES INTERSECTING
  // bodies.emplace_back(Vector2f(SCREEN_WIDTH/2.0 - 30.0,
  //                              SCREEN_HEIGHT/2.0),
  //                     Vector2f::Zero(),
  //                     100.0);
  // bodies.emplace_back(Vector2f(SCREEN_WIDTH/2.0 + 30.0,
  //                              SCREEN_HEIGHT/2.0),
  //                     Vector2f::Zero(),
  //                     100.0);
  // bodies.emplace_back(Vector2f(SCREEN_WIDTH/2.0,
  //                              SCREEN_HEIGHT/2.0 - 30.0),
  //                     Vector2f::Zero(),
  //                     100.0);

  // Create assets
  sf::CircleShape body_shape(1.0f, 200);
  body_shape.setFillColor(sf::Color::White);

  sf::Font font;
  font.loadFromFile("./Hack-Bold.ttf");
  sf::Text fps_text;
  fps_text.setFont(font);
  fps_text.setString("0");
  fps_text.setPosition(SCREEN_WIDTH - 60.0, 10.0);
  fps_text.setFillColor(sf::Color::Green);
  fps_text.setCharacterSize(12);


  std::cout << "BODY NUM: " << bodies.size() << std::endl;

  // create the window
  sf::RenderWindow window(sf::VideoMode(static_cast<int>(SCREEN_WIDTH),
                                        static_cast<int>(SCREEN_HEIGHT)),
                          "Orbits");
  // window.setVerticalSyncEnabled(true);

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

    // Draw FPS counter
    fps_text.setString(std::to_string(1.0/dt));
    window.draw(fps_text);

    // end the current frame
    window.display();

    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }

  return 0;
}
