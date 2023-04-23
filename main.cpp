#include <iostream>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

#include "common.h"
#include "tools.h"
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

void spawn_planet_with_moons(
  std::vector<Body>& bodies,
  const Vector2f position,
  const Vector2f frame_velocity,
  const float main_planet_radius,
  const size_t moon_num,
  const float moon_orbit_radius_range[2],    // Starting from surface of planet
  const float moon_body_radius_range[2],
  const bool orbit_direction_clockwise  // anticlockwise = false, clockwise = true
) {
  bodies.emplace_back(position, frame_velocity, main_planet_radius);
  const float main_planet_mass = bodies[bodies.size()-1].get_mass();

  // let mut rng = rand::thread_rng();

  //   let orbit_rad_range = Uniform::from(moon_orbit_radius_range.0..moon_orbit_radius_range.1);
  //   let angle_range = Uniform::from(0.0..TWO_PI);
  //   let size_rad_range = Uniform::from(moon_body_radius_range.0..moon_body_radius_range.1);

  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0.0, 1.0);

  for (size_t n = 0; n < moon_num; ++n) {
    const float orbit_radius = main_planet_radius + moon_orbit_radius_range[0] + dist(e2) * (moon_orbit_radius_range[1] - moon_orbit_radius_range[0]);
    const float orbit_speed = tools::circular_orbit_speed(main_planet_mass, orbit_radius);
    const float start_angle = dist(e2) * 2.0 * M_PI;      // Angle from main planet to moon
    const Vector2f start_pos = tools::get_components(orbit_radius, start_angle);   // Position on circle orbit where planet will start

    const Vector2f start_velocity = tools::get_components(
      orbit_speed,
      orbit_direction_clockwise ? start_angle + M_PI/2.0 : start_angle - M_PI/2.0
    );

    const float moon_radius = moon_body_radius_range[0] + dist(e2) * (moon_body_radius_range[1] - moon_body_radius_range[0]);

    bodies.emplace_back(position + start_pos,
                        start_velocity + frame_velocity,  // Add velocity of main planet
                        moon_radius);
  }
}

// Reset bodies to start state
void start_state(std::vector<Body>& bodies) {
  bodies.clear();

  // spawn_square_of_planets(bodies, Vector2f(200.0, 100.0),
  //                         25, 25, 20.0, 2.0);

  // spawn_random_planets(bodies, Vector2f(400.0, 400.0),
  //                      Vector2f(100.0, 100.0), 400, 1.0);

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

  const float orbit_range[2] = {30.0, 150.0};
  const float rad_range[2] = {1.0, 1.5};
  spawn_planet_with_moons(bodies, Vector2f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
                          Vector2f::Zero(), 50.0, 1000, orbit_range,
                          rad_range, true);
}


int main() {
  srand((unsigned int) time(0));

  std::vector<Body> bodies;
  start_state(bodies);

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

  // Mouse
  bool dragging = false;
  sf::Vector2i mouse_start_pos, curr_mouse_press_pos;
  sf::Vertex drag_line[2];

  std::cout << "BODY NUM: " << bodies.size() << std::endl;

  // create the window
  sf::RenderWindow window(sf::VideoMode(static_cast<int>(SCREEN_WIDTH),
                                        static_cast<int>(SCREEN_HEIGHT)),
                          "Orbits");

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // --- Mouse ---
      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        dragging = true;
        mouse_start_pos = sf::Mouse::getPosition(window);
      }
      if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        dragging = false;

        // Spawn planet with velocity
        const auto drag = mouse_start_pos - curr_mouse_press_pos;
        bodies.emplace_back(Vector2f(mouse_start_pos.x, mouse_start_pos.y),
                            Vector2f(drag.x, drag.y),
                            10.0);
      }
      // -------------
      // --- Keyboard ---
      if (event.type == sf::Event::KeyPressed) {
        if(event.key.code == sf::Keyboard::R) {
          start_state(bodies);
        } else if (event.key.code == sf::Keyboard::C) {
          bodies.clear();
        }
      }

      // ----------------
    }

    // Mouse drag
    if (dragging) {
      curr_mouse_press_pos = sf::Mouse::getPosition(window);
      drag_line[0] = sf::Vertex(sf::Vector2f(static_cast<float>(mouse_start_pos.x), static_cast<float>(mouse_start_pos.y)));
      drag_line[1] = sf::Vertex(sf::Vector2f(static_cast<float>(curr_mouse_press_pos.x), static_cast<float>(curr_mouse_press_pos.y)));
      drag_line[0].color = sf::Color::Green;
      drag_line[1].color = sf::Color::Green;
    }

    // Update physics
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
        if (dist < radius_sum) {
          a.elastic_collide_with(b, dist);
        }
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

    // Draw mouse drag
    if (dragging) {
      window.draw(drag_line, 2, sf::Lines);
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
