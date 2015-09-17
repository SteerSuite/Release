//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_ORIENTED_BOX_OBSTACLE_H__
#define __STEERLIB_ORIENTED_BOX_OBSTACLE_H__

/// @file OrientedBoxObstacle.h
/// @brief Declares the OrientedBoxObstacle class
/// @todo
///   - remove the hard-coded arbitrary value for getTraversalCost


#include "interfaces/ObstacleInterface.h"
#include "Globals.h"


namespace SteerLib {


	class STEERLIB_API OrientedBoxObstacle : public SteerLib::ObstacleInterface
	{
	public:
		//OrientedBoxObstacle(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) : _bounds(xmin, xmax, ymin, ymax, zmin, zmax) { _blocksLineOfSight = (ymax > 0.7) ? true : false; }
		//OrientedBoxObstacle(const Util::AxisAlignedBox & newBounds ) : _bounds(newBounds) { _blocksLineOfSight = (newBounds.ymax > 0.7) ? true : false; }

		OrientedBoxObstacle( Util::Point centerPosition, float lengthX, float lengthZ, float ymin, float ymax, float thetaY, float traversalCost=1001.0f );

		// ObstacleInterface functionality (not all virtual functions were overridden here)
		void draw(); // implementation in .cpp
		const Util::AxisAlignedBox & getBounds() { return _bounds; }
		virtual void setBounds(const Util::AxisAlignedBox & bounds) { _bounds = bounds; }

		/// @name The SpatialDatabaseItem interface
		/// @brief The OrientedBoxObstacle implementation of this interface represents a box that blocks line of sight if it is taller than 0.5 meter, and cannot be traversed.
		//@{
		virtual bool isAgent() { return false; }
		bool blocksLineOfSight() { return _blocksLineOfSight; }
		float getTraversalCost() { return _traversalCost; }

		virtual bool intersects(const Util::Ray &r, float &t) 
		{ 
			//std::cerr << " please debug intersects routine in OrientedBoxObstacle \n";
			Util::Ray ray = r;
			ray.pos = Util::Point(ray.pos.x - _centerPosition.x,ray.pos.y - _centerPosition.y,ray.pos.z - _centerPosition.z);
			ray.pos = Util::rotateInXZPlane(ray.pos,-_thetaY);
			ray.dir = Util::rotateInXZPlane ( ray.dir, -_thetaY );
			return Util::rayIntersectsBox2D(_bounds.xmin, _bounds.xmax, _bounds.zmin, _bounds.zmax, ray, t); 
		}
		virtual bool overlaps(const Util::Point & p, float radius) 
		{
			Util::Point pos = Util::Point(p.x - _centerPosition.x,p.y - _centerPosition.y,p.z - _centerPosition.z);
			pos = Util::rotateInXZPlane(pos,-_thetaY);
			return Util::boxOverlapsCircle2D(_dummyBounds.xmin, _dummyBounds.xmax, _dummyBounds.zmin, _dummyBounds.zmax,pos, radius); 
		}
		virtual float computePenetration(const Util::Point & p, float radius) 
		{ 
			Util::Point pos = Util::Point(p.x - _centerPosition.x,p.y - _centerPosition.y,p.z - _centerPosition.z);
			pos = Util::rotateInXZPlane(pos,-_thetaY);
			return Util::computeBoxCirclePenetration2D(_dummyBounds.xmin, _dummyBounds.xmax, _dummyBounds.zmin, _dummyBounds.zmax, pos, radius); 
		}
		virtual std::pair<std::vector<Util::Point>,std::vector<size_t> > getStaticGeometry()
		{
			std::cout << "get static geometry not implemented yet" << std::endl;
			std::vector<Util::Point> ps;
			std::vector<size_t> vs;
			return std::make_pair(ps, vs);
		}

		virtual std::vector<Util::Point> get2DStaticGeometry()
		{
			std::cout << "get static geometry not implemented yet" << std::endl;
			std::vector<Util::Point> ps;
			return ps;
		}

		//@}

	protected:
		Util::Point _centerPosition;
		float _lengthX;
		float _lengthZ;
		float _thetaY; // radians 
		Util::Vector _a,_b,_c,_d;
		Util::AxisAlignedBox _bounds;
		Util::AxisAlignedBox _dummyBounds; // used in overlaps and intersection routines.

		bool _blocksLineOfSight;

		float _traversalCost;
	};

}
#endif

