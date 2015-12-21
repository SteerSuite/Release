//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
/*
 * KdTree.cpp
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

#include "KdTree.h"
#include "Definitions.h"
#undef min
#undef max
#include <algorithm>
#include "Obstacle.h"


using namespace Util;

KdTree::KdTree(): obstacleTree_(NULL)
{

}

KdTree::~KdTree()
{
	deleteObstacleTree(obstacleTree_);
}

void KdTree::buildAgentTree()
{
	// agents_ =  (sim_->getAgents());
	agents_.clear();
	for(unsigned int i = agents_.size() ; i < sim_->getAgents().size(); i++)
	{// This is done because the number of active agents can change each frame

		// RVO2DAgent * agent__ = dynamic_cast<RVO2DAgent *>(sim_->getAgents().at(i));
		SteerLib::AgentInterface * agent__ = (sim_->getAgents().at(i));
		if (agent__->enabled())
		{
			agents_.push_back(agent__);
		}
		// std::cout << "Adding an agent to the list of agents:" << i << " id() " << agent__->id() << std::endl;
	}
	// std::cout << "There are this many agents in the simulation: " << sim_->getAgents().size() << std::endl;
	// std::cout << "There are this many agents: " << agents_.size() << std::endl;
	if (!agents_.empty()) {
		agentTree_.resize(2 * agents_.size() - 1);
		buildAgentTreeRecursive(0, agents_.size(), 0);
	}
	// std::cout << "agent Tree size build: " << agentTree_.size() << std::endl;
}
/*
void KdTree::buildAgentTree()
{
	agents_ =  (gEngine->getAgents());

	if (agents_.size() < sim_->agents_.size()) {
		for (size_t i = agents_.size(); i < sim_->agents_.size(); ++i) {
			agents_.push_back(sim_->agents_[i]);
		}

		agentTree_.resize(2 * agents_.size() - 1);
	}

	if (!agents_.empty()) {
		buildAgentTreeRecursive(0, agents_.size(), 0);
	}
}*/

void KdTree::setSimulator(SteerLib::EngineInterface *sim_)
{
	this->sim_ = sim_;
}

void KdTree::buildAgentTreeRecursive(size_t begin, size_t end, size_t node)
{
	agentTree_[node].begin = begin;
	agentTree_[node].end = end;
	// std::vector<RVO2DAgent *> * _agents = dynamic_cast<std::vector<RVO2DAgent *> *>(&agents_);
	agentTree_[node].minX = agentTree_[node].maxX = agents_[begin]->position().x;
	agentTree_[node].minY = agentTree_[node].maxY = agents_[begin]->position().z;

	for (size_t i = begin + 1; i < end; ++i) {
		agentTree_[node].maxX = std::max(agentTree_[node].maxX, agents_[i]->position().x);
		agentTree_[node].minX = std::min(agentTree_[node].minX, agents_[i]->position().x);
		agentTree_[node].maxY = std::max(agentTree_[node].maxY, agents_[i]->position().z);
		agentTree_[node].minY = std::min(agentTree_[node].minY, agents_[i]->position().z);
	}

	if (end - begin > MAX_LEAF_SIZE) {
		/* No leaf node. */
		const bool isVertical = (agentTree_[node].maxX - agentTree_[node].minX > agentTree_[node].maxY - agentTree_[node].minY);
		const float splitValue = (isVertical ? 0.5f * (agentTree_[node].maxX + agentTree_[node].minX) : 0.5f * (agentTree_[node].maxY + agentTree_[node].minY));

		size_t left = begin;
		size_t right = end;

		while (left < right) {
			while (left < right && (isVertical ? agents_[left]->position().x : agents_[left]->position().z) < splitValue) {
				++left;
			}

			while (right > left && (isVertical ? agents_[right - 1]->position().x : agents_[right - 1]->position().z) >= splitValue) {
				--right;
			}

			if (left < right) {
				std::swap(agents_[left], agents_[right - 1]);
				++left;
				--right;
			}
		}

		if (left == begin) {
			++left;
			++right;
		}

		agentTree_[node].left = node + 1;
		agentTree_[node].right = node + 2 * (left - begin);

		buildAgentTreeRecursive(begin, left, agentTree_[node].left);
		buildAgentTreeRecursive(left, end, agentTree_[node].right);
	}
}

