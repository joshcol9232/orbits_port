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
namespace {

void spawn_square_of_planets(
  std::vector<Body>& bodies,
  Vector2f top_left,
  Vector2f v,
  const size_t w,
  const size_t h,
  const float rad
) {
  bool charge = false;
  for (size_t i = 0; i < w; ++i) {
    for (size_t j = 0; j < h; ++j) {
      bodies.emplace_back(Vector2f(top_left.x() + static_cast<float>(i) * rad * 2.0 + 0.01,
                                   top_left.y() + static_cast<float>(j) * rad * 2.0 + 0.01),
                          v,
                          rad,
                          charge);
      charge = !charge;
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
    const Vector2f cc = Vector2f::Random();
    std::cout << cc.x() << std::endl;
    pos.x() = top_left.x() + (pos.x() + 1.0) * size.x()/2.0;
    pos.y() = top_left.y() + (pos.y() + 1.0) * size.y()/2.0;
    bodies.emplace_back(pos, Vector2f::Zero(), rad, std::signbit(cc.x()));
  }
}

// Reset bodies to start state
void start_state(std::vector<Body>& bodies) {
  bodies.clear();

  spawn_square_of_planets(bodies, Vector2f(600.0, 800.0),
                          Vector2f(0.0, 0.0), 11, 11, 10.0);

  // spawn_random_planets(bodies, Vector2f(0.0, 0.0),
  //                      Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT), 10, 10.0);

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

  // bodies.emplace_back(Vector2f(0.0, 0.0),
  //                     Vector2f(0.0, 0.0),
  //                     10.0,
  //                     false);
  // bodies.emplace_back(Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT),
  //                     Vector2f(0.0, 0.0),
  //                     10.0,
  //                     true);
}

void move_camera(auto& window, auto& main_camera, const float dx, const float dy, const float dt) {
  main_camera.move(dx * dt, dy * dt);
  window.setView(main_camera);
}

}  // namespace

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

  // Camera
  bool cam_move_up, cam_move_down, cam_move_left, cam_move_right;

  std::cout << "BODY NUM: " << bodies.size() << std::endl;

  // create the window
  sf::RenderWindow window(sf::VideoMode(static_cast<int>(SCREEN_WIDTH),
                                        static_cast<int>(SCREEN_HEIGHT)),
                          "Orbits");
  sf::View main_camera(sf::FloatRect(0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT));

  sf::Clock delta_clock;
  float dt = 1.0/60.0;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // --- Mouse ---
      if (event.type == sf::Event::MouseButtonPressed) {
        dragging = true;
        mouse_start_pos = sf::Mouse::getPosition(window);
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        dragging = false;

        // Spawn planet with velocity
        constexpr float mouse_rad = 10.0;
        const auto drag = mouse_start_pos - curr_mouse_press_pos;
        bodies.emplace_back(Vector2f(mouse_start_pos.x, mouse_start_pos.y),
                            Vector2f(drag.x, drag.y) * 5.0,
                            mouse_rad, tools::volume_of_sphere(mouse_rad) * PLANET_DENSITY * 5.0,
                            event.mouseButton.button == sf::Mouse::Left);
      }
      // -------------
      // --- Keyboard ---
      if (event.type == sf::Event::KeyPressed) {
        if(event.key.code == sf::Keyboard::R) {
          start_state(bodies);
        } else if (event.key.code == sf::Keyboard::C) {
          bodies.clear();
        }
        // --- CAMERA ---
        cam_move_up    = event.key.code == sf::Keyboard::W;
        cam_move_left  = event.key.code == sf::Keyboard::A;
        cam_move_down  = event.key.code == sf::Keyboard::S;
        cam_move_right = event.key.code == sf::Keyboard::D;
        // --------------
      } else if (event.type == sf::Event::KeyReleased) {
        // --- CAMERA ---
        if (event.key.code == sf::Keyboard::W) {
          cam_move_up = false;
        } else if (event.key.code == sf::Keyboard::A) {
          cam_move_left = false;
        } else if (event.key.code == sf::Keyboard::S) {
          cam_move_down = false;
        } else if (event.key.code == sf::Keyboard::D) {
          cam_move_right = false;
        }
        // --------------
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

    // Camera
    // if (cam_move_up)    move_camera(window, main_camera,    0.0, -600.0, dt);
    // if (cam_move_down)  move_camera(window, main_camera,    0.0,  600.0, dt);
    // if (cam_move_left)  move_camera(window, main_camera, -600.0,    0.0, dt);
    // if (cam_move_right) move_camera(window, main_camera,  600.0,    0.0, dt);

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
        if (dist < a.get_radius() + b.get_radius()) { a.elastic_collide_with(b, dist, dt); }
      }
    }

    // Euler step
    for (auto& body : bodies) {
      body.step(dt);
    }

    // Overlap passes
    for (size_t o = 0; o < 8; ++o) {
      for (size_t i = 0; i < bodies.size()-1; ++i) {
        Body& a = bodies[i];
        for (size_t j = i+1; j < bodies.size(); ++j) {
          Body& b = bodies[j];

          auto dist_vec = a.displacement_to(b);
          auto dist = dist_vec.norm();
          if (dist < a.get_radius() + b.get_radius())
            a.correct_overlap_with(b, dist);
        }
      }
    }

    // Draw
    window.clear(sf::Color::Black);

    for (const auto& body : bodies) {
      body.draw(window, body_shape);
    }

    // Draw mouse drag
    if (dragging) window.draw(drag_line, 2, sf::Lines);

    // --- RENDER STATIC ITEMS LIKE FPS ---
    window.setView(window.getDefaultView());
    // Draw FPS counter
    fps_text.setString(std::to_string(1.0/dt));
    window.draw(fps_text);
    // ------------------------------------
    window.setView(main_camera);

    // end the current frame
    window.display();

    sf::Time dt_time = delta_clock.restart();
    dt = dt_time.asSeconds();
  }

  return 0;
}
