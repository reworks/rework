///
/// Scene2D.cpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#include "galaxy/core/ServiceLocator.hpp"
#include "galaxy/systems/AnimationSystem.hpp"
#include "galaxy/systems/CollisionSystem.hpp"
#include "galaxy/systems/RenderSystem2D.hpp"
#include "galaxy/systems/TransformSystem.hpp"

#include "Scene2D.hpp"

namespace galaxy
{
	namespace scene
	{
		Scene2D::Scene2D(std::string_view name) noexcept
		    : Scene {name}, m_active_map {""}
		{
			m_camera.create(0.0f, SL_HANDLE.window()->get_width(), SL_HANDLE.window()->get_height(), 0.0f);
			m_camera.set_speed(100.0f);

			m_dispatcher.subscribe<events::KeyDown>(m_camera);
			m_dispatcher.subscribe<events::KeyUp>(m_camera);
			m_dispatcher.subscribe<events::MouseWheel>(m_camera);
			m_dispatcher.subscribe<events::WindowResized>(m_camera);

			m_world.create_system<systems::AnimationSystem>();
			m_world.create_system<systems::TransformSystem>();
			m_world.create_system<systems::RenderSystem2D>();
			m_world.create_system<systems::CollisionSystem>();
		}

		Scene2D::~Scene2D() noexcept
		{
		}

		void Scene2D::on_push()
		{
			SL_HANDLE.window()->set_scene_dispatcher(&m_dispatcher);
		}

		void Scene2D::on_pop()
		{
			SL_HANDLE.window()->set_scene_dispatcher(nullptr);
		}

		void Scene2D::events()
		{
		}

		void Scene2D::update(const double dt)
		{
			m_camera.update(dt);
			m_world.update(dt);
			m_gui.update(dt);
		}

		void Scene2D::pre_render()
		{
		}

		void Scene2D::render()
		{
			m_world.get_system<systems::RenderSystem2D>()->render(m_world, m_camera);
			m_gui.render();
		}

		void Scene2D::create_maps(std::string_view path)
		{
			m_maps.clear();
			m_maps.load(path);
			if (!m_maps.parse(m_world))
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to parse tiled world.");
			}
		}

		map::Map* Scene2D::get_map(std::string_view name)
		{
			return m_maps.get_map(name);
		}

		map::Map* Scene2D::get_active_map()
		{
			return m_maps.get_map(m_active_map);
		}

		void Scene2D::set_active_map(std::string_view name)
		{
			m_active_map = static_cast<std::string>(name);
		}

		nlohmann::json Scene2D::serialize()
		{
			nlohmann::json json = "{}"_json;

			json["name"]   = m_name;
			json["type"]   = "2D";
			json["camera"] = m_camera.serialize();
			json["world"]  = m_world.serialize();
			json["theme"]  = m_gui_theme.serialize();
			json["gui"]    = m_gui.serialize();

			return json;
		}

		void Scene2D::deserialize(const nlohmann::json& json)
		{
			m_name = json.at("name");

			const auto camera_json = json.at("camera");
			const auto world_json  = json.at("world");

			m_camera.deserialize(camera_json);
			m_world.deserialize(world_json);

			m_gui_theme.deserialize(json.at("theme"));
			m_gui.set_theme(&m_gui_theme);
			m_gui.deserialize(json.at("gui"));
		}
	} // namespace scene
} // namespace galaxy