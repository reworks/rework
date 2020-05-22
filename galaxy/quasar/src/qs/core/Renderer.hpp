///
/// Renderer.hpp
/// quasar
///
/// Refer to LICENSE.txt for more details.
///

#ifndef QUASAR_RENDERER_HPP_
#define QUASAR_RENDERER_HPP_

///
/// Core namespace.
///
namespace qs
{
	///
	/// Forward decs.
	///
	class Line;
	class Point;
	class Shader;
	class Circle;
	class Sprite;
	class AnimatedSprite;
	class SpriteBatch;
	class RenderTexture;

	///
	/// \brief OpenGL 2D renderer for drawing VA with transforms, shaders and textures.
	///
	/// ONLY THE RENDERER IS ALLOWED TO UNBIND/BIND SHADERS.
	///
	class Renderer final
	{
	public:
		///
		/// Default constructor.
		///
		Renderer() noexcept = default;

		///
		/// Default destructor.
		///
		~Renderer() noexcept = default;

		///
		/// \brief Draw a point.
		///
		/// Must have uniform(s): 
		/// u_cameraProj, u_cameraView, u_point_size, u_colour
		///
		/// \param point Point to draw.
		/// \param shader Shader to apply when drawing.
		///
		void drawPoint(qs::Point& point, qs::Shader& shader) noexcept;

		///
		/// \brief Draw a line.
		///
		/// Must have uniform(s): 
		/// u_cameraProj, u_cameraView, u_point_size, u_colour
		///
		/// \param line Line to draw.
		///
		void drawLine(qs::Line& line) noexcept;
		
		///
		/// \brief Draw a circle.
		///
		/// Must have uniform(s):
		/// u_cameraProj, u_cameraView
		///
		/// \param circle Circle to draw.
		///
		void drawCircle(qs::Circle& circle) noexcept;

		///
		/// \brief Draw a sprite.
		///
		/// Must have uniform(s):
		/// u_transform, u_opacity, u_width, u_height, u_cameraProj, u_cameraView
		///
		/// \param sprite Sprite to draw to screen.
		/// \param shader Shader to apply to sprite. You must have called bind() already!
		///
		void drawSprite(qs::Sprite& sprite, qs::Shader& shader) noexcept;

		///
		/// \brief Draw an animated sprite.
		///
		/// Must have uniform(s):
		/// u_transform, u_opacity, u_width, u_height, u_cameraProj, u_cameraView
		///
		/// \param sprite Sprite to draw to screen.
		/// \param shader Shader to apply to sprite. You must have called bind() already!
		///
		void drawAnimatedSprite(qs::AnimatedSprite& sprite, qs::Shader& shader) noexcept;

		///
		/// \brief Draw a spritebatch.
		///
		/// Must have uniform(s):
		/// u_width, u_height, u_cameraProj, u_cameraView
		///
		/// \param spritebatch SpriteBatch to draw to screen.
		/// \param shader Shader to apply to sprite. You must have called bind() already!
		///
		void drawSpriteBatch(qs::SpriteBatch& spritebatch, qs::Shader& shader) noexcept;

		///
		/// \brief Draw a sprite to a texture.
		///
		/// Must have uniform(s):
		/// u_transform, u_opacity, u_width, u_height, u_projection
		///
		/// \param sprite Sprite to draw. Pointer so you can cast up other sprite type objects.
		/// \param target Target to draw to.
		/// \param shader Shader to apply to sprite. You must have called bind() already!
		///
		void drawSpriteToTexture(qs::Sprite* sprite, qs::RenderTexture& target, qs::Shader& shader) noexcept;
		
		///
		/// Draw to render texture.
		///
		/// \param sprite Sprite to draw to screen.
		/// \param rt Target to draw to.
		/// \param shader Shader to apply to sprite. CALLS bind() FOR YOU!
		///
		///
		/// Draw VertexArray to render texture.
		///
		/// \param character (glyph) to draw.
		/// \param rt Target to draw to.
		/// \param shader Shader to apply to va.  CALLS bind() FOR YOU!
		///
		///
		/// Draw text to screen.
		///
		/// \param text Text to draw.
		/// \param shader Shader to apply to text. You must have called bind() already!
		///
		///
		/// \brief Draw a scene.
		///
		/// A scene is where you render all the sprites to a texture then apply a post process shader.
		/// Usually is a lightsource shader.
		/// Shader is bound for you.
		/// LightSource provides shader.
		///
		/// \param spritebatch Framebuffer sprite.
		/// \param camera used to render scene.
		/// \param ls Light source(s) for lighting. Provides its own shader.
		///
	};
}

#endif