#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// Taken from SFML github wiki

class SfLine : public sf::Drawable {
public:
    SfLine(const sf::Vector2f& point1, const sf::Vector2f& point2,
           const sf::Color color = sf::Color::Red,
           const float thickness = 2.0) :
        vertices()
    {
        sf::Vector2f direction = point2 - point1;
        sf::Vector2f unitDirection = direction/std::sqrt(direction.x*direction.x+direction.y*direction.y);
        sf::Vector2f unitPerpendicular(-unitDirection.y,unitDirection.x);

        sf::Vector2f offset = (thickness/2.f)*unitPerpendicular;

        vertices.push_back(sf::Vertex(point1 + offset));
        vertices.push_back(sf::Vertex(point2 + offset));
        vertices.push_back(sf::Vertex(point2 - offset));
        vertices.push_back(sf::Vertex(point1 - offset));

        for (int i=0; i<4; ++i)
            vertices[i].color = color;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(vertices.data(), vertices.size(), sf::Quads);
    }

    SfLine& operator+=(const SfLine& other) {
        vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
        return *this;
    }

private:
    std::vector<sf::Vertex> vertices;
    float thickness;
    sf::Color color;
};