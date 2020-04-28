///
/// Object.hpp
/// starmap
///
/// Refer to LICENSE.txt for more details.
///

#ifndef STARMAP_OBJECT_HPP_
#define STARMAP_OBJECT_HPP_

#include "starmap/types/Text.hpp"
#include "starmap/types/Point.hpp"
#include "starmap/types/Property.hpp"

///
/// Core namespace.
///
namespace starmap
{
	///
	/// Object representation in tiled.
	///
	class Object final
	{
	public:
		///
		/// Default constructor.
		///
		Object() noexcept;

		///
		/// \brief Parse constructor.
		///
		/// Can throw exceptions.
		///
		/// \param json JSON structure/array containing object.
		///
		explicit Object(const nlohmann::json& json) noexcept;

		///
		/// Destructor.
		///
		~Object() noexcept;

		///
		/// \brief Parse object level json.
		///
		/// Can throw exceptions.
		///
		/// \param json JSON structure/array containing object.
		///
		void parse(const nlohmann::json& json) noexcept;

		///
		/// Get ellipse flag.
		///
		/// \return True if ellipse shaped.
		///
		const bool isEllipse() const noexcept;

		///
		/// Get gid.
		///
		/// \return Gid as const int.
		///
		const int getGID() const noexcept;

		///
		/// Get name.
		///
		/// \return Name as std::string.
		///
		const std::string& getName() const noexcept;

		///
		/// Get point flag.
		///
		/// \return True if object is point shaped.
		///
		const bool isPoint() const noexcept;

		///
		/// Gets points.
		///
		/// \return Points as std::vector array.
		///
		const auto& getPoints() const noexcept;

		///
		/// Retrieve property.
		/// You will need to provide the type when retrieving.
		///
		/// \param name Name of the property to retrieve.
		///
		/// \return Property cast as type.
		///
		template<typename T>
		const T getProperty(const std::string& name) noexcept;

		///
		/// Get rotation of object.
		///
		/// \return Double - angle in degrees clockwise.
		///
		const double getRotation() const noexcept;

		///
		/// Get template reference.
		///
		/// \return String reference to a template file, in case object is a template instance.
		///
		const std::string& getTemplate() const noexcept;

		///
		/// Get text object (if object is text based).
		///
		/// \return Const reference to a text object.
		///
		const starmap::Text& getText() const noexcept;

		///
		/// Get type string.
		///
		/// \return String assigned to type field in editor.
		///
		const std::string& getType() const noexcept;

		///
		/// Get visibility.
		///
		/// \return True if object is visible.
		///
		const bool isVisible() const noexcept;

		///
		/// Get width of object.
		///
		/// \return Width in pixels as double.
		///
		const double getWidth() const noexcept;

		///
		/// Get width of object.
		///
		/// \return Width in pixels as double.
		///
		const double getX() const noexcept;

		///
		/// Get width of object.
		///
		/// \return Width in pixels as double.
		///
		const double getY() const noexcept;

	private:
		///
		/// Used to mark an object as an ellipse.
		///
		bool m_ellipse;

		///
		/// Global tile ID, only if object represents a tile.
		///
		int m_gid;

		///
		/// Height in pixels.
		///
		double m_height;

		///
		/// Incremental id, unique across all objects.
		///
		int m_id;

		///
		/// String assigned to name field in editor.
		///
		std::string m_name;

		///
		/// Used to mark an object as a point.
		///
		bool m_point;

		///
		/// Array of Points, in case the object is a polygon/polyline
		///
		std::vector<starmap::Point> m_points;

		///
		/// Map of Properties.
		///
		std::unordered_map<std::string, starmap::Property> m_properties;

		///
		/// Angle in degrees clockwise.
		///
		double m_rotation;

		///
		/// Reference to a template file, in case object is a template instance.
		///
		std::string m_template;

		///
		/// Only used for text objects.
		///
		starmap::Text m_text;

		///
		/// String assigned to type field in editor.
		///
		std::string m_type;

		///
		/// Whether object is shown.
		///
		bool m_visible;

		///
		/// Width in pixels.
		///
		double m_width;

		///
		/// X coordinate in pixels.
		///
		double m_x;

		///
		/// Y coordinate in pixels.
		///
		double m_y;
	};

	template<typename T>
	inline const T Object::getProperty(const std::string& name) noexcept
	{
		return m_properties[name].get<T>();
	}
}

#endif