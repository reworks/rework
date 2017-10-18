//
//  Music.hpp
//  rework
//
//  Created by reworks on 13/10/2017.
//  Copyright (c) 2017 reworks. All rights reserved.
//

#ifndef REWORK_MUSIC_HPP_
#define REWORK_MUSIC_HPP_

#include <allegro5/allegro_audio.h>

#include "sol2/sol.hpp"

namespace re
{
	class Music
	{
	public:
		///
		/// Constructor.
		///
		/// \param table sol::table containing data.
		///
		Music(sol::table& table);

		///
		/// Destructor.
		///
		~Music();

		///
		/// Play the song from the beginning.
		///
		void play();

		///
		/// Stops the song.
		///
		void stop();

		///
		/// Resumes a stopped song from the position where it stopped.
		///
		void resume();
		
		///
		/// Change the pan.
		///
		/// \param pan 0.0 is centred, -1.0 is left, 1.0 is right, or ALLEGRO_AUDIO_PAN_NONE.
		///
		void setPan(float pan);

		///
		/// Change the speed.
		///
		/// \param speed Relative speed at which the sample is played. 1.0 is normal. 0.5 is at 50% speed, etc.
		///
		void setSpeed(float speed);

		///
		/// Change the volume.
		///
		/// \param volume Relative volume (gain) at which the sample is played. 1.0 is 100%. 0.5 is 50%, etc.
		///
		void setVolume(float volume);

	private:
		unsigned int m_position;

		ALLEGRO_SAMPLE* m_music;
		ALLEGRO_SAMPLE_INSTANCE* m_instance;
	};
}

#endif