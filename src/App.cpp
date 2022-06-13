#include "App.hpp"

#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <sstream>
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

    if (!m_renderTexture.create({ 600, 600 }))
        throw std::runtime_error("Unable to create RenderTexture");

    if (!sf::Shader::isAvailable())
        throw std::runtime_error("Shaders are not available");

    m_shaderSource.resize(5000);
}

App::~App() { ImGui::SFML::Shutdown(m_window); }

void App::run()
{
    sf::Clock loopClock;
    auto didFailLastCompile { false };
    std::string errorString;

    while (m_window.isOpen()) {
        auto deltaTime = loopClock.restart();
        if (deltaTime > sf::seconds(0.25f)) {
            deltaTime = sf::seconds(0.25f);
        }

        sf::Event event;
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_window, event);
            if (event.type == sf::Event::Closed)
                m_window.close();

            if (event.type == sf::Event::Resized) {
                const sf::View v { sf::Vector2f { static_cast<float>(event.size.width) / 2.0f,
                                                  static_cast<float>(event.size.height) / 2.0f },
                                   sf::Vector2f { static_cast<float>(event.size.width),
                                                  static_cast<float>(event.size.height) } };
                m_window.setView(v);
            };
        }

        logFPS(deltaTime);

        ImGui::SFML::Update(m_window, deltaTime);

        ImGui::Begin("Test Window");

        if (ImGui::InputTextMultiline("Fragment Shader Source", m_shaderSource.data(), m_shaderSource.size())) {
            auto defaultStr = sf::err().rdbuf();
            std::stringstream errStream;
            sf::err().rdbuf(errStream.rdbuf());
            if (!m_shader.loadFromMemory(m_shaderSource, sf::Shader::Type::Fragment)) {
                errorString = errStream.str();
                didFailLastCompile = true;
            } else
                didFailLastCompile = false;

            sf::err().rdbuf(defaultStr);
        }

        if (didFailLastCompile) {
            ImGui::TextColored(ImVec4(sf::Color::Red), "Shader compile error!");
            ImGui::Text("%s", errorString.data());
        }
        ImGui::End();

        m_renderTexture.clear();
        sf::RectangleShape shape(sf::Vector2f { m_renderTexture.getSize() });
        sf::RenderStates states;
        states.shader = &m_shader;
        m_renderTexture.draw(shape, states);
        m_renderTexture.display();

        m_window.clear(sf::Color(75, 75, 75));
        sf::Sprite spr(m_renderTexture.getTexture());
        spr.setPosition((sf::Vector2f(m_window.getSize()) * 0.5f) - (spr.getGlobalBounds().getSize() * 0.5f));
        m_window.draw(spr);
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