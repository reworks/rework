///
/// Keys.hpp
/// starlight
///
/// Created by reworks on 20/01/2018.
/// MIT License.
/// Refer to LICENSE.txt for more details.
///

#ifndef STARLIGHT_KEYS_HPP_
#define STARLIGHT_KEYS_HPP_

namespace sl
{
	///
	/// Use this data structure to configure and check keybinds.
	/// You need to assign values to these keys, otherwise they all default to 0.
	///
	struct Keys
	{
		static inline int KEY_FORWARD = 0;
		static inline int KEY_BACKWARD = 0;
		static inline int KEY_LEFT = 0;
		static inline int KEY_RIGHT = 0;
		static inline int KEY_QUIT = 0;

	};
}

#endif