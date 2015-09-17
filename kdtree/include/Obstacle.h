//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * Obstacle.h
 * RVO2 Library
 *
 * Copyright (c) 2008-2010 University of North Carolina at Chapel Hill.
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

#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

/**
 * \file       Obstacle.h
 * \brief      Contains the Obstacle class.
 */

#include "Definitions.h"
#include "util/Geometry.h"
#include "interfaces/ObstacleInterface.h"

using namespace SteerLib;
	/**
	 * \brief      Defines static obstacles in the simulation.
	 */
class Obstacle : public SteerLib::ObstacleInterface
{
public:
	/**
	 * \brief      Constructs a static obstacle instance.
	 */
	Obstacle();
	~Obstacle();


	// ObstacleInterface functionality (not all virtual functions were overridden here)
	void draw(); // implementation in .cpp
	const Util::AxisAlignedBox & getBounds() { return _bounds; }
	virtual void setBounds(const Util::AxisAlignedBox & bounds) { _bounds = bounds; }

	/// @name The SpatialDatabaseItem interface
	/// @brief The BoxObstacle implementation of this interface represents a box that blocks line of sight if it is taller than 0.5 meter, and cannot be traversed.
	//@{
	virtual bool isAgent() { return false; }
	bool blocksLineOfSight() { return _blocksLineOfSight; }
	float getTraversalCost() { return _traversalCost; }
	virtual bool intersects(const Util::Ray &r, float &t);
	virtual bool overlaps(const Util::Point & p, float radius) { return Util::boxOverlapsCircle2D(_bounds.xmin, _bounds.xmax, _bounds.zmin, _bounds.zmax,p, radius); }
	virtual float computePenetration(const Util::Point & p, float radius);
	virtual std::pair<std::vector<Util::Point>,std::vector<size_t> > getStaticGeometry();
	virtual std::vector<Util::Point> get2DStaticGeometry()
	{
		std::cout << "get static geometry not implemented yet" << std::endl;
		std::vector<Util::Point> ps;
		return ps;
	}
	//@}

protected:
	Util::AxisAlignedBox _bounds;
	bool _blocksLineOfSight;
	float _traversalCost;


	friend class KdTree;
};

#endif /* _OBSTACLE_H_ */
