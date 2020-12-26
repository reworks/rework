///
/// System.hpp
/// galaxy
///
/// See LICENSE.txt.
///

#ifndef GALAXY_ECS_SYSTEM_HPP_
#define GALAXY_ECS_SYSTEM_HPP_

namespace galaxy
{
	namespace ecs
	{
		///
		/// Represents a system that operates on sets of components.
		///
		class System
		{
		public:
			///
			/// Default virtual destructor.
			///
			virtual ~System() = default;

			///
			/// Abstract implementation for processing events.
			///
			virtual void events() = 0;

			///
			/// Abstract implementation for updating the system. Use the manager to retreive your components.
			///
			/// \param dt "Lag" from gameloop.
			///
			virtual void update(const double dt) = 0;

		protected:
			///
			/// Default constructor.
			///
			System() = default;
		};
	} // namespace ecs
} // namespace galaxy

#endif