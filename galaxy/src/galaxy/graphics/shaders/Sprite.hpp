///
/// Sprite.hpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#ifndef QUASAR_GRAPHICS_SHADERS_SPRITE_HPP_
#define QUASAR_GRAPHICS_SHADERS_SPRITE_HPP_

#include <string>

namespace galaxy
{
	namespace shaders
	{
		///
		/// Sprite vertex shader.
		///
		inline const std::string sprite_vert = R"(
			#version 450 core
			layout(location = 0) in vec2 l_pos;
			layout(location = 1) in vec2 l_texels;

			out vec2 io_texels;
			out float io_opacity;

			uniform mat4 u_cameraView;
			uniform mat4 u_cameraProj;
			uniform mat4 u_transform;
			uniform float u_width;
			uniform float u_height;
			uniform float u_opacity;

			void main()
			{
				mat4 camera = u_cameraProj * u_cameraView;
				gl_Position =  camera * u_transform * vec4(l_pos, 0.0, 1.0);

				io_texels = vec2(l_texels.x / u_width, 1.0 - (l_texels.y / u_height));
				io_opacity = u_opacity;
			}
		)";

		///
		/// Sprite fragment shader.
		///
		inline const std::string sprite_frag = R"(
			#version 450 core

			in vec2 io_texels;
			in float io_opacity;

			out vec4 io_frag_colour;

			uniform sampler2D u_texture;

			void main()
			{
				vec4 tex = texture(u_texture, io_texels);
				tex.a *= io_opacity;

				io_frag_colour = tex;
			}
		)";
	} // namespace shaders
} // namespace galaxy

#endif