///
/// Grid.cpp
/// starmap
///
/// Refer to LICENSE.txt for more details.
///

#include <nlohmann/json.hpp>

#include "Grid.hpp"

///
/// Core namespace.
///
namespace starmap
{
	Grid::Grid() noexcept
		:m_height(0), m_orientation("orthogonal"), m_width(0)
	{
	}

	Grid::Grid(const nlohmann::json& json) noexcept
		:m_height(0), m_orientation("orthogonal"), m_width(0)
	{
		parse(json);
	}

	void Grid::parse(const nlohmann::json& json) noexcept
	{
		if (json.count("height") > 0)
		{
			m_height = json.at("height");
		}
		
		if (json.count("orientation") > 0)
		{
			m_orientation = json.at("orientation");
		}

		if (json.count("width") > 0)
		{
			m_width = json.at("width");
		}
	}

	const int Grid::getHeight() const noexcept
	{
		return m_height;
	}

	const std::string& Grid::getOrientation() const noexcept
	{
		return m_orientation;
	}

	const int Grid::getWidth() const noexcept
	{
		return m_width;
	}
}