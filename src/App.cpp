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

    m_shaderSource.resize(1000000);
}

App::~App() { ImGui::SFML::Shutdown(m_window); }

void App::run()
{
    sf::Clock loopClock;
    sf::Clock elapsedClock;
    while (m_window.isOpen()) {
        auto dt = loopClock.restart();
        if (dt > sf::seconds(0.25f)) {
            dt = sf::seconds(0.25f);
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

        logFPS(dt);
        updateUI(dt);
        setupShaderUniforms(dt, elapsedClock.getElapsedTime());

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
        ++m_frames;
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

void App::updateUI(const sf::Time& dt)
{
    constexpr auto SIDE_PANEL_WINDOW_WIDTH_PERCENT { 0.2f };
    constexpr auto BOTTOM_PANEL_WINDOW_WIDTH_PERCENT { 1.f - (1.f * SIDE_PANEL_WINDOW_WIDTH_PERCENT) };
    constexpr auto BOTTOM_PANEL_WINDOW_HEIGHT_PERCENT { 0.15f };

    ImGui::SFML::Update(m_window, dt);
    const auto renderWindowSize = sf::Vector2f { m_window.getSize() };
    const auto sidePanelSize
        = sf::Vector2f { renderWindowSize.x * SIDE_PANEL_WINDOW_WIDTH_PERCENT, renderWindowSize.y };

    /*
    Options Window
    */
    ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(sidePanelSize);
    ImGui::SetWindowPos({ 0, 0 });

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.9f);
    ImGui::Text("Fragment Shader Source");
    if (ImGui::InputTextMultiline("##",
                                  m_shaderSource.data(),
                                  m_shaderSource.size(),
                                  { 0, 0.6f * sidePanelSize.y },
                                  ImGuiInputTextFlags_AllowTabInput)) {
        auto defaultStr = sf::err().rdbuf();
        std::stringstream errStream;
        sf::err().rdbuf(errStream.rdbuf());
        if (!m_shader.loadFromMemory(m_shaderSource, sf::Shader::Type::Fragment)) {
            m_errorString = errStream.str();
            m_didFailLastCompile = true;
        } else
            m_didFailLastCompile = false;

        sf::err().rdbuf(defaultStr);
    }
    ImGui::Text("In built variables");
    ImGui::Text("u_deltaTime = Delta Time");
    ImGui::Text("u_elapsedTime = Elapsed Time");
    ImGui::Text("u_resolution = Surface Resolution");
    ImGui::Text("u_mouse = Mouse Screen Position");
    ImGui::Text("u_frames = Number of frames elapsed");
    ImGui::PopItemWidth();
    ImGui::End();

    /*
    Errors Window
    */

    const auto errorsPanelSize = sf::Vector2f { BOTTOM_PANEL_WINDOW_WIDTH_PERCENT * renderWindowSize.x,
                                                renderWindowSize.y * BOTTOM_PANEL_WINDOW_HEIGHT_PERCENT };
    ImGui::Begin("Errors", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(errorsPanelSize);
    ImGui::SetWindowPos({ sidePanelSize.x, renderWindowSize.y - errorsPanelSize.y });
    if (m_didFailLastCompile) {
        ImGui::TextColored(ImVec4(sf::Color::Red), "Shader compile error!");
        ImGui::Text("%s", m_errorString.data());
    }
    ImGui::End();

    /*
    Export Window
    (Still deciding on preferred layout..)
    */

    // ImGui::Begin("Export", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    // ImGui::SetWindowSize(sidePanelSize);
    // ImGui::SetWindowPos({ renderWindowSize.x - sidePanelSize.x, 0 });
    // ImGui::End();
}

void App::setupShaderUniforms(const sf::Time& dt, const sf::Time& elapsed)
{
    // We failed to compile, so no point trying to pass uniforms
    if (m_didFailLastCompile)
        return;

    const auto renderTextureSize = sf::Vector2f { m_renderTexture.getSize() };

    m_shader.setUniform("u_deltaTime", dt.asSeconds());
    m_shader.setUniform("u_elapsedTime", elapsed.asSeconds());
    m_shader.setUniform("u_resolution", sf::Vector2f { m_renderTexture.getSize() });

    auto mousePosition = sf::Vector2f { sf::Mouse::getPosition(m_window) };
    const auto subtractAmount
        = sf::Vector2f { m_window.getView().getCenter().x - static_cast<float>(renderTextureSize.x) / 2.f,
                         m_window.getView().getCenter().y - static_cast<float>(renderTextureSize.y) / 2.f };
    mousePosition -= subtractAmount;
    mousePosition.x = std::clamp(mousePosition.x, 0.0f, renderTextureSize.x);
    mousePosition.y = std::clamp(mousePosition.y, 0.0f, renderTextureSize.y);
    mousePosition.y = renderTextureSize.y - mousePosition.y;

    m_shader.setUniform("u_mouse", mousePosition);
    m_shader.setUniform("u_frames", m_frames);
}
