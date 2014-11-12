//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_COLOR_H__
#define __UTIL_COLOR_H__

/// @file Color.h
/// @brief Defines a color class and several predefined static constant colors

#include <assert.h>
#include <iostream>
#include "Globals.h"

namespace Util {

	/**
	 * @brief An RGB color data type.
	 *
	 * Note that no implicit clamping is done when performing operations on Color objects.
	 *
	 */
	class UTIL_API Color {
	public:
		float r, g, b;
		/// Default constructor initializes to white.
		Color() : r(1.0f), g(1.0f), b(1.0f) { }
		/// Initializes to a user-defined color
		Color( float newr, float newg, float newb ) : r(newr), g(newg), b(newb) { }
		/// Adds two colors
		Color operator+( const Color & col ) const { return Color(r + col.r, g + col.g, b + col.b); }
		/// Subtracts two colors
		Color operator-( const Color & col ) const { return Color(r - col.r, g - col.g, b - col.b); }
		/// scales a color
		Color operator*( float c ) const { return Color(c*r, c*g, c*b); }
		Color operator/( float c ) const { assert(c!=0); float cInverse = 1.0f / c; return Color(r * cInverse, g * cInverse, b * cInverse); }
	};

	/// An additional float-color multiplication operator, so that c*col is the same as col*c.
	static inline Util::Color operator*( float f, const Util::Color & c) { return c*f; }

	/// The ostream << operator to output a Color data type
	static inline std::ostream &operator<<(std::ostream &out, const Color &col) { out << "(" << col.r << "," << col.g << "," << col.b << ")"; return out; }


	static const Color gBlack(0.0f, 0.0f, 0.0f);
	static const Color gWhite(1.0f, 1.0f, 1.0f);
	static const Color gRed(1.0f, 0.0f, 0.0f); 
	static const Color gGreen(0.0f, 1.0f, 0.0f);
	static const Color gBlue(0.0f, 0.0f, 1.0f);
	static const Color gYellow(1.0f, 1.0f, 0.0f);
	static const Color gCyan(0.0f, 1.0f, 1.0f);
	static const Color gMagenta(1.0f, 0.0f, 1.0f);
	static const Color gOrange(1.0f, 0.5f, 0.0f);
	static const Color gDarkRed(0.5f, 0.0f, 0.0f);
	static const Color gDarkGreen(0.0f, 0.5f, 0.0f);
	static const Color gDarkBlue(0.0f, 0.0f, 0.5f);
	static const Color gDarkYellow(0.5f, 0.5f, 0.0f);
	static const Color gDarkCyan(0.0f, 0.5f, 0.5f);
	static const Color gDarkMagenta(0.5f, 0.0f, 0.5f);
	static const Color gDarkOrange(0.5f, 0.25f, 0.0f);
	static const Color gGray10(0.1f,0.1f,0.1f);
	static const Color gGray20(0.2f,0.2f,0.2f);
	static const Color gGray30(0.3f,0.3f,0.3f);
	static const Color gGray40(0.4f,0.4f,0.4f);
	static const Color gGray50(0.5f,0.5f,0.5f);
	static const Color gGray60(0.6f,0.6f,0.6f);
	static const Color gGray70(0.7f,0.7f,0.7f);
	static const Color gGray80(0.8f,0.8f,0.8f);
	static const Color gGray90(0.9f,0.9f,0.9f);
	
} // end namespace Util

#endif
