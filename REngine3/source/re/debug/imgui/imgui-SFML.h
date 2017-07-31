#pragma once

#include <vector>
#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

#include "imgui.h"

namespace sf
{
    class Event;
    class RenderTarget;
    class RenderWindow;
    class Sprite;
    class Texture;
    class Window;
}

namespace ImGui
{
namespace SFML
{
    void Init(sf::RenderTarget& target, bool loadDefaultFont = true);

    void ProcessEvent(const sf::Event& event);

    void Update(sf::RenderWindow& window, sf::Time dt);
    void Update(sf::Window& window, sf::RenderTarget& target, sf::Time dt);
    void Update(const sf::Vector2i& mousePos, const sf::Vector2f& displaySize, sf::Time dt);

    void Render(sf::RenderTarget& target);

    void Shutdown();

    void UpdateFontTexture();
    sf::Texture& GetFontTexture();

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	inline bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ImGui::Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	inline bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ImGui::ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}
}

// custom ImGui widgets for SFML stuff

// Image overloads
    void Image(const sf::Texture& texture,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);
    void Image(const sf::Texture& texture, const sf::Vector2f& size,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);
    void Image(const sf::Texture& texture, const sf::FloatRect& textureRect,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);
    void Image(const sf::Texture& texture, const sf::Vector2f& size, const sf::FloatRect& textureRect,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);

    void Image(const sf::Sprite& sprite,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);
    void Image(const sf::Sprite& sprite, const sf::Vector2f& size,
        const sf::Color& tintColor = sf::Color::White,
        const sf::Color& borderColor = sf::Color::Transparent);

// ImageButton overloads
    bool ImageButton(const sf::Texture& texture, const int framePadding = -1,
                     const sf::Color& bgColor = sf::Color::Transparent,
                     const sf::Color& tintColor = sf::Color::White);
    bool ImageButton(const sf::Texture& texture, const sf::Vector2f& size, const int framePadding = -1,
                     const sf::Color& bgColor = sf::Color::Transparent, const sf::Color& tintColor = sf::Color::White);

    bool ImageButton(const sf::Sprite& sprite, const int framePadding = -1,
                     const sf::Color& bgColor = sf::Color::Transparent,
                     const sf::Color& tintColor = sf::Color::White);
    bool ImageButton(const sf::Sprite& sprite, const sf::Vector2f& size, const int framePadding = -1,
                     const sf::Color& bgColor = sf::Color::Transparent,
                     const sf::Color& tintColor = sf::Color::White);

// Draw_list overloads. All positions are in relative coordinates (relative to top-left of the current window)
    void DrawLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Color& col, float thickness = 1.0f);
    void DrawRect(const sf::FloatRect& rect, const sf::Color& color, float rounding = 0.0f, int rounding_corners = 0x0F, float thickness = 1.0f);
    void DrawRectFilled(const sf::FloatRect& rect, const sf::Color& color, float rounding = 0.0f, int rounding_corners = 0x0F);
}
