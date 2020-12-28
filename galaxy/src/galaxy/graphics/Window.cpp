///
/// Window.cpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#include <filesystem>
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "galaxy/error/Log.hpp"
#include "galaxy/graphics/post/PostEffect.hpp"
#include "galaxy/graphics/Renderer.hpp"
#include "galaxy/graphics/WindowSettings.hpp"

#include "Window.hpp"

namespace galaxy
{
	namespace graphics
	{
		Window::Window()
		    : m_window {nullptr}, m_cursor {nullptr}, m_width {0}, m_height {0}, m_colour {1.0f, 1.0f, 1.0f, 1.0f}, m_text_input {""}, m_inputting_text {false}, m_framebuffer {nullptr}, m_fb_sprite {nullptr}
		{
			m_prev_mouse_btn_states = {GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE};
		}

		Window::Window(WindowSettings& settings)
		    : m_window {nullptr}, m_cursor {nullptr}, m_width {0}, m_height {0}, m_colour {1.0f, 1.0f, 1.0f, 1.0f}, m_text_input {""}, m_inputting_text {false}, m_framebuffer {nullptr}, m_fb_sprite {nullptr}
		{
			m_prev_mouse_btn_states = {GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE};

			if (!create(settings))
			{
				GALAXY_LOG(GALAXY_FATAL, "GLFW window creation failed.");
			}
		}

		Window::~Window()
		{
			// Call again to ensure everything is cleaned up.
			// Has checks to ensure no null data is destroyed.
			destroy();
		}

		bool Window::create(WindowSettings& settings)
		{
			// Function result.
			bool result = true;

			// Window w/h.
			m_width  = settings.m_width;
			m_height = settings.m_height;

			// Error callbacks.
			glfwSetErrorCallback([](int error, const char* description) {
				GALAXY_LOG(GALAXY_ERROR, "[GLFW] Code: {0}. Desc: {1}.", error, description);
			});

			// Init glfw.
			if (!glfwInit())
			{
				GALAXY_LOG(GALAXY_FATAL, "Failed to initialize glfw!");
			}
			else
			{
				// Set the OpenGL flags.
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

				// Set debug when compiling for debug mode.
#ifdef _DEBUG
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

				// Window related hints.
				glfwWindowHint(GLFW_RESIZABLE, true);
				glfwWindowHint(GLFW_VISIBLE, true);
				glfwWindowHint(GLFW_DECORATED, true);
				glfwWindowHint(GLFW_FOCUSED, true);

				// Framebuffer stuff
				glfwWindowHint(GLFW_RED_BITS, 8);
				glfwWindowHint(GLFW_GREEN_BITS, 8);
				glfwWindowHint(GLFW_BLUE_BITS, 8);
				glfwWindowHint(GLFW_ALPHA_BITS, 8);
				glfwWindowHint(GLFW_DEPTH_BITS, 24);
				glfwWindowHint(GLFW_STENCIL_BITS, 8);
				glfwWindowHint(GLFW_DOUBLEBUFFER, true);
				glfwWindowHint(GLFW_SRGB_CAPABLE, true);

				// MSAA
				glfwWindowHint(GLFW_SAMPLES, settings.m_anti_aliasing);

				// Create the window from input, ensuring it is centered in the screen.
				m_window = glfwCreateWindow(m_width, m_height, settings.m_title.c_str(), nullptr, nullptr);

				// Then if the window failed to create:
				if (!m_window)
				{
					GALAXY_LOG(GALAXY_FATAL, "Failed to create window.");
					result = false;
				}
				else
				{
					// Set window context and aspect ratio.
					glfwMakeContextCurrent(m_window);

					if (!(settings.m_aspect_ratio_x == -1 || settings.m_aspect_ratio_y == -1))
					{
						glfwSetWindowAspectRatio(m_window, settings.m_aspect_ratio_x, settings.m_aspect_ratio_y);
					}

					// Set up glad.
					if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
					{
						GALAXY_LOG(GALAXY_FATAL, "Failed to init glad.");
						result = false;
					}
					else
					{
						// Set internal pointer references.
						glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));

						// Set resize callback.
						glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int w, int h) {
							reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->resize(w, h);
						});

						// Set vsync.
						glfwSwapInterval(settings.m_vsync);

