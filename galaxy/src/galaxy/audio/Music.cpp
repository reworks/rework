///
/// Music.cpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#include <nlohmann/json.hpp>

#include "galaxy/error/Log.hpp"

#include "Music.hpp"

namespace galaxy
{
	namespace audio
	{
		Music::Music() noexcept
		    : Serializable {this}, m_looping {false}, m_running {false}
		{
		}

		Music::Music(const nlohmann::json& json)
		    : Serializable {this}, m_looping {false}, m_running {false}
		{
			deserialize(json);
		}

		Music::~Music()
		{
			destroy();
		}

		void Music::play()
		{
			alSourcePlay(m_source.handle());
		}

		void Music::pause()
		{
			alSourcePause(m_source.handle());
		}

		void Music::stop()
		{
			alSourceStop(m_source.handle());

			alSourceUnqueueBuffers(m_source.handle(), 2, m_buffers.data());

			stb_vorbis_seek_start(m_stream);

			stb_vorbis_get_samples_short_interleaved(m_stream, m_info.channels, m_data, CHUNK);
			alBufferData(m_buffers[0], m_format, m_data, CHUNK * sizeof(short), m_info.sample_rate);

			stb_vorbis_get_samples_short_interleaved(m_stream, m_info.channels, m_data, CHUNK);
			alBufferData(m_buffers[1], m_format, m_data, CHUNK * sizeof(short), m_info.sample_rate);

			m_source.queue(this);
		}

		const bool Music::load(std::string_view file)
		{
			destroy();

			const auto res = internal_load(file);
			if (res)
			{
				set_max_distance(1000.0f);

				m_source.queue(this);
				m_running = true;

				// clang-format off
				m_thread = std::jthread([this]()
				{
					while (this->m_running)
					{
						this->update();
					}
				});
				// clang-format on
			}

			return res;
		}

		void Music::set_looping(const bool looping)
		{
			m_looping = looping;
		}

		const bool Music::get_looping()
		{
			return m_looping;
		}

		nlohmann::json Music::serialize()
		{
			nlohmann::json json    = "{}"_json;
			json["file"]           = m_filename;
			json["looping"]        = get_looping();
			json["pitch"]          = get_pitch();
			json["gain"]           = get_gain();
			json["rolloff-factor"] = get_rolloff_factor();
			json["max-distance"]   = get_max_distance();

			glm::vec3 cone              = get_cone();
			json["cone"]["outer-gain"]  = cone.x;
			json["cone"]["inner-gain"]  = cone.y;
			json["cone"]["inner-angle"] = cone.z;

			glm::vec3 pos    = get_position();
			json["pos"]["x"] = pos.x;
			json["pos"]["y"] = pos.y;
			json["pos"]["z"] = pos.z;

			glm::vec3 vel    = get_velocity();
			json["vel"]["x"] = vel.x;
			json["vel"]["y"] = vel.y;
			json["vel"]["z"] = vel.z;

			glm::vec3 dir    = get_direction();
			json["dir"]["x"] = dir.x;
			json["dir"]["y"] = dir.y;
			json["dir"]["z"] = dir.z;

			bool is_playing = false;
			if (get_state() == AL_PLAYING)
			{
				is_playing = true;
			}
			json["is-playing"] = is_playing;

			return json;
		}

		void Music::deserialize(const nlohmann::json& json)
		{
			if (load(json.at("file")))
			{
				set_looping(json.at("looping"));
				set_pitch(json.at("pitch"));
				set_gain(json.at("gain"));
				set_rolloff_factor(json.at("rolloff-factor"));
				set_max_distance(json.at("max-distance"));

				const auto& cone_json = json.at("cone");
				set_cone(cone_json.at("outer-gain"), cone_json.at("inner-gain"), cone_json.at("inner-angle"));

				const auto& pos_json = json.at("pos");
				glm::vec3 pos        = {pos_json.at("x"), pos_json.at("y"), pos_json.at("z")};
				set_position(pos);

				const auto& vel_json = json.at("vel");
				glm::vec3 vel        = {vel_json.at("x"), vel_json.at("y"), vel_json.at("z")};
				set_velocity(vel);

				const auto& dir_json = json.at("dir");
				glm::vec3 dir        = {dir_json.at("x"), dir_json.at("y"), dir_json.at("z")};
				set_direction(dir);

				const bool is_playing = json.at("is-playing");
				if (is_playing)
				{
					play();
				}
			}
			else
			{
				GALAXY_LOG(GALAXY_ERROR, "Unable to load music: {0}.", std::string {json.at("file")});
			}
		}

		void Music::update()
		{
			int processed = 0;
			int amount    = 0;
			ALuint which  = 0;

			while (get_state() == AL_PLAYING)
			{
				alGetSourcei(m_source.handle(), AL_BUFFERS_PROCESSED, &processed);
				if (processed)
				{
					alSourceUnqueueBuffers(m_source.handle(), 1, &which);
					amount = stb_vorbis_get_samples_short_interleaved(m_stream, m_info.channels, m_data, CHUNK);
					if (amount == 0 && m_looping)
					{
						stb_vorbis_seek_start(m_stream);
						amount = stb_vorbis_get_samples_short_interleaved(m_stream, m_info.channels, m_data, CHUNK);
					}

					if (amount > 0)
					{
						alBufferData(which, m_format, m_data, amount * 2 * sizeof(short), m_info.sample_rate);
						alSourceQueueBuffers(m_source.handle(), 1, &which);
					}
				}
			}
		}

		void Music::destroy()
		{
			m_running = false;
			m_looping = false;

			if (m_thread.joinable())
			{
				m_thread.request_stop();
				m_thread.join();
			}

			alSourceStop(m_source.handle());
			alSourceUnqueueBuffers(m_source.handle(), 2, m_buffers.data());
		}
	} // namespace audio
} // namespace galaxy