void KdTree::buildObstacleTree()
{
	deleteObstacleTree(obstacleTree_);

	std::vector<ObstacleInterface *> obstacles_;
	// std::cout << "The number of obstacles in the scenario is:" << obstacles_.size() << std::endl;
	// for (size_t i = 0; i < sim_->getObstacles().size(); ++i)
	int i1 = 0;
	for(std::set<SteerLib::ObstacleInterface*>::const_iterator iter = sim_->getObstacles().begin(); iter != sim_->getObstacles().end(); iter++)
	{

		// convert SteerSuite Obstacle to RVO Obstacle
		AxisAlignedBox currObstacle = (*iter)->getBounds();

		std::vector<Util::Point> vertices;

		vertices = (*iter)->get2DStaticGeometry();
		/*
		 * Every verticy is considered an obstacle
		 * Still obstacles inside the list reference the actual points that make
		 * up the faces of the obstacle.
		 */

		const size_t obstacleNo = obstacles_.size();

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			ObstacleInterface *obstacle = new Obstacle();
			obstacle->setBounds(currObstacle);
			obstacle->point_ = vertices[i];

			if (i != 0)
			{
				obstacle->prevObstacle_ = obstacles_.back();
				obstacle->prevObstacle_->nextObstacle_ = obstacle;
			}

			if (i == vertices.size() - 1) {
				obstacle->nextObstacle_ = obstacles_[obstacleNo];
				obstacle->nextObstacle_->prevObstacle_ = obstacle;
			}

			obstacle->unitDir_ = normalize(vertices[(i == vertices.size() - 1 ? 0 : i + 1)] - vertices[i]);

			if (vertices.size() == 2) {
				obstacle->isConvex_ = true;
			}
			else {
				obstacle->isConvex_ = (leftOf(vertices[(i == 0 ? vertices.size() - 1 : i - 1)], vertices[i], vertices[(i == vertices.size() - 1 ? 0 : i + 1)]) >= 0.0f);
			}

			obstacle->id_ = obstacles_.size();

			obstacles_.push_back(obstacle);
			// std::cout << "Added an obstacle to the list of obstacles:" << obstacle->point_ << std::endl;
		}

		// Seems like for obstacles the x and z coordinates are backwards?
		/*
		obstacleTopLeft->point_ = Vector2(currObstacle.xmax, currObstacle.zmax);
		obstacleTopRight->point_ = Vector2(currObstacle.xmin, currObstacle.zmax);
		obstacleBottomLeft->point_ = Vector2(currObstacle.xmin, currObstacle.zmin);
		obstacleBottomRight->point_ = Vector2(currObstacle.xmax, currObstacle.zmin);

		obstacleTopLeft->nextObstacle_ = obstacleTopRight;
		obstacleTopLeft->prevObstacle_ = obstacleBottomLeft;
		obstacleTopLeft->unitDir_ = Vector2(1,0);
		obstacleTopLeft->id_ = obstacles_.size();
		obstacles_.push_back(obstacleTopLeft);

		obstacleTopRight->nextObstacle_ = obstacleBottomRight;
		obstacleTopRight->prevObstacle_ = obstacleTopLeft;
		obstacleTopRight->unitDir_ = Vector2(0,-1);
		obstacleTopRight->id_ = obstacles_.size();
		obstacles_.push_back(obstacleTopRight);

		obstacleBottomRight->nextObstacle_ = obstacleBottomLeft;
		obstacleBottomRight->prevObstacle_ = obstacleTopRight;
		obstacleBottomRight->unitDir_ = Vector2(-1,0);
		obstacleBottomRight->id_ = obstacles_.size();
		obstacles_.push_back(obstacleBottomRight);

		obstacleBottomLeft->nextObstacle_ = obstacleTopLeft;
		obstacleBottomLeft->prevObstacle_ = obstacleBottomRight;
		obstacleBottomLeft->unitDir_ = Vector2(0,-1);
		obstacleBottomLeft->id_ = obstacles_.size();
		obstacles_.push_back(obstacleBottomLeft);
		std::cout << "Added an obstacle to the list of obstacles:" << obstacles_.at(obstacles_.size() -1)->point_ << std::endl;
	*/
		// std
		// obstacles[i] = obstacleTopLeft;
		// std::cout << "Adding an obstacle to the list of obstacles:" << obstacles_[i]->point_ << std::endl;
		i1++;
	}
	// std::cout << "This many obstacles were created:" << obstacles_.size() << std::endl;
	obstacleTree_ = buildObstacleTreeRecursive(obstacles_);
}

