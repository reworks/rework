///
/// TextureAtlas.cpp
/// quasar
///
/// Refer to LICENSE.txt for more details.
///

#include <pulsar/Log.hpp>

#include "qs/core/Shader.hpp"
#include "qs/core/Renderer.hpp"
#include "qs/sprite/Sprite.hpp"

#include "TextureAtlas.hpp"

namespace qs
{
	TextureAtlas::TextureAtlas()
	    : m_size {1024}, m_texture {1024, 1024}
	{
		// This is the default size.
		m_packer.init(1024, 1024);
	}

	TextureAtlas::TextureAtlas(const unsigned int size)
	    : m_size {size}, m_texture {size, size}
	{
		if (size == 0)
		{
			throw std::runtime_error("TextureAtlas size cannot be 0.");
		}

		if (!((size & (size - 1)) == 0))
		{
			throw std::runtime_error("TextureAtlas size is not power of 2.");
		}

		m_packer.init(size, size);
	}

	TextureAtlas::~TextureAtlas()
	{
		m_textures.clear();
	}

	void TextureAtlas::add(std::string_view file)
	{
		const auto path = std::filesystem::path {file};
		if (!m_textures.contains(path.stem().string()))
		{
			m_textures[path.stem().string()] = {.m_path = std::move(path)};
		}
		else
		{
			PL_LOG(PL_WARNING, "Attempted to add pre-existing texture.");
		}
	}

	void TextureAtlas::create(qs::Renderer& renderer, qs::Shader& shader)
	{
		if (!m_textures.empty())
		{
			m_texture.bind();

			for (auto& [name, info] : m_textures)
			{
				// Load texture.
				qs::Sprite to_draw;
				to_draw.load(info.m_path.string());
				to_draw.create<qs::BufferDynamic>();

				// Pack into rect then add to hashmap.
				auto opt = m_packer.pack(to_draw.get_width(), to_draw.get_height());
				if (opt == std::nullopt)
				{
					PL_LOG(PL_ERROR, "Failed to pack texture: {0}.", name);
				}
				else
				{
					auto rect = opt.value();
					to_draw.move(static_cast<float>(rect.m_x), static_cast<float>(rect.m_y));

					renderer.draw_sprite_to_texture(&to_draw, m_texture, shader);

					info.m_region = {static_cast<float>(rect.m_x), static_cast<float>(rect.m_y), static_cast<float>(rect.m_width), static_cast<float>(rect.m_height)};
				}
			}

			m_texture.unbind();
		}
		else
		{
			PL_LOG(PL_ERROR, "Tried to create atlas with no texture files!");
		}
	}

	void TextureAtlas::add_nine_slice(std::string_view name, NineSlice& slice)
	{
		const auto str = static_cast<std::string>(name);
		if (!m_textures.contains(str))
		{
			PL_LOG(PL_WARNING, "Attempted to add a nine-slice to a non-existant texture: {0}.", str);
		}
		else
		{
			m_textures[str].m_nineslice = std::move(slice);
		}
	}

	void TextureAtlas::save(std::string_view file)
	{
		m_texture.save(file);
	}

	std::optional<pr::Rect<float>> TextureAtlas::get_region(std::string_view name)
	{
		const auto str = static_cast<std::string>(name);
		if (!m_textures.contains(str))
		{
			PL_LOG(PL_WARNING, "Tried to access texture rect that does not exist.");
			return std::nullopt;
		}
		else
		{
			return std::make_optional(m_textures[str].m_region);
		}
	}

	qs::RenderTexture* TextureAtlas::get_atlas()
	{
		return &m_texture;
	}

	const int TextureAtlas::get_size() const
	{
		return m_size;
	}
} // namespace qs