//
//  Time.hpp
//  REngine3
//
//  Created by reworks on 8/07/2016.
//  Copyright (c) 2016 reworks. All rights reserved.
//

#ifndef RENGINE3_TIME_HPP_
#define RENGINE3_TIME_HPP_

#include "re/Types.hpp"

#include <chrono>

namespace re
{
	static inline uint64 TimeNano()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	static inline uint64 TimeMillis()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static inline std::string GetCurrentTimeAndDate()
	{
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string temp = std::ctime(&time);
		// Remove pesky newlines
		temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
		return temp;
	}
}

#endif