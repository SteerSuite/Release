//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * Definitions.h
 * RVO2-3D Library
 *
 * Copyright (c) 2008-2011 University of North Carolina at Chapel Hill.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and non-profit purposes, without
 * fee, and without a written agreement is hereby granted, provided that the
 * above copyright notice, this paragraph, and the following four paragraphs
 * appear in all copies.
 *
 * Permission to incorporate this software into commercial products may be
 * obtained by contacting the authors <geom@cs.unc.edu> or the Office of
 * Technology Development at the University of North Carolina at Chapel Hill
 * <otd@unc.edu>.
 *
 * This software program and documentation are copyrighted by the University of
 * North Carolina at Chapel Hill. The software program and documentation are
 * supplied "as is," without any accompanying services from the University of
 * North Carolina at Chapel Hill or the authors. The University of North
 * Carolina at Chapel Hill and the authors do not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user understands
 * that the program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
 * AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
 * SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
 * CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
 * DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY
 * STATUTORY WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON
 * AN "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE
 * AUTHORS HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/RVO2/>
 */

/**
 * \file   Definitions.h
 * \brief  Contains functions and constants used in multiple classes.
 */

#ifndef RVO_DEFINITIONS_H_
#define RVO_DEFINITIONS_H_

#include "util/Geometry.h"
#include "Vector2.h"

const float RVO_EPSILON = 0.00001f;

/**
 * \brief      Computes the squared distance from a line segment with the
 *             specified endpoints to a specified point.
 * \param      a               The first endpoint of the line segment.
 * \param      b               The second endpoint of the line segment.
 * \param      c               The point to which the squared distance is to
 *                             be calculated.
 * \return     The squared distance from the line segment to the point.
 */

inline float distSqPointLineSegment(const Vector2 &a, const Vector2 &b,
									const Vector2 &c)
{
	const float r = ((c - a) * (b - a)) / absSq(b - a);

	if (r < 0.0f) {
		return absSq(c - a);
	}
	else if (r > 1.0f) {
		return absSq(c - b);
	}
	else {
		return absSq(c - (a + r * (b - a)));
	}
}

/**
 * \brief   Computes the square of a float.
 * \param   scalar  The float to be squared.
 * \return  The square of the float.
 */
inline float sqr(float scalar)
{
	return scalar * scalar;
}

/**
 * \relates    Vector2
 * \brief      Computes the determinant of a two-dimensional square matrix with
 *             rows consisting of the specified two-dimensional vectors.
 * \param      vector1         The top row of the two-dimensional square
 *                             matrix.
 * \param      vector2         The bottom row of the two-dimensional square
 *                             matrix.
 * \return     The determinant of the two-dimensional square matrix.
 */
inline float det(const Util::Vector &vector1, const Util::Vector &vector2)
{
	return vector1.x * vector2.z - vector1.z * vector2.x;
}


/**
 * \brief      Computes the signed distance from a line connecting the
 *             specified points to a specified point.
 * \param      a               The first point on the line.
 * \param      b               The second point on the line.
 * \param      c               The point to which the signed distance is to
 *                             be calculated.
 * \return     Positive when the point c lies to the left of the line ab.
 */
inline float leftOf(const Util::Point &a, const Util::Point &b, const Util::Point &c)
{
	return det(a - c, b - a);
}
inline float leftOf(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
	return det(a - c, b - a);
}


#endif /* RVO_DEFINITIONS_H_ */
