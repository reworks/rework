//
//  RenderSystem.cpp
//  REngine3
//
//  Created by reworks on 6/08/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#include "re/app/World.hpp"
#include "re/graphics/Window.hpp"
#include "re/component/PositionComponent.hpp"

#include "RenderSystem.hpp"

namespace re
{
	RenderSystem::RenderSystem(int numOfGroups)
	{
		int max = numOfGroups + 1;

		m_groups.clear();
		m_groups.resize(0);
		m_groups.reserve(max);

		for (int i = 0; i < max; i++)
		{
			m_groups.push_back(Group());
		}
	}

	RenderSystem::~RenderSystem()
	{
		m_groups.clear();
		m_entitys.clear();
	}

	void RenderSystem::AutoSubmit(World* world)
	{
		for (auto& it : world->GetAlive())
		{
			if (it.second.Has<SpriteComponent>() || it.second.Has<TextComponent>() || it.second.Has<AnimatedSpriteComponent>() || it.second.Has<TimeComponent>())
			{
				AddEntity(&it.second);
			}
		}
	}

	void RenderSystem::AddEntity(Entity* e)
	{
		e->m_systemIds.emplace("RenderSystem", std::type_index(typeid(RenderSystem)));

		if (e->Has<SpriteComponent>())
		{
			m_groups[e->Get<SpriteComponent>()->m_group].AddDrawable(e->m_name, e->Get<SpriteComponent>(), e->Get<PositionComponent>());
		}

		if (e->Has<TextComponent>())
		{
			m_groups[e->Get<TextComponent>()->m_group].AddDrawable(e->m_name, e->Get<TextComponent>(), nullptr);
		}

		if (e->Has<AnimatedSpriteComponent>())
		{
			m_groups[e->Get<AnimatedSpriteComponent>()->m_group].AddDrawable(e->m_name, e->Get<AnimatedSpriteComponent>(), e->Get<PositionComponent>());
		}

		if (e->Has<TimeComponent>())
		{
			m_groups[e->Get<TimeComponent>()->m_group].AddDrawable(e->m_name, e->Get<TimeComponent>(), nullptr);
		}
	}

	void RenderSystem::RemoveEntity(const std::string& name)
	{
		for (auto& v : m_groups)
		{
			auto found = v.GetDrawableMap().find(name);
			if (found != v.GetDrawableMap().end())
			{
				v.GetDrawableMap().erase(name);
			}
		}
	}

	void RenderSystem::Render(Window* window)
	{
		for (auto& g : m_groups)
		{
			window->draw(g);
		}
	}

	void RenderSystem::Clean()
	{
		for (auto& v : m_groups)
		{
			v.GetDrawableMap().clear();
		}

		m_entitys.clear();
	}
}