KdTree::ObstacleInterfaceTreeNode *KdTree::buildObstacleTreeRecursive(const std::vector<ObstacleInterface *> &obstacles)
{
	if (obstacles.empty()) {
		return NULL;
	}
	else {
		ObstacleInterfaceTreeNode *const node = new ObstacleInterfaceTreeNode;

		size_t optimalSplit = 0;
		size_t minLeft = obstacles.size();
		size_t minRight = obstacles.size();

		for (size_t i = 0; i < obstacles.size(); ++i) {
			size_t leftSize = 0;
			size_t rightSize = 0;

			const ObstacleInterface *const obstacleI1 = obstacles[i];
			// std::cout << "So far i is: " << i << std::endl;
			const ObstacleInterface *const obstacleI2 = obstacleI1->nextObstacle_;

			/* Compute optimal split node. */
			for (size_t j = 0; j < obstacles.size(); ++j) {
				if (i == j) {
					continue;
				}

				const ObstacleInterface *const obstacleJ1 = obstacles[j];
				const ObstacleInterface *const obstacleJ2 = obstacleJ1->nextObstacle_;

				const float j1LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ1->point_);
				const float j2LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ2->point_);

				if (j1LeftOfI >= -RVO_EPSILON && j2LeftOfI >= -RVO_EPSILON) {
					++leftSize;
				}
				else if (j1LeftOfI <= RVO_EPSILON && j2LeftOfI <= RVO_EPSILON) {
					++rightSize;
				}
				else {
					++leftSize;
					++rightSize;
				}

				if (std::make_pair(std::max(leftSize, rightSize), std::min(leftSize, rightSize)) >= std::make_pair(std::max(minLeft, minRight), std::min(minLeft, minRight))) {
					break;
				}
			}

			if (std::make_pair(std::max(leftSize, rightSize), std::min(leftSize, rightSize)) < std::make_pair(std::max(minLeft, minRight), std::min(minLeft, minRight))) {
				minLeft = leftSize;
				minRight = rightSize;
				optimalSplit = i;
			}
		}

		/* Build split node. */
		std::vector<ObstacleInterface *> leftObstacles(minLeft);
		std::vector<ObstacleInterface *> rightObstacles(minRight);

		size_t leftCounter = 0;
		size_t rightCounter = 0;
		const size_t i = optimalSplit;

		const ObstacleInterface *const obstacleI1 = obstacles[i];
		const ObstacleInterface *const obstacleI2 = obstacleI1->nextObstacle_;

		for (size_t j = 0; j < obstacles.size(); ++j) {
			if (i == j) {
				continue;
			}

			ObstacleInterface *const obstacleJ1 = obstacles[j];
			ObstacleInterface *const obstacleJ2 = obstacleJ1->nextObstacle_;

			const float j1LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ1->point_);
			const float j2LeftOfI = leftOf(obstacleI1->point_, obstacleI2->point_, obstacleJ2->point_);

			if (j1LeftOfI >= -RVO_EPSILON && j2LeftOfI >= -RVO_EPSILON) {
				leftObstacles[leftCounter++] = obstacles[j];
			}
			else if (j1LeftOfI <= RVO_EPSILON && j2LeftOfI <= RVO_EPSILON) {
				rightObstacles[rightCounter++] = obstacles[j];
			}
			else {
				/* Split obstacle j. */
				const float t = det(obstacleI2->point_ - obstacleI1->point_, obstacleJ1->point_ - obstacleI1->point_) / det(obstacleI2->point_ - obstacleI1->point_, obstacleJ1->point_ - obstacleJ2->point_);

				const Util::Point splitpoint = obstacleJ1->point_ + t * (obstacleJ2->point_ - obstacleJ1->point_);

				ObstacleInterface *const newObstacle = new Obstacle();
				newObstacle->point_ = splitpoint;
				newObstacle->prevObstacle_ = obstacleJ1;
				newObstacle->nextObstacle_ = obstacleJ2;
				newObstacle->isConvex_ = true;
				newObstacle->unitDir_ = obstacleJ1->unitDir_;

				// newObstacle->id_ = sim_->obstacles_.size();
				// TODO Possible point of error
				// don't know why the agent is adding obstacles to the simulation
				//sim_->obstacles_.push_back(newObstacle);

				obstacleJ1->nextObstacle_ = newObstacle;
				obstacleJ2->prevObstacle_ = newObstacle;

				if (j1LeftOfI > 0.0f) {
					leftObstacles[leftCounter++] = obstacleJ1;
					rightObstacles[rightCounter++] = newObstacle;
				}
				else {
					rightObstacles[rightCounter++] = obstacleJ1;
					leftObstacles[leftCounter++] = newObstacle;
				}
			}
		}

		node->obstacle = obstacleI1;
		node->left = buildObstacleTreeRecursive(leftObstacles);
		node->right = buildObstacleTreeRecursive(rightObstacles);
		return node;
	}
}



