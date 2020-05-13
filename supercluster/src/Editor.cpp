///
/// Editor.cpp
/// supercluster
///
/// Refer to LICENSE.txt for more details.
///

#include <filesystem>

#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <qs/core/Window.hpp>
#include <imgui_impl_opengl3.h>
#include <galaxy/core/World.hpp>
#include <qs/shaders/Sprites.hpp>
#include <solar/entity/Manager.hpp>
#include <pfd/portable-file-dialogs.h>
#include <galaxy/core/ServiceLocator.hpp>
#include <galaxy/systems/RenderSystem.hpp>

#include "Editor.hpp"

namespace sc
{
	Editor::Editor() noexcept
		:m_name("Editor"), m_showEUI(false), m_showCUI(false), m_isFileOpen(false), m_currentOpenFile(nullptr)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsClassic();

		m_window = SL_HANDLE.window();
		m_world = SL_HANDLE.world();
		ImGui_ImplGlfw_InitForOpenGL(m_window->getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450 core");

		m_spriteShader.loadFromRaw(qs::s_spriteVS, qs::s_spriteFS);
		m_camera.create(0.0f, m_window->getWidth(), m_window->getHeight(), 0.0f);
	}

	Editor::~Editor() noexcept
	{
	}

	void Editor::onPush() noexcept
	{
	}

	void Editor::onPop() noexcept
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		m_window = nullptr;
	}

	void Editor::events() noexcept
	{
		if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			m_window->close();
		}

		m_world->events();
	}

	void Editor::update(protostar::ProtectedDouble* deltaTime) noexcept
	{
		m_world->update(deltaTime);
		m_camera.update(deltaTime->get());
	}

	void Editor::render() noexcept
	{
		m_spriteShader.bind();
		m_spriteShader.setUniform<glm::mat4>("u_cameraProj", m_camera.getProj());
		m_spriteShader.setUniform<glm::mat4>("u_cameraView", m_camera.getTransformation());
		
		m_world->get<galaxy::RenderSystem>()->render(m_world, m_spriteShader);

		start();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					m_window->close();
				}

				if (ImGui::MenuItem("Restart"))
				{
					SL_HANDLE.m_restart = true;
					m_window->close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ECS"))
			{
				if (ImGui::MenuItem("Entity Manager"))
				{
					m_showEUI = !m_showEUI;
				}

				if (ImGui::MenuItem("Component Manager"))
				{
					m_showCUI = !m_showCUI;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (m_showEUI)
		{
			entityUI();
		}

		if (m_showCUI)
		{
			componentUI();
		}

		end();
	}

	void Editor::entityUI() noexcept
	{
		static bool show = true;
		static std::filesystem::path path = "";
		ImGui::Begin("Entities", &show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button("Open Definition"))
		{
			if (!m_isFileOpen)
			{
				m_isFileOpen = true;
				m_currentOpenFile = std::make_unique<pfd::open_file>("Open entity JSON definition.");
			}
		}

		if ((m_isFileOpen && m_currentOpenFile->ready()) && (!m_currentOpenFile->result().empty()))
		{
			path = std::filesystem::path(m_currentOpenFile->result()[0]);
			m_isFileOpen = false;
			m_currentOpenFile.reset();
			m_currentOpenFile = nullptr;
		}

		if (!path.empty())
		{
			SL_HANDLE.world()->createFromJSON(path.string());
			path = "";
		}
		
		ImGui::End();
	}
	
	void Editor::componentUI() noexcept
	{
		static bool show = true;
		ImGui::Begin("Entities", &show, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::End();
	}

	void Editor::start() noexcept
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::end() noexcept
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}