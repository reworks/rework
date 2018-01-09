///
///  AnimationComponent.cpp
///  rework
///
///  Created by reworks on 16/08/2016.
///  Copyright (c) 2018+ reworks.
///  Refer to LICENSE.txt for more details.
///

#include "sol2/sol.hpp"
#include "loguru/loguru.hpp"
#include "imgui/imgui_impl_a5.h"

#include "AnimationComponent.hpp"

namespace re
{
	AnimationComponent::AnimationComponent(const sol::table& table)
		:m_currentFrameTime(0.0)
	{
		m_isPaused = table.get<bool>("paused");
		m_activeAnimation = table.get<std::string>("defaultAnim");

		sol::table animTable = table.get<sol::table>("Animations");
		if (!animTable.empty())
		{
			animTable.for_each([&](std::pair<sol::object, sol::object> pair)
			{
				m_animations.emplace(pair.first.as<std::string>(), pair.second.as<sol::table>());
			});
		}
		else
		{
			LOG_S(WARNING) << "Tried to load AnimationComponent with no Animations!";
		}
	}

	AnimationComponent::~AnimationComponent()
	{
		m_animations.clear();
	}

	void AnimationComponent::changeAnimation(const std::string& animation)
	{
		m_animations[m_activeAnimation].m_currentFrame = 0;
		m_currentFrameTime = 0.0;
		m_activeAnimation = animation;
	}

	void AnimationComponent::play()
	{
		m_isPaused = false;
	}

	void AnimationComponent::play(const std::string& animation)
	{
		if (m_activeAnimation != animation)
		{
			changeAnimation(animation);
		}
			
		play();
	}

	void AnimationComponent::pause()
	{
		m_isPaused = true;
	}

	void AnimationComponent::stop()
	{
		m_isPaused = true;
		m_animations[m_activeAnimation].m_currentFrame = 0;
		m_currentFrameTime = 0.0;
	}

	void AnimationComponent::debug()
	{
		ImGui::Checkbox("Is Paused", &m_isPaused);

		ImGui::Spacing();
		if (ImGui::stl::InputText("Active Animation", &m_activeAnimation, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			changeAnimation(m_activeAnimation);
			play();
		}

		ImGui::Spacing();
		m_animations[m_activeAnimation].debug();

		ImGui::Spacing();
		if (ImGui::Button("Play animation"))
		{
			play();
		}

		ImGui::Spacing();
		if (ImGui::Button("Pause animation"))
		{
			pause();
		}

		ImGui::Spacing();
		if (ImGui::Button("Stop Animation"))
		{
			stop();
		}
	}
}