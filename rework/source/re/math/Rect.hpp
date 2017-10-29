//
//  Rect.hpp
//  rework
//
//  Created by reworks on 26/09/2017.
//  Copyright (c) 2017 reworks. All rights reserved.
//

#ifndef REWORK_RECT_HPP_
#define REWORK_RECT_HPP_

namespace re
{
	template<typename T1, typename T2 = T1>
	class Rect
	{
	public:
		///
		/// Default constructor. Values are initialized to 0.
		///
		Rect();

		///
		/// Value constructor.
		///
		/// \param _x X value.
		/// \param _y Y value.
		/// \param _w Width value.
		/// \param _h Height value.
		///
		Rect(T1 _x, T1 _y, T2 _width, T2 _height);

		///
		/// Does the rectangle contain the point (x, y).
		///
		/// \param x X coordinate.
		/// \param y Y coordinate.
		///
		/// \return true if contains the point.
		///
		bool contains(T1 _x, T1 _y);

		///
		/// Do the rectangles a and b overlap.
		///
		/// \param a Left rectangle.
		/// \param b Right rectangle.
		///
		/// \return Returns true if there is an overlap.
		///
		bool overlaps(const Rect<T1, T2>& a, const Rect<T1, T2>& b);

	public:
		T1 x;
		T1 y;
		T2 width;
		T2 height;

	private:
		///
		/// Private function to determine if value is in range.
		/// From: https://stackoverflow.com/a/306379
		///
		bool valueInRange(T1 value, T1 min, T1 max);
	};

	template<typename T1, typename T2 = T1>
	Rect<T1, T2>::Rect()
		:x(0), y(0), width(0), height(0)
	{
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2>::Rect(T1 _x, T1 _y, T2 _width, T2 _height)
		: x(_x), y(_y), width(_width), height(_height)
	{
	}

	template<typename T1, typename T2 = T1>
	bool Rect<T1, T2>::contains(T1 _x, T1 _y)
	{
		bool out = ((_x > x) && (_x < (x + width)) && (_y > y) && (_y < (y + height))) ? true : false;

		return out;
	}

	template<typename T1, typename T2 = T1>
	bool Rect<T1, T2>::overlaps(const Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		// Thanks: https://stackoverflow.com/a/306379

		bool xOverlap = valueInRange<T>(a.x, b.x, b.x + b.width) ||
			valueInRange<T>(b.x, a.x, a.x + a.width);

		bool yOverlap = valueInRange<T>(a.y, b.y, b.y + b.height) ||
			valueInRange<T>(b.y, a.y, a.y + a.height);

		return xOverlap && yOverlap;
	}

	template<typename T1, typename T2 = T1>
	bool Rect<T1, T2>::valueInRange(T1 value, T1 min, T1 max)
	{
		return (value >= min) && (value <= max);
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2> operator-(const Rect<T1, T2>& a)
	{
		return Rect<T>(-a.x, -a.y, -a.width, -a.height);
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2>& operator+=(Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		a.x += b.x;
		a.y += b.y;
		a.width += b.width;
		a.height += a.height;

		return a;
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2>& operator-=(Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		a.x -= b.x;
		a.y -= b.y;
		a.width -= b.width;
		a.height -= b.height;

		return a;
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2> operator+(const Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		return Rect<T>(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	template<typename T1, typename T2 = T1>
	Rect<T1, T2> operator-(const Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		return Rect<T>(a.x - b.x, a.y - b.y, a.width - b.width, a.height - b.height);
	}

	template<typename T1, typename T2 = T1>
	bool operator==(const Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.width == b.width) && (a.height == b.height);
	}

	template<typename T1, typename T2 = T1>
	bool operator!=(const Rect<T1, T2>& a, const Rect<T1, T2>& b)
	{
		return (a.x != b.x) || (a.y != b.y) || (a.width != b.width) || (a.height != b.height);
	}
}

#endif