void KdTree::computeAgentNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const
{
	queryAgentTreeRecursive(dynamic_cast<AgentInterface*>(agent), rangeSq, 0);
}

void KdTree::computeObstacleNeighbors(SpatialDatabaseItemPtr agent, float rangeSq) const
{
	dynamic_cast<AgentInterface*>(agent)->obstacleNeighbors_.clear();
	// std::cout << " computing agent neighbours " << std::endl;
	queryObstacleTreeRecursive(dynamic_cast<AgentInterface*>(agent), rangeSq, obstacleTree_);
	// std::cout << "found " << dynamic_cast<AgentInterface*>(agent)->obstacleNeighbors_.size() <<
		// 	" obstacle neighbours" << std::endl;
}

void KdTree::deleteObstacleTree(ObstacleInterfaceTreeNode *node)
{
	if (node != NULL)
	{
		deleteObstacleTree(node->left);
		deleteObstacleTree(node->right);
		delete node->obstacle;
		delete node;
	}
}

void KdTree::queryAgentTreeRecursive(SteerLib::AgentInterface  *agent, float &rangeSq, size_t node) const
{
	// std::cout << "agent tree size: " << agentTree_.size() << " number of agents " << agents_.size() << std::endl;

	if (agentTree_[node].end - agentTree_[node].begin <= MAX_LEAF_SIZE) {
		for (size_t i = agentTree_[node].begin; i < agentTree_[node].end && (i < agents_.size()); ++i) {
			agent->insertAgentNeighbor(agents_[i], rangeSq);
		}
	}
	else
	{
		const float distSqLeft = sqr(std::max(0.0f, agentTree_[agentTree_[node].left].minX - agent->position().x)) + sqr(std::max(0.0f, agent->position().x - agentTree_[agentTree_[node].left].maxX)) + sqr(std::max(0.0f, agentTree_[agentTree_[node].left].minY - agent->position().z)) + sqr(std::max(0.0f, agent->position().z - agentTree_[agentTree_[node].left].maxY));

		const float distSqRight = sqr(std::max(0.0f, agentTree_[agentTree_[node].right].minX - agent->position().x)) + sqr(std::max(0.0f, agent->position().x - agentTree_[agentTree_[node].right].maxX)) + sqr(std::max(0.0f, agentTree_[agentTree_[node].right].minY - agent->position().z)) + sqr(std::max(0.0f, agent->position().z - agentTree_[agentTree_[node].right].maxY));

		if (distSqLeft < distSqRight) {
			if (distSqLeft < rangeSq) {
				queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].left);

				if (distSqRight < rangeSq) {
					queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].right);
				}
			}
		}
		else {
			if (distSqRight < rangeSq) {
				queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].right);

				if (distSqLeft < rangeSq) {
					queryAgentTreeRecursive(agent, rangeSq, agentTree_[node].left);
				}
			}
		}

	}
}

