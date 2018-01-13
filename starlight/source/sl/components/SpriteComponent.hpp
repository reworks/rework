///
///  SpriteComponent.hpp
///  starlight
///
///  Created by reworks on 12/08/2016.
///  Copyright (c) 2018+ reworks.
///  Refer to LICENSE.txt for more details.
///

#ifndef STARLIGHT_SPRITECOMPONENT_HPP_
#define STARLIGHT_SPRITECOMPONENT_HPP_

#include "sol2/sol_forward.hpp"
#include "entt/core/hashed_string.hpp"

namespace sl
{
	class SpriteComponent final
	{
	public:
		///
		/// Constructor.
		///
		/// \param table sol::table containing data.
		///
		SpriteComponent(const sol::table& table);

		///
		/// Move Constructor.
		///
		SpriteComponent(SpriteComponent&&) = default;

		///
		/// Destructor.
		///
		~SpriteComponent() = default;
		
		///
		/// Calls imgui debug functions. Don't call this, done for you by debugmanager.
		///
		void debug();

	public:
		entt::HashedString m_spriteName;

	private:
		///
		/// Default constructor.
		/// Deleted.
		///
		SpriteComponent() = delete;

		///
		/// Copy Constructor.
		/// Deleted.
		///
		SpriteComponent(const SpriteComponent&) = delete;
	};
}

#endif