//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * KdTree.h
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
 * \file    KdTree.h
 * \brief   Contains the KdTree class.
 */
#ifndef _KD_TREE_H_
#define _KD_TREE_H_

// #include "API.h"
#include <algorithm>

#ifndef _WIN32
// win32 does not define "std::max", instead they define "max" as a macro.
// because of this, on unix we use "using std::max" so that the code only
// needs to use "max()" instead of "std::max()".  This way, the code
// works on both win32 and unix.
using std::max;
using std::min;
#endif

#include <cstddef>
#include <vector>

//#include "Vector3.h"
#include "SteerLib.h"
#include "interfaces/AgentInterface.h"
// #include "interfaces/ObstacleInterface.h"


// class RVO2Agent;
using namespace SteerLib;

/**
 * \brief   Defines <i>k</i>d-trees for agents in the simulation.
 */
class KdTree
{
public:
	/**
	 * \brief   Defines an agent <i>k</i>d-tree node.
	 */
	class AgentInterfaceTreeNode {
	public:
		/**
		 * \brief      The beginning node number.
		 */
		size_t begin;

		/**
		 * \brief      The ending node number.
		 */
		size_t end;

		/**
		 * \brief      The left node number.
		 */
		size_t left;

		/**
		 * \brief      The maximum x-coordinate.
		 */
		float maxX;

		/**
		 * \brief      The maximum y-coordinate.
		 */
		float maxY;

		/**
		 * \brief      The minimum x-coordinate.
		 */
		float minX;

		/**
		 * \brief      The minimum y-coordinate.
		 */
		float minY;

		/**
		 * \brief      The right node number.
		 */
		size_t right;
	};

	/**
	 * \brief      Defines an obstacle <i>k</i>d-tree node.
	 */
	class ObstacleInterfaceTreeNode {
	public:
		/**
		 * \brief      The left obstacle tree node.
		 */
		ObstacleInterfaceTreeNode *left;

		/**
		 * \brief      The obstacle number.
		 */
		const ObstacleInterface *obstacle;

		/**
		 * \brief      The right obstacle tree node.
		 */
		ObstacleInterfaceTreeNode *right;
	};


	/**
	 * \brief   Builds an agent <i>k</i>d-tree.
	 */
	void buildAgentTree();

	void buildAgentTreeRecursive(size_t begin, size_t end, size_t node);

	/**
	 * \brief      Builds an obstacle <i>k</i>d-tree.
	 */
	void buildObstacleTree();

	ObstacleInterfaceTreeNode *buildObstacleTreeRecursive(const std::vector<ObstacleInterface *> &
												 obstacles);

	/**
	 * \brief   Computes the agent neighbors of the specified agent.
	 * \param   agent    A pointer to the agent for which agent neighbors are to be computed.
	 * \param   rangeSq  The squared range around the agent.
	 */
	void computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const;

	/**
	 * \brief      Computes the obstacle neighbors of the specified agent.
	 * \param      agent           A pointer to the agent for which obstacle
	 *                             neighbors are to be computed.
	 * \param      rangeSq         The squared range around the agent.
	 */
	void computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const;

	/**
	 * \brief      Deletes the specified obstacle tree node.
	 * \param      node            A pointer to the obstacle tree node to be
	 *                             deleted.
	 */
	void deleteObstacleTree(ObstacleInterfaceTreeNode *node);


	void queryObstacleTreeRecursive(SteerLib::AgentInterface *agent, float rangeSq,
									const ObstacleInterfaceTreeNode *node) const;

	/**
	 * \brief      Queries the visibility between two points within a
	 *             specified radius.
	 * \param      q1              The first point between which visibility is
	 *                             to be tested.
	 * \param      q2              The second point between which visibility is
	 *                             to be tested.
	 * \param      radius          The radius within which visibility is to be
	 *                             tested.
	 * \return     True if q1 and q2 are mutually visible within the radius;
	 *             false otherwise.
	 */
	bool queryVisibility(const Util::Point &q1, const Util::Point &q2,
						 float radius) const;

	bool queryVisibilityRecursive(const Util::Point &q1, const Util::Point &q2,
								  float radius,
								  const ObstacleInterfaceTreeNode *node) const;

	void queryAgentTreeRecursive(SteerLib::AgentInterface *agent,
									float &rangeSq, size_t node) const;

	std::vector<SteerLib::AgentInterface *> agents_;
	std::vector<AgentInterfaceTreeNode> agentTree_;
	ObstacleInterfaceTreeNode *obstacleTree_;
	SteerLib::EngineInterface * sim_;

	void setSimulator(SteerLib::EngineInterface * sim_);

	friend class RVO2Agent;
	friend class RVO2AIModule;
	friend class RVOSimulator;
	friend class SpatialDataBaseInterface;


	static const size_t MAX_LEAF_SIZE = 10;
	/**
	 * \brief   Constructs a <i>k</i>d-tree instance.
	 * \param   sim  The simulator instance.
	 */
	KdTree();
	~KdTree();
};

#endif /* RVO_KD_TREE_H_ */
