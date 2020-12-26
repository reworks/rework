///
/// Config.cpp
/// galaxy
///
/// Refer to LICENSE.txt for more details.
///

#include <fstream>

#include "Config.hpp"

namespace galaxy
{
	namespace fs
	{
		Config::Config()
		    : m_opened {false}, m_exists {false}
		{
		}

		void Config::init(std::string_view file)
		{
			m_path = {file};
			if (!std::filesystem::exists(m_path))
			{
				m_exists = false;
			}
			else
			{
				m_exists = true;
			}
		}

		bool Config::open()
		{
			bool result = true;

			if (m_exists)
			{
				// Makes sure the filepath is correct for the current platform.
				std::fstream ifs;
				ifs.open(m_path.string(), std::fstream::in);
				if (!ifs.good())
				{
					result = false;
					GALAXY_LOG(GALAXY_ERROR, "std::ifstream failed to open config file!");
				}
				else
				{
					// Use JSON stream to deserialize data and parse.
					ifs >> m_config;
					m_opened = true;
				}

				ifs.close();
			}
			else
			{
				result = false;
				GALAXY_LOG(GALAXY_WARNING, "Config file must be init() and define(), then created.");
			}

			return result;
		}

		void Config::create()
		{
			if (m_config.empty())
			{
				GALAXY_LOG(GALAXY_FATAL, "Attempted to create empty config file!");
			}
			else
			{
				save();
				m_exists = true;
			}
		}

		void Config::save()
		{
			std::ofstream ofs;
			ofs.open(m_path.string(), std::ofstream::out | std::ofstream::trunc);
			if (!ofs.good())
			{
				GALAXY_LOG(GALAXY_FATAL, "std::ofstream failed to open config file!");
			}

			// Use JSON stream to serialize data and write to file.
			ofs << m_config.dump(4);
			m_opened = true;

			ofs.close();
		}
	} // namespace fs
} // namespace galaxy