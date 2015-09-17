//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_TEST_CASE_IO_PRIVATE_H__
#define __STEERLIB_TEST_CASE_IO_PRIVATE_H__

/// @file TestCaseIOPrivate.h
/// @brief Declares private functionality for reading/writing SteerSuite test cases.

#include "Globals.h"
#include "simulation/Camera.h"
#include "util/Geometry.h"
#include "util/Color.h"
#include "tinyxml/ticpp.h"
#include "interfaces/SpatialDataBaseInterface.h"
#include "testcaseio/AgentInitialConditions.h"
#include "testcaseio/ObstacleInitialConditions.h"
#include "mersenne/MersenneTwister.h"
#include "testcaseio/Behaviour.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {

	/// Useful meta data from the XML &lt;header&gt; element of a test case.
	class STEERLIB_API TestCaseHeader {
	public:
		/// Version of the SteerSuite test case XML schema used for this test case.
		std::string version;
		/// String used to identify the test case
		std::string name;
		/// Description of the test case.
		std::string description;
		/// The bounds of the test case
		Util::AxisAlignedBox worldBounds;
		/// For now, just a human-readable string describing the passing criteria; eventually will be more elaborate.
		std::string passingCriteria;
	};

	/// Temporary agent information used by the TestCaseReader internally during parsing and initialization.
 	class STEERLIB_API RawAgentInfo : public SpatialDatabaseItem {
	public:
		std::string name;
		bool isPositionRandom;
		bool isDirectionRandom;
		bool isColorRandom;
		bool colorSet;
		Util::AxisAlignedBox regionBounds;
		Util::Point position;
		Util::Vector direction;
		Util::Color color;
		float radius;
		float speed;
		std::vector<AgentGoalInfo> goals;
		#ifdef VARIABLE_SPAWN_TIME
		float spawn_time, despawn_time;
		#endif
		
		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return true; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(position, radius, r, t); }
		virtual bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( position, this->radius, p, radius); }
		virtual float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( position, this->radius, p, radius); }
		//@}
	};

	/// Temporary obstacle information used by the TestCaseReader internally during parsing and initialization.
	class STEERLIB_API RawObstacleInfo : public SpatialDatabaseItem {
	public:
		bool isObstacleRandom;
		Util::AxisAlignedBox obstacleBounds;

		virtual ObstacleInitialConditions *getObstacleInitialConditions() = 0;

		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return false; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) { return false; }
		virtual bool overlaps(const Util::Point & p, float radius) { return false; }
		virtual float computePenetration(const Util::Point & p, float radius) { return 0.0; }
		//@}
	};

	class STEERLIB_API RawBoxObstacleInfo : public RawObstacleInfo {
	public:
		Util::AxisAlignedBox regionBounds;
		float size;
		float height;

		virtual ObstacleInitialConditions *getObstacleInitialConditions()
		{
			BoxObstacleInitialConditions *o = new BoxObstacleInitialConditions;

			o->xmin = obstacleBounds.xmin;
			o->xmax = obstacleBounds.xmax;
			o->ymin = obstacleBounds.ymin;
			o->ymax = obstacleBounds.ymax;
			o->zmin = obstacleBounds.zmin;
			o->zmax = obstacleBounds.zmax;

			return o;
		}

		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return false; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsBox2D(obstacleBounds.xmin, obstacleBounds.xmax, obstacleBounds.zmin, obstacleBounds.zmax, r, t); }
		virtual bool overlaps(const Util::Point & p, float radius) { return Util::boxOverlapsCircle2D(obstacleBounds.xmin, obstacleBounds.xmax, obstacleBounds.zmin, obstacleBounds.zmax,p, radius); }
		virtual float computePenetration(const Util::Point & p, float radius) { return Util::computeBoxCirclePenetration2D(obstacleBounds.xmin, obstacleBounds.xmax, obstacleBounds.zmin, obstacleBounds.zmax, p, radius); }
		//@}
	};

	class STEERLIB_API RawPolygonObstacleInfo : public RawObstacleInfo {
	public:
		std::vector<Util::Point> vertices;		

		virtual ObstacleInitialConditions *getObstacleInitialConditions()
		{
			PolygonObstacleInitialConditions *o = new PolygonObstacleInitialConditions(vertices);

			// for(int i=0; i<vertices.size(); i++)
			// 	o->_vertices.push_back(vertices[i]);

			return o;
		}

		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return false; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			bool ret = o->intersects(r, t);

			delete o;
			delete ic;

			return ret;
		}
		virtual bool overlaps(const Util::Point & p, float radius) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			bool ret = o->overlaps(p, radius);

			delete o;
			delete ic;

			return ret;
		}
		virtual float computePenetration(const Util::Point & p, float radius) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			float ret = o->computePenetration(p, radius);

			delete o;
			delete ic;

			return ret;
		}
		//@}
	};

	class STEERLIB_API RawOrientedBoxObstacleInfo : public RawObstacleInfo {
	public:
		Util::Point position;
		Util::Vector size;
		float thetaY;

		virtual ObstacleInitialConditions *getObstacleInitialConditions()
		{
			OrientedBoxObstacleInitialConditions *o = new OrientedBoxObstacleInitialConditions;

			o->height = size.y;
			o->lengthX = size.x;
			o->lengthZ = size.z;
			o->position = position;
			o->thetaY = thetaY;

			return o;
		}

		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return false; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			bool ret = o->intersects(r, t);

			delete o;
			delete ic;

			return ret;
		}
		virtual bool overlaps(const Util::Point & p, float radius) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			bool ret = o->overlaps(p, radius);

			delete o;
			delete ic;

			return ret;
		}
		virtual float computePenetration(const Util::Point & p, float radius) {
			ObstacleInitialConditions *ic = getObstacleInitialConditions();
			ObstacleInterface *o = ic->createObstacle();

			float ret = o->computePenetration(p, radius);

			delete o;
			delete ic;

			return ret;
		}
		//@}
	};	

	class STEERLIB_API RawOrientedWallObstacleInfo : public RawOrientedBoxObstacleInfo {
	public:
		
		double doorLocation;
		double doorRadius;

		virtual OrientedWallObstacleInitialConditions * getObstacleInitialConditions()
		{
			// std::vector<OrientedBoxObstacleInitialConditions *> obstacles;
			// OrientedWallObstacleInitialConditions * obstacle;

			OrientedWallObstacleInitialConditions *o = new OrientedWallObstacleInitialConditions;

			o->height = size.y;
			o->lengthX = size.x;
			o->lengthZ = size.z;
			o->position = position;
			o->thetaY = thetaY;
			o->doorLocation = doorLocation;
			o->doorRadius = doorRadius;

			return o;
		}

		//@}
	};	

	class STEERLIB_API RawCircleObstacleInfo : public RawObstacleInfo {
	public:
		Util::Point position;
		float radius;
		float height;

		virtual ObstacleInitialConditions *getObstacleInitialConditions()
		{
			CircleObstacleInitialConditions *o = new CircleObstacleInitialConditions;

			o->position = position;
			o->height = height;
			o->radius = radius;

			return o;
		}

		/// @name Implementation of the SpatialDatabaseItem interface
		//@{
		virtual bool isAgent() { return false; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0.0f; }
		virtual bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(position, radius, r, t); }
		virtual bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D(this->position, this->radius, p, radius); }
		virtual float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D(this->position, this->radius, p, radius); }
		//@}
	};



	/**
	 * @brief Private data for the TestCaseReader public interface
	 *
	 * This class should not be used directly.  Instead, use the TestCaseReader public interface that
	 * inherits from this class.
	 */
	class STEERLIB_API TestCaseReaderPrivate {
	protected:
		/// Protected constructor enforces that users cannot publically instantiate this class.
		TestCaseReaderPrivate() { }


		/// @name Helper functions for parsing
		//@{
		/// Parses the top-level SteerSuite test case element.
		void _parseTestCaseDOM(const ticpp::Element * root);
		/// Parses the header element.
		void _parseHeader(const ticpp::Element * subRoot);
		/// Parses a suggested camera view element.
		void _parseCameraView(const ticpp::Element * subRoot);
		/// Parses an agent element.
		void _parseAgent(const ticpp::Element * subRoot);
		/// Parses an agent element.
		void _parseAgentEmitter(const ticpp::Element * subRoot);
		/// Parses an agent region element.
		void _parseAgentRegion(const ticpp::Element * subRoot);
		/// Parses a box obstacle element.
		void _parseBoxObstacle(const ticpp::Element * subRoot);
		/// Parses a ploygon obstacle element.
		void _parsePolygonObstacle(const ticpp::Element * subRoot);
		/// Parses an oriented box obstacle element.
		void _parseOrientedBoxObstacle(const ticpp::Element * subRoot);
		/// Parses an oriented wall obstacle element.
		void _parseOrientedWallObstacle(const ticpp::Element * subRoot);
		/// Parses a circular obstacle element.
		void _parseCircleObstacle(const ticpp::Element * subRoot);
		/// Parses an obstacle region element.
		void _parseObstacleRegion(const ticpp::Element * subRoot);
		/// Parses the initial conditions specified in an agent or agent region.
		void _parseInitialConditions(const ticpp::Element * subRoot, RawAgentInfo & newAgent);
		/// Parses the sequence of goals specified in an agent or agent region.
		void _parseGoalSequence(const ticpp::Element * subRoot, std::vector<AgentGoalInfo> & goals);
		/// Reads a bounding-box data type from a SteerSuite test case.
		Util::AxisAlignedBox _getBoundsFromXMLElement(const ticpp::Element * subRoot);
		/// Reads a 3 element vector from a SteerSuite test case, or indicates that it should be randomly generated.
		void _getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Vector & xyzTuple, bool & isRandom);
		/// Reads a 3 element point from a SteerSuite test case, or indicates that it should be randomly generated.
		void _getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Point & xyzTuple, bool & isRandom);
		/// Reads a 3 element colour from a SteerSuite test case, or indicates that it should be randomly generated.
		void _getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Color & xyzTuple, bool & isRandom);
		///
		void _parseDoorwayXML(const ticpp::Element * subRoot, double & value, double & radius, bool & isRandom);
		/// Reads a Behaviour object from the xml data
		void _getBehaviorFromXMLElement(const ticpp::Element * subRoot, Behaviour * behavior);
		//@}

		/// @name Helper functions to set up initial conditions
		//@{
		void _initObstacleInitialConditions( SteerLib::BoxObstacleInitialConditions & o, const Util::AxisAlignedBox & bounds );
		void _initAgentInitialConditions( SteerLib::AgentInitialConditions & a, const SteerLib::RawAgentInfo & agent );
		void _initAgentEmitterInitialConditions( SteerLib::AgentInitialConditions & a, const SteerLib::RawAgentInfo & agent );
		//@}


		MTRand _randomNumberGenerator;

		/// Data read directly from the XML test case header tag
		TestCaseHeader _header;
		/// The list of suggested camera views found in the test case
		std::vector<CameraView> _cameraViews;
		/// Initial conditions of all agents
		std::vector<AgentInitialConditions> _initializedAgents;
		/// Initial conditions of all agent emitters
		std::vector<AgentInitialConditions> _initializedAgentEmitters;
		/// Initial conditions of all obstacles
		std::vector<ObstacleInitialConditions*> _initializedObstacles;

		/// Temporary data of agents while parsing the test case
		std::vector<RawAgentInfo> _rawAgents;
		/// Temporary data of agent emitters while parsing the test case
		std::vector<RawAgentInfo> _rawAgentEmitters;
		/// Temporary data of obstacles while parsing the test case
		std::vector<RawObstacleInfo *> _rawObstacles;
	};

} // end namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