void KdTree::queryObstacleTreeRecursive(SteerLib::AgentInterface *agent, float rangeSq, const ObstacleInterfaceTreeNode *node) const
{
	if (node == NULL) {
		return;
	}
	else {
		const ObstacleInterface *const obstacle1 = node->obstacle;
		const ObstacleInterface *const obstacle2 = obstacle1->nextObstacle_;

		const float agentLeftOfLine = leftOf(obstacle1->point_, obstacle2->point_, agent->position());

		queryObstacleTreeRecursive(agent, rangeSq, (agentLeftOfLine >= 0.0f ? node->left : node->right));

		const float distSqLine = sqr(agentLeftOfLine) / absSq(obstacle2->point_ - obstacle1->point_);

		if (distSqLine < rangeSq) {
			if (agentLeftOfLine < 0.0f) {
				/*
				 * Try obstacle at this node only if agent is on right side of
				 * obstacle (and can see obstacle).
				 */
				(agent)->insertObstacleNeighbor(node->obstacle, rangeSq);
			}

			/* Try other side of line. */
			queryObstacleTreeRecursive(agent, rangeSq, (agentLeftOfLine >= 0.0f ? node->right : node->left));

		}
	}
}

bool KdTree::queryVisibility(const Util::Point &q1, const Util::Point &q2, float radius) const
{
	return queryVisibilityRecursive(q1, q2, radius, obstacleTree_);
}

bool KdTree::queryVisibilityRecursive(const Util::Point &q1, const Util::Point &q2, float radius, const ObstacleInterfaceTreeNode *node) const
{
	if (node == NULL) {
		return true;
	}
	else {
		const ObstacleInterface *const obstacle1 = node->obstacle;
		const ObstacleInterface *const obstacle2 = obstacle1->nextObstacle_;

		const float q1LeftOfI = leftOf(obstacle1->point_, obstacle2->point_, q1);
		const float q2LeftOfI = leftOf(obstacle1->point_, obstacle2->point_, q2);
		const float invLengthI = 1.0f / absSq(obstacle2->point_ - obstacle1->point_);

		if (q1LeftOfI >= 0.0f && q2LeftOfI >= 0.0f) {
			return queryVisibilityRecursive(q1, q2, radius, node->left) && ((sqr(q1LeftOfI) * invLengthI >= sqr(radius) && sqr(q2LeftOfI) * invLengthI >= sqr(radius)) || queryVisibilityRecursive(q1, q2, radius, node->right));
		}
		else if (q1LeftOfI <= 0.0f && q2LeftOfI <= 0.0f) {
			return queryVisibilityRecursive(q1, q2, radius, node->right) && ((sqr(q1LeftOfI) * invLengthI >= sqr(radius) && sqr(q2LeftOfI) * invLengthI >= sqr(radius)) || queryVisibilityRecursive(q1, q2, radius, node->left));
		}
		else if (q1LeftOfI >= 0.0f && q2LeftOfI <= 0.0f) {
			/* One can see through obstacle from left to right. */
			return queryVisibilityRecursive(q1, q2, radius, node->left) && queryVisibilityRecursive(q1, q2, radius, node->right);
		}
		else {
			const float point1LeftOfQ = leftOf(q1, q2, obstacle1->point_);
			const float point2LeftOfQ = leftOf(q1, q2, obstacle2->point_);
			const float invLengthQ = 1.0f / absSq(q2 - q1);

			return (point1LeftOfQ * point2LeftOfQ >= 0.0f && sqr(point1LeftOfQ) * invLengthQ > sqr(radius) && sqr(point2LeftOfQ) * invLengthQ > sqr(radius) && queryVisibilityRecursive(q1, q2, radius, node->left) && queryVisibilityRecursive(q1, q2, radius, node->right));
		}
	}
}
