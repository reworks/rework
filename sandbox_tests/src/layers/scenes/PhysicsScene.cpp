///
/// PhysicsScene.cpp
/// sandbox_tests
///
/// Refer to LICENSE.txt for more details.
///

#include <galaxy/core/ServiceLocator.hpp>

#include <galaxy/systems/PhysicsSystem.hpp>
#include <galaxy/systems/RenderSystem.hpp>

#include "PhysicsScene.hpp"

using namespace galaxy;

namespace sb
{
	PhysicsScene::PhysicsScene()
	{
		m_camera.create(0.0f, SL_HANDLE.window()->get_width(), SL_HANDLE.window()->get_height(), 0.0f);
		m_camera.set_speed(100.0f);
		SL_HANDLE.window()->set_on_scroll([&](GLFWwindow* window, double x, double y) {
			m_camera.scale(x);
		});
		SL_HANDLE.window()->register_on_window_resize(m_camera);

		m_world.create_system<systems::PhysicsSystem>();
		m_world.create_system<systems::RenderSystem>();

		m_world.set_gravity(0.0f, 0.0f);
	}

	PhysicsScene::~PhysicsScene()
	{
	}

	void PhysicsScene::events()
	{
		if (SL_HANDLE.window()->key_pressed(input::Keys::SPACE))
		{
		}
	}

	void PhysicsScene::update(const double dt)
	{
		m_camera.update(dt);
		m_world.update(dt);
	}

	void PhysicsScene::pre_render()
	{
	}

	void PhysicsScene::render()
	{
		m_world.get_system<systems::RenderSystem>()->render(m_world, m_camera);
	}
} // namespace sb