						// Raw mouse input.
						if (glfwRawMouseMotionSupported())
						{
							glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, settings.m_raw_mouse_input);
						}

						glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
							Window* qs_win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
							if (qs_win->m_inputting_text)
							{
								qs_win->m_text_input += static_cast<char>(codepoint);
							}
						});

						if (settings.m_gl_debug)
						{
							glEnable(GL_DEBUG_OUTPUT);
							glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) -> void {
								GALAXY_LOG(GALAXY_WARNING, "[OpenGL] - [ {0}, {1}, {2}, {3}, {4} ] - {5}.", source, type, id, severity, length, message);
							},
									       nullptr);
						}

						// Enable MSAA.
						glEnable(GL_MULTISAMPLE);

						// Set up blending.
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						// Allow for changing vertex point size.
						glEnable(GL_PROGRAM_POINT_SIZE);

						// sRGB colours for rendering.
						if (settings.m_srgb)
						{
							glEnable(GL_FRAMEBUFFER_SRGB);
						}

						// Set custom line width.
						glLineWidth(settings.m_line_thickness);

						m_mousebutton_map.reserve(12);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_1, GLFW_MOUSE_BUTTON_1);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_2, GLFW_MOUSE_BUTTON_2);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_3, GLFW_MOUSE_BUTTON_3);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_4, GLFW_MOUSE_BUTTON_4);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_5, GLFW_MOUSE_BUTTON_5);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_6, GLFW_MOUSE_BUTTON_6);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_7, GLFW_MOUSE_BUTTON_7);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_8, GLFW_MOUSE_BUTTON_8);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_LAST, GLFW_MOUSE_BUTTON_LAST);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_LEFT, GLFW_MOUSE_BUTTON_LEFT);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_RIGHT, GLFW_MOUSE_BUTTON_RIGHT);
						m_mousebutton_map.emplace(input::MouseButton::BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_MIDDLE);

						m_keymap.reserve(102);
						m_keymap.emplace(input::Keys::A, GLFW_KEY_A);
						m_keymap.emplace(input::Keys::B, GLFW_KEY_B);
						m_keymap.emplace(input::Keys::C, GLFW_KEY_C);
						m_keymap.emplace(input::Keys::D, GLFW_KEY_D);
						m_keymap.emplace(input::Keys::E, GLFW_KEY_E);
						m_keymap.emplace(input::Keys::F, GLFW_KEY_F);
						m_keymap.emplace(input::Keys::G, GLFW_KEY_G);
						m_keymap.emplace(input::Keys::H, GLFW_KEY_H);
						m_keymap.emplace(input::Keys::I, GLFW_KEY_I);
						m_keymap.emplace(input::Keys::J, GLFW_KEY_J);
						m_keymap.emplace(input::Keys::K, GLFW_KEY_K);
						m_keymap.emplace(input::Keys::L, GLFW_KEY_L);
						m_keymap.emplace(input::Keys::M, GLFW_KEY_M);
						m_keymap.emplace(input::Keys::N, GLFW_KEY_N);
						m_keymap.emplace(input::Keys::O, GLFW_KEY_O);
						m_keymap.emplace(input::Keys::P, GLFW_KEY_P);
						m_keymap.emplace(input::Keys::Q, GLFW_KEY_Q);
						m_keymap.emplace(input::Keys::R, GLFW_KEY_R);
						m_keymap.emplace(input::Keys::S, GLFW_KEY_S);
						m_keymap.emplace(input::Keys::T, GLFW_KEY_T);
						m_keymap.emplace(input::Keys::U, GLFW_KEY_U);
						m_keymap.emplace(input::Keys::V, GLFW_KEY_V);
						m_keymap.emplace(input::Keys::W, GLFW_KEY_W);
						m_keymap.emplace(input::Keys::X, GLFW_KEY_X);
						m_keymap.emplace(input::Keys::Y, GLFW_KEY_Y);
						m_keymap.emplace(input::Keys::Z, GLFW_KEY_Z);
						m_keymap.emplace(input::Keys::NUM_1, GLFW_KEY_1);
						m_keymap.emplace(input::Keys::NUM_2, GLFW_KEY_2);
						m_keymap.emplace(input::Keys::NUM_3, GLFW_KEY_3);
						m_keymap.emplace(input::Keys::NUM_4, GLFW_KEY_4);
						m_keymap.emplace(input::Keys::NUM_5, GLFW_KEY_5);
						m_keymap.emplace(input::Keys::NUM_6, GLFW_KEY_6);
						m_keymap.emplace(input::Keys::NUM_7, GLFW_KEY_7);
						m_keymap.emplace(input::Keys::NUM_8, GLFW_KEY_8);
						m_keymap.emplace(input::Keys::NUM_9, GLFW_KEY_9);
						m_keymap.emplace(input::Keys::NUM_0, GLFW_KEY_0);
						m_keymap.emplace(input::Keys::MINUS, GLFW_KEY_MINUS);
						m_keymap.emplace(input::Keys::EQUALS, GLFW_KEY_EQUAL);
						m_keymap.emplace(input::Keys::BACKSPACE, GLFW_KEY_BACKSPACE);
						m_keymap.emplace(input::Keys::GRAVE, GLFW_KEY_GRAVE_ACCENT);
						m_keymap.emplace(input::Keys::TAB, GLFW_KEY_TAB);
						m_keymap.emplace(input::Keys::CAPS, GLFW_KEY_CAPS_LOCK);
						m_keymap.emplace(input::Keys::LSHIFT, GLFW_KEY_LEFT_SHIFT);
						m_keymap.emplace(input::Keys::LCNTRL, GLFW_KEY_LEFT_CONTROL);
						m_keymap.emplace(input::Keys::LSTART, GLFW_KEY_LEFT_SUPER);
						m_keymap.emplace(input::Keys::LALT, GLFW_KEY_LEFT_ALT);
						m_keymap.emplace(input::Keys::SPACE, GLFW_KEY_SPACE);
						m_keymap.emplace(input::Keys::RALT, GLFW_KEY_RIGHT_ALT);
						m_keymap.emplace(input::Keys::RSTART, GLFW_KEY_RIGHT_SUPER);
						m_keymap.emplace(input::Keys::MENU, GLFW_KEY_MENU);
						m_keymap.emplace(input::Keys::RCNTRL, GLFW_KEY_RIGHT_CONTROL);
						m_keymap.emplace(input::Keys::RSHIFT, GLFW_KEY_RIGHT_SHIFT);
						m_keymap.emplace(input::Keys::ENTER, GLFW_KEY_ENTER);
						m_keymap.emplace(input::Keys::SEMICOLON, GLFW_KEY_SEMICOLON);
						m_keymap.emplace(input::Keys::APOSTROPHE, GLFW_KEY_APOSTROPHE);
						m_keymap.emplace(input::Keys::SLASH, GLFW_KEY_SLASH);
						m_keymap.emplace(input::Keys::PERIOD, GLFW_KEY_PERIOD);
						m_keymap.emplace(input::Keys::COMMA, GLFW_KEY_COMMA);
						m_keymap.emplace(input::Keys::LBRACKET, GLFW_KEY_LEFT_BRACKET);
						m_keymap.emplace(input::Keys::RBRACKET, GLFW_KEY_RIGHT_BRACKET);
						m_keymap.emplace(input::Keys::BACKSLASH, GLFW_KEY_BACKSLASH);
						m_keymap.emplace(input::Keys::ESC, GLFW_KEY_ESCAPE);
						m_keymap.emplace(input::Keys::F1, GLFW_KEY_F1);
						m_keymap.emplace(input::Keys::F2, GLFW_KEY_F2);
						m_keymap.emplace(input::Keys::F3, GLFW_KEY_F3);
						m_keymap.emplace(input::Keys::F4, GLFW_KEY_F4);
						m_keymap.emplace(input::Keys::F5, GLFW_KEY_F5);
						m_keymap.emplace(input::Keys::F6, GLFW_KEY_F6);
						m_keymap.emplace(input::Keys::F7, GLFW_KEY_F7);
						m_keymap.emplace(input::Keys::F8, GLFW_KEY_F8);
						m_keymap.emplace(input::Keys::F9, GLFW_KEY_F9);
						m_keymap.emplace(input::Keys::F10, GLFW_KEY_F10);
						m_keymap.emplace(input::Keys::F11, GLFW_KEY_F11);
						m_keymap.emplace(input::Keys::F12, GLFW_KEY_F12);
						m_keymap.emplace(input::Keys::PRINTSCREEN, GLFW_KEY_PRINT_SCREEN);
						m_keymap.emplace(input::Keys::SCROLL_LOCK, GLFW_KEY_SCROLL_LOCK);
						m_keymap.emplace(input::Keys::PAUSE, GLFW_KEY_PAUSE);
						m_keymap.emplace(input::Keys::INSERT, GLFW_KEY_INSERT);
						m_keymap.emplace(input::Keys::HOME, GLFW_KEY_HOME);
						m_keymap.emplace(input::Keys::PAGEUP, GLFW_KEY_PAGE_UP);
						m_keymap.emplace(input::Keys::PAGEDOWN, GLFW_KEY_PAGE_DOWN);
						m_keymap.emplace(input::Keys::END, GLFW_KEY_END);
						m_keymap.emplace(input::Keys::DEL, GLFW_KEY_DELETE);
						m_keymap.emplace(input::Keys::UP, GLFW_KEY_UP);
						m_keymap.emplace(input::Keys::DOWN, GLFW_KEY_DOWN);
						m_keymap.emplace(input::Keys::LEFT, GLFW_KEY_LEFT);
						m_keymap.emplace(input::Keys::RIGHT, GLFW_KEY_RIGHT);
						m_keymap.emplace(input::Keys::NUMLOCK, GLFW_KEY_NUM_LOCK);
						m_keymap.emplace(input::Keys::NUMPAD_DIVIDE, GLFW_KEY_KP_DIVIDE);
						m_keymap.emplace(input::Keys::NUMPAD_MULTIPLY, GLFW_KEY_KP_MULTIPLY);
						m_keymap.emplace(input::Keys::NUMPAD_ADD, GLFW_KEY_KP_ADD);
						m_keymap.emplace(input::Keys::NUMPAD_ENTER, GLFW_KEY_KP_ENTER);
						m_keymap.emplace(input::Keys::NUMPAD_PERIOD, GLFW_KEY_KP_DECIMAL);
						m_keymap.emplace(input::Keys::NUMPAD_0, GLFW_KEY_KP_0);
						m_keymap.emplace(input::Keys::NUMPAD_1, GLFW_KEY_KP_1);
						m_keymap.emplace(input::Keys::NUMPAD_2, GLFW_KEY_KP_2);
						m_keymap.emplace(input::Keys::NUMPAD_3, GLFW_KEY_KP_3);
						m_keymap.emplace(input::Keys::NUMPAD_4, GLFW_KEY_KP_4);
						m_keymap.emplace(input::Keys::NUMPAD_5, GLFW_KEY_KP_5);
						m_keymap.emplace(input::Keys::NUMPAD_6, GLFW_KEY_KP_6);
						m_keymap.emplace(input::Keys::NUMPAD_7, GLFW_KEY_KP_7);
						m_keymap.emplace(input::Keys::NUMPAD_8, GLFW_KEY_KP_8);
						m_keymap.emplace(input::Keys::NUMPAD_9, GLFW_KEY_KP_9);

						m_prev_key_states.reserve(102);
						m_prev_key_states.emplace(input::Keys::A, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::B, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::C, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::D, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::E, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::G, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::H, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::I, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::J, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::K, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::L, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::M, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::N, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::O, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::P, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::Q, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::R, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::S, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::T, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::U, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::V, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::W, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::X, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::Y, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::Z, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_1, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_2, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_3, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_4, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_5, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_6, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_7, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_8, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_9, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUM_0, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::MINUS, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::EQUALS, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::BACKSPACE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::GRAVE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::TAB, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::CAPS, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LSHIFT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LCNTRL, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LSTART, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LALT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::SPACE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RALT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RSTART, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::MENU, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RCNTRL, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RSHIFT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::ENTER, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::SEMICOLON, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::APOSTROPHE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::SLASH, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::PERIOD, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::COMMA, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LBRACKET, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RBRACKET, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::BACKSLASH, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::ESC, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F1, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F2, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F3, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F4, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F5, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F6, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F7, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F8, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F9, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F10, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F11, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::F12, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::PRINTSCREEN, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::SCROLL_LOCK, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::PAUSE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::INSERT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::HOME, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::PAGEUP, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::PAGEDOWN, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::END, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::DEL, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::UP, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::DOWN, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::LEFT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::RIGHT, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMLOCK, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_MULTIPLY, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_DIVIDE, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_ADD, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_ENTER, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_PERIOD, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_0, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_1, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_2, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_3, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_4, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_5, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_6, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_7, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_8, GLFW_RELEASE);
						m_prev_key_states.emplace(input::Keys::NUMPAD_9, GLFW_RELEASE);

						// Setup framebuffer.
						m_framebuffer = std::make_unique<RenderTexture>();
						m_framebuffer->create(m_width, m_height);

						m_fb_sprite = std::make_unique<Sprite>();
						m_fb_sprite->load(m_framebuffer->gl_texture(), m_width, m_height);
						m_fb_sprite->create<BufferStatic>();
					}
				}
			}

			return result;
		}

		void Window::set_window_background(graphics::Colour& col)
		{
			m_colour = col.get_normalized();
		}

		void Window::set_icon(std::string_view icon)
		{
			stbi_set_flip_vertically_on_load(true);

			// Always convert to proper path before loading.
			auto path = std::filesystem::path {icon};

			// Fill glfw-compatible struct.
			GLFWimage img;
			img.pixels = stbi_load(path.string().c_str(), &img.width, &img.height, nullptr, STBI_rgb_alpha);
			if (!img.pixels)
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to load image: {0}.", path.string());
			}
			else
			{
				// Copies data so safe to destroy.
				glfwSetWindowIcon(m_window, 1, &img);
			}

			stbi_image_free(img.pixels);
		}

		void Window::set_icon(std::span<unsigned char> buffer)
		{
			stbi_set_flip_vertically_on_load(true);

			// Fill glfw-compatible struct.
			GLFWimage img;
			img.pixels = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size_bytes()), &img.width, &img.height, nullptr, STBI_rgb_alpha);

			if (!img.pixels)
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to load image for window icon from memory.");
			}
			else
			{
				// Copies data so safe to destroy.
				glfwSetWindowIcon(m_window, 1, &img);
			}

			stbi_image_free(img.pixels);
		}

		void Window::set_cursor_visibility(const bool visible)
		{
			visible == true ? glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL)
					: glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		void Window::remove_cursor()
		{
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		void Window::set_cursor_icon(std::string_view icon)
		{
			stbi_set_flip_vertically_on_load(false);

			// Always convert to proper path before loading.
			auto path = std::filesystem::path {icon};

			// Fill glfw-compatible struct.
			GLFWimage img;
			img.pixels = stbi_load(path.string().c_str(), &img.width, &img.height, nullptr, STBI_rgb_alpha);

			if (!img.pixels)
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to load image: {0}.", path.string());
			}
			else
			{
				// Copies data so safe to destroy.
				m_cursor.m_glfw = glfwCreateCursor(&img, 0, 0);
				glfwSetCursor(m_window, m_cursor.m_glfw);
			}

			stbi_image_free(img.pixels);
		}

		void Window::set_cursor_icon(std::span<unsigned char> buffer)
		{
			stbi_set_flip_vertically_on_load(false);

			// Fill glfw-compatible struct.
			GLFWimage img;
			img.pixels = stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size_bytes()), &img.width, &img.height, nullptr, STBI_rgb_alpha);

			if (!img.pixels)
			{
				GALAXY_LOG(GALAXY_ERROR, "Failed to load image for cursor icon from memory.");
			}
			else
			{
				// Copies data so safe to destroy.
				m_cursor.m_glfw = glfwCreateCursor(&img, 0, 0);
				glfwSetCursor(m_window, m_cursor.m_glfw);
			}

			stbi_image_free(img.pixels);
		}

		void Window::set_scrolling_callback(GLFWscrollfun func)
		{
			glfwSetScrollCallback(m_window, func);
		}

		void Window::destroy()
		{
			end_text_input();

			// Clean up window data, checking to make sure its not already been destroyed.
			if (m_window != nullptr)
			{
				m_fb_sprite.reset();
				m_framebuffer.reset();
				glfwDestroyWindow(m_window);
				m_window = nullptr;
			}

			// Cursor
			if (m_cursor.m_glfw != nullptr)
			{
				glfwDestroyCursor(m_cursor.m_glfw);
			}

			glfwTerminate();
		}

		bool Window::is_open() const
		{
			return (!glfwWindowShouldClose(m_window));
		}

		void Window::close()
		{
			glfwSetWindowShouldClose(m_window, true);
		}

		void Window::resize(const int width, const int height)
		{
			m_width  = width;
			m_height = height;

			m_framebuffer->change_size(width, height);
			m_fb_sprite->load(m_framebuffer->gl_texture(), m_width, m_height);
			m_fb_sprite->create<BufferStatic>();

			glfwSetWindowSize(m_window, width, height);
		}

		void Window::request_attention()
		{
			glfwRequestWindowAttention(m_window);
		}

		void Window::begin()
		{
			m_framebuffer->bind();
		}

		void Window::end(Renderer* renderer)
		{
			// clang-format off
			m_framebuffer->unbind();

			glViewport(0, 0, m_width, m_height);
			glClearColor(m_colour[0], m_colour[1], m_colour[2], m_colour[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
			if (renderer->get_post_effects().empty())
			{
				GALAXY_LOG(GALAXY_FATAL, "Failed to set a post effect. At least 1 required.");
			}
#endif
			// clang-format on

			m_fb_sprite->bind();

			for (PostEffect* effect : renderer->get_post_effects())
			{
				effect->m_shader.bind();
				effect->m_shader.set_uniform("u_projection", m_framebuffer->get_proj());
				effect->m_shader.set_uniform("u_transform", m_fb_sprite->get_transform());
				effect->m_shader.set_uniform("u_width", static_cast<float>(m_fb_sprite->get_width()));
				effect->m_shader.set_uniform("u_height", static_cast<float>(m_fb_sprite->get_height()));
				effect->apply_uniforms();

				glDrawElements(GL_TRIANGLES, m_fb_sprite->index_count(), GL_UNSIGNED_INT, nullptr);
			}

			m_fb_sprite->unbind();
			glfwSwapBuffers(m_window);
		}

		void Window::poll_events()
		{
			glfwPollEvents();
		}

		bool Window::mouse_button_pressed(input::MouseButton mouse_button)
		{
			bool res = false;

			int state = glfwGetMouseButton(m_window, m_mousebutton_map[mouse_button]);
			if (state == GLFW_PRESS && m_prev_mouse_btn_states[m_mousebutton_map[mouse_button]] == GLFW_RELEASE)
			{
				res = true;
			}

			m_prev_mouse_btn_states[m_mousebutton_map[mouse_button]] = state;

			return res;
		}

		bool Window::mouse_button_released(input::MouseButton mouse_button)
		{
			bool res = false;

			int state = glfwGetMouseButton(m_window, m_mousebutton_map[mouse_button]);
			if (state == GLFW_RELEASE && m_prev_mouse_btn_states[m_mousebutton_map[mouse_button]] == GLFW_PRESS)
			{
				res = true;
			}

			m_prev_mouse_btn_states[m_mousebutton_map[mouse_button]] = state;

			return res;
		}

		bool Window::key_down(input::Keys key)
		{
			return glfwGetKey(m_window, m_keymap[key]) == GLFW_PRESS;
		}

		bool Window::key_pressed(input::Keys key)
		{
			bool res = false;

			int state = glfwGetKey(m_window, m_keymap[key]);
			if (m_prev_key_states[key] == GLFW_RELEASE && state == GLFW_PRESS)
			{
				res = true;
			}

			m_prev_key_states[key] = state;
			return res;
		}

		std::string* Window::begin_text_input()
		{
			m_text_input     = "";
			m_inputting_text = true;

			return &m_text_input;
		}

		void Window::end_text_input()
		{
			m_inputting_text = false;
		}

		std::tuple<bool, glm::vec2> Window::get_cursor_pos()
		{
			glfwGetCursorPos(m_window, &m_cursor.m_pos.x, &m_cursor.m_pos.y);

			if (m_cursor.m_pos != m_cursor.m_prev_pos)
			{
				m_cursor.m_prev_pos = m_cursor.m_pos;
				return std::make_tuple(true, m_cursor.m_pos);
			}

			return std::make_tuple(false, m_cursor.m_pos);
		}

		GLFWwindow* Window::gl_window()
		{
			return m_window;
		}

		const int Window::get_width() const
		{
			return m_width;
		}

		const int Window::get_height() const
		{
			return m_height;
		}
	} // namespace graphics
} // namespace galaxy