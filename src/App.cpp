#include "App.hpp"

#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <string>

constexpr auto WINDOW_TITLE { "Shader Playground [indev]" };

App::App()
{
    sf::ContextSettings ctxt;
    ctxt.antialiasingLevel = 16;
    m_window.create(sf::VideoMode({ 1024, 720 }), WINDOW_TITLE, sf::Style::Default, ctxt);
    spdlog::set_level(spdlog::level::debug);
    m_window.setFramerateLimit(60);
    m_window.setKeyRepeatEnabled(false);

    sf::Image icon;
    if (!icon.loadFromFile("bin/appicon.png"))
        throw std::runtime_error("Unable to load application icon");

    m_window.setIcon(icon.getSize(), icon.getPixelsPtr());

    if (!ImGui::SFML::Init(m_window))
        throw std::runtime_error("Unable to initialise ImGui SFML");
}

App::~App() { ImGui::SFML::Shutdown(m_window); }

void App::run()
{
    sf::Clock loopClock;

    while (m_window.isOpen()) {
        auto deltaTime = loopClock.restart();
        if (deltaTime > sf::seconds(0.25f)) {
            deltaTime = sf::seconds(0.25f);
        }
        
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window.close();

            ImGui::SFML::ProcessEvent(m_window, event);
        }

        logFPS(deltaTime);

        ImGui::SFML::Update(m_window, deltaTime);

        ImGui::Begin("Test Window"); 
        ImGui::End();

        m_window.clear();
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

void App::logFPS(const sf::Time& dt)
{
    static int counter = 0;
    static auto sum = sf::Time::Zero;

    if (counter < 10) {
        sum += dt;
        ++counter;
    } else {
        auto fps = 1.0f / (sum.asSeconds() / static_cast<float>(counter));
        const auto newTitle = fmt::format("{} - FPS {}", WINDOW_TITLE, static_cast<sf::Uint32>(fps));
        m_window.setTitle(newTitle);
        sum = sf::Time::Zero;
        counter = 0;
    }
}