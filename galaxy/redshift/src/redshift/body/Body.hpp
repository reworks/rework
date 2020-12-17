///
/// Body.hpp
/// redshift
///
/// Refer to LICENSE.txt for more details.
///

#ifndef REDSHIFT_BODY_HPP_
#define REDSHIFT_BODY_HPP_

#include <glm/vec2.hpp>

#include "redshift/collision/Collidable.hpp"

///
/// Core namespace.
///
namespace rs
{
	class Body : public rs::Collidable
	{
		friend class World;

	public:
		///
		/// Virtual destructor.
		///
		virtual ~Body() = default;

		///
		/// Can this body move.
		///
		/// \return Const bool.
		///
		virtual const bool is_rigid() const noexcept = 0;

		///
		/// Current position of the body.
		///
		glm::vec2 m_pos;

		///
		/// Restitution of this body.
		///
		float m_restitution;

		///
		/// Friction of surface when an object is resting on it.
		///
		float m_static_friction;

		///
		/// Friction of surface when an object is moving along it.
		///
		float m_dynamic_friction;

	protected:
		///
		/// Argument constructor.
		///
		Body(const float density, const glm::vec2& size);

		///
		/// Argument constructor.
		///
		Body(const float density, const float width, const float height);

		///
		/// Mass of this body.
		///
		float m_mass;

		///
		/// Velocity of this body.
		///
		glm::vec2 m_velocity;

	private:
		///
		/// Constructor.
		///
		Body() = delete;
	};
} // namespace rs

#endif