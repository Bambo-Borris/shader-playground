#pragma once

#include <SFML/Graphics.hpp>

class App {
public:
    App();
    ~App();

    void run();

private:
    void logFPS(const sf::Time& dt);

    sf::RenderWindow m_window;
};
