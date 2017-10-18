//
//  Music.cpp
//  rework
//
//  Created by reworks on 13/10/2017.
//  Copyright (c) 2017 reworks. All rights reserved.
//

#include <boost/filesystem.hpp>

#include "re/services/VFS.hpp"
#include "re/services/ServiceLocator.hpp"

#include "Music.hpp"

namespace re
{
	Music::Music(sol::table& table)
	{
		std::string fn = table.get<std::string>("file");
		std::string ext = boost::filesystem::extension(fn);

		m_music = al_load_sample_f(Locator::get<VFS>()->open(fn, "r"), ext.c_str());
		m_instance = al_create_sample_instance(m_music);

		// 0 - play once
		// 1 - loop
		// 2 - 
		int playmode = table.get<int>("playmode");
		switch (playmode)
		{
			case 0:
				al_set_sample_instance_playmode(m_instance, ALLEGRO_PLAYMODE_ONCE);
				break;

			case 1:
				al_set_sample_instance_playmode(m_instance, ALLEGRO_PLAYMODE_LOOP);
				break;

			case 2:
				al_set_sample_instance_playmode(m_instance, ALLEGRO_PLAYMODE_BIDIR);
				break;

			default:
				BOOST_LOG_TRIVIAL(warning) << "Invalid playmode. Setting to loop..." << std::endl;
				al_set_sample_instance_playmode(m_instance, ALLEGRO_PLAYMODE_LOOP);
				break;
		}
		
		al_set_sample_instance_pan(m_instance, table.get<float>("pan"));
		al_set_sample_instance_speed(m_instance, table.get<float>("speed"));
		al_set_sample_instance_gain(m_instance, table.get<float>("volume"));

		al_attach_sample_instance_to_mixer(m_instance, al_get_default_mixer());
	}

	Music::~Music()
	{
		al_destroy_sample_instance(m_instance);
		al_destroy_sample(m_music);
	}

	void Music::play()
	{
		al_set_sample_instance_position(m_instance, 0);
		al_play_sample_instance(m_instance);
	}

	void Music::stop()
	{
		m_position = al_get_sample_instance_position(m_instance);
		al_stop_sample_instance(m_instance);
	}

	void Music::resume()
	{
		al_set_sample_instance_position(m_instance, m_position);
		al_play_sample_instance(m_instance);
	}

	void Music::setPan(float pan)
	{
		al_set_sample_instance_pan(m_instance, pan);
	}

	void Music::setSpeed(float speed)
	{
		al_set_sample_instance_speed(m_instance, speed);
	}

	void Music::setVolume(float volume)
	{
		al_set_sample_instance_gain(m_instance, volume);
	}
}