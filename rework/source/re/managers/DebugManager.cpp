//
//  DebugManager.cpp
//  rework
//
//  Created by reworks on 20/04/2017.
//  Copyright (c) 2017 reworks. All rights reserved.
//

#include <sstream>
#include <fstream>

#include <allegro5/events.h>

#include "imgui/imgui.h"
#include "re/core/World.hpp"
#include "re/services/VFS.hpp"
#include "imgui/imgui_impl_a5.h"
#include "re/managers/StateManager.hpp"
#include "re/services/ServiceLocator.hpp"

#include "DebugManager.hpp"

namespace re
{
	// https://stackoverflow.com/a/217605
	void trimFromEnd(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	DebugManager::DebugManager(ALLEGRO_DISPLAY* display)
	:m_reloadState(nullptr), m_disabled(false);
	{
		m_world = Locator::get<World>();
		ImGui_ImplA5_Init(display);
	}

	DebugManager::~DebugManager()
	{
		ImGui_ImplA5_Shutdown();
	}

	void DebugManager::disable(bool isDisabled)
	{
		m_disabled = isDisabled;
	}

	void DebugManager::event(ALLEGRO_EVENT* event)
	{
		if (!m_disabled)
		{
			ImGui_ImplA5_ProcessEvent(event);
		}
	}

	void DebugManager::update()
	{
		if (!m_disabled)
		{
			ImGui_ImplA5_NewFrame();
		}
	}

	void DebugManager::render()
	{
		if (!m_disabled)
		{
			ImGui::Render();
		}
	}

	void DebugManager::displayMenu()
	{
		if (!m_disabled)
		{
			static int index = 0;
			static bool showScriptEditor = false;
			static std::vector<std::string> entityScripts;

			entityScripts.clear();
			entityScripts.reserve(m_world->m_entityScripts.size());
			for (auto const& map : m_world->m_entityScripts)
			{
				entityScripts.push_back(map.first);
			}

			ImGui::Begin("Debug Menu", (bool*)false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

			if (ImGui::Button("Reload State"))
			{
				showScriptEditor = false;
				m_reloadFunc();
				Locator::get<StateManager>()->reloadState(m_reloadState);
			}

			ImGui::Spacing();

			ImGui::Text("Entity Editor");
			ImGui::Spacing();

			ImGui::al::Combo("Entity Selector", &index, entityScripts);

			size_t size = entityScripts.size();
			if ((size_t)index >= size)
			{
				index = (size - 1);
			}

			std::string curEntityScriptName = entityScripts[index];
			std::string curEntityScriptData = m_world->m_entityScripts[entityScripts[index]];

			m_lua.script(curEntityScriptData);
			sol::table entityTable = m_lua.get<sol::table>("entity");
			ex::Entity& e = m_world->m_entitys[entityTable.get<std::string>("name")];

			std::map<std::string, sol::table> kvp;
			std::vector<std::string> componentNames;
			entityTable.for_each([&](std::pair<sol::object, sol::object> pair) {
				kvp.insert({ pair.first.as<std::string>(), pair.second.as<sol::table>() });
			});

			// Remove stuff that isn't components
			kvp.erase("name");
			kvp.erase("special");

			for (auto& it : kvp)
			{
				componentNames.push_back(it.first);
			}

			ImGui::Spacing();

			if (ImGui::Button("Toggle Script Editor"))
			{
				if (showScriptEditor == true)
				{
					showScriptEditor = false;
				}
				else
				{
					showScriptEditor = true;
				}
			}

			if (showScriptEditor)
			{
				static std::vector<char> scriptData(curEntityScriptData.begin(), curEntityScriptData.end());
				static bool doneOnce = false;
				static std::string first = curEntityScriptName;

				if (!doneOnce)
				{
					// this should be large enough...i hope
					scriptData.resize(4000);
					doneOnce = true;
				}

				if (first != m_world->m_loadedEntityScripts[index])
				{
					first = m_world->m_loadedEntityScripts[index];

					scriptData.clear();
					scriptData = std::vector<char>(curEntityScriptData.begin(), curEntityScriptData.end());
					scriptData.resize(4000);
				}

				ImGui::SetNextWindowSize(ImVec2(420, 500), ImGuiSetCond_FirstUseEver);
				ImGui::SetNextWindowPosCenter();

				std::string name = "Script Editor: " + curEntityScriptName;
				ImGui::Begin(name.c_str(), (bool*)false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::InputTextMultiline("", scriptData.data(), scriptData.size(), ImVec2(420, 500), ImGuiInputTextFlags_EnterReturnsTrue);

				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::Button("Save Changes"))
				{
					for (auto& v : scriptData)
					{
						if (!v)
						{
							v = ' ';
						}
					}

					std::string path = Locator::get<VFS>()->getBasePath() + curEntityScriptName;

					std::ofstream out;
					out.open(path, std::ofstream::out);

					std::string dataOut(scriptData.begin(), scriptData.end());
					trimFromEnd(dataOut);
					dataOut.erase(std::remove(dataOut.begin(), dataOut.end(), '\r'), dataOut.end());

					dataOut.shrink_to_fit();
					out << dataOut << std::endl;
					out.close();

					scriptData.clear();
					scriptData = std::vector<char>(dataOut.begin(), dataOut.end());
					scriptData.resize(4000);
				}

				ImGui::End();
			}

			ImGui::Spacing();
			ImGui::al::Combo("Component Selector", &indexComponent, componentNames);

			if ((size_t)indexComponent >= componentNames.size())
			{
				indexComponent = (componentNames.size() - 1);
			}

			std::string curComponent = componentNames[indexComponent];

			ImGui::Separator();
			ImGui::Spacing();

			// Do component stuff???

			ImGui::End();
		}
	}

	void DebugManager::specifyReloadState(std::shared_ptr<State> s, std::function<void(void)> func)
	{
		m_reloadState = s;
		m_reloadFunc = func;
	}
}