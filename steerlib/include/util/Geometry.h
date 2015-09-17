//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __UTIL_GEOMETRY_H__
#define __UTIL_GEOMETRY_H__

/// @file Geometry.h
/// @brief Declares and implements useful geometry functionality, such as vectors, points, rays.
///
/// @todo
///  - should some of the UTIL_API function calls be migrated to static calls, for performance?
///  - double-check Util::AxisAlignedBox implementation against pbrt?
///  - finish doxygen commenting in this file
///

#include <assert.h>
#include <iostream>
#include <math.h>
#include <float.h>
#include "Globals.h"
#include <vector>



#define _UTIL_GEOMETRY_EPSILON 0.00001f

#ifndef INFINITY
#define INFINITY FLT_MAX
#endif

#define M_PI            3.14159265358979323846264338328f
#define M_2_PI          6.28318530717958647692528676656f
#define M_PI_OVER_2     1.57079632679489661923132169164f
#define M_PI_OVER_180   0.0174532925199432957692369076849f
#define M_180_OVER_PI   57.2957795130823208767981548141f

namespace Util {

	/**
	 * @brief A geometric 3D vector data type.
	 *
	 * <h3> A note about performance </h3>
	 *
	 * This class (and other classes like Point, Color, etc.) can be used very much like a 
	 * primitive floating-point data type, but behind the scenes, many of these operations
	 * still cause extra instances of Vector or Point objects to be created on-the-fly.
	 * This causes default constructors to be called, often times which are not inlined,
	 * and this degrades performance severely.
	 *
	 * If performance is a major concern, you may still need to expand your math operations
	 * manually, accessing the x, y, z members directly.
	 *
	 * It is still worth using this class, however, for two reasons:  (1) compile-time type checking
	 * and (2) passing by reference requires only one reference instead of 3 references to individual
	 * x, y, z, elements.
	 * 
	 * @see
	 *  - Point class
	 *  - Ray class
	 *  - The ::normalize() function
	 *  - The ::dot() function
	 *  - The ::cross() function
	 */
	class UTIL_API Vector {
	public:
		float x, y, z;
		Vector() : x(0.0f), y(0.0f), z(0.0f) { }
		Vector(float newx, float newy, float newz) : x(newx), y(newy), z(newz) { }
		// Vector(Point p) : x(p.x), y(p.y), z(p.z) { }

		/// Accesses the Vector as if it were an array of 3 floats.
		float operator[](unsigned int i) const { assert(i<3); return (&x)[i]; }
		/// Accesses the Vector as if it were an array of 3 floats, allowing each element to be modified.
		float &operator[](unsigned int i) { assert(i<3); return (&x)[i]; }

		void zero() { x = 0.0f; y = 0.0f; z = 0.0f; }
		void one() { x = 1.0f; y = 1.0f; z = 1.0f; }

		/// @name Unary operations for arithmetic.
		//@{	
		/// If performance is important, use lengthSquared when possible instead of length(), because it avoids a costly square root.
		float lengthSquared() const { return x*x + y*y + z*z; }
		/// Computes the magnitude of the vector.
		float length() const { return sqrtf(x*x + y*y + z*z); }
		float norm() const { return length(); }
		/// Negates a vector.
		Vector operator-() const { return Vector(-x, -y, -z); }
		//@}

		/// @name Binary operators for arithmetic
		//@{
		/// Adds two vectors.
		Vector operator+(const Vector &vec) const { return Vector(x + vec.x, y + vec.y, z + vec.z); }
		Vector operator+(const float &f) const { return Vector(x + f, y + f, z + f); }
		void operator+=(const Vector &vec) { x += vec.x; y += vec.y; z += vec.z; }
		/// Subtracts two vectors.
		Vector operator-(const Vector &vec) const { return Vector(x - vec.x, y - vec.y, z - vec.z); }
		void operator-=(const Vector &vec) { x -= vec.x; y -= vec.y; z -= vec.z; }
		/// Multiplies a vector by a scalar.
		Vector operator*(float c) const { return Vector(c*x, c*y, c*z); }
		// compute dot product
		float operator*( const Vector &vec2) const { return x * vec2.x + y * vec2.y + z * vec2.z; }
		void operator*=(float c) { x *= c; y *= c; z *= c; }

		/// Divides a vector by a scalar.
		Vector operator/(float c) const { float cInverse = 1.0f / c; return Vector(cInverse*x, cInverse*y, cInverse*z); }
		void operator/=(float c) { float cInverse = 1.0f / c; (*this) *= cInverse; }
		//@}

		/// @name Binary operators for logic
		//@{
		/// Returns true if two vectors are equal.
		bool operator==(const Vector &vec) const { return ((x == vec.x) && (y == vec.y) && (z == vec.z)); }
		/// Returns true if all elements are equal to the same scalar value.
		bool operator==(const float c) const { return ((x == c) && (y == c) && (z == c)); }
		/// Returns true if two vectors are not equal.
		bool operator!=(const Vector &vec) const { return ((x != vec.x) || (y != vec.y) || (z != vec.z)); }
		/// Returns true if all elements are not equal to the same scalar value.
		bool operator!=(const float c) const { return ((x != c) && (y != c) && (z != c)); }

		//@}
		/*
		Point point()
		{
			return Point(this->x, this->y, this->z);
		}*/

	};

	/**
	 * @brief A geometric 3D point data type.
	 *
	 * @see
	 *  - Vector class documentation for an important note about performance.
	 *
	 */
	class UTIL_API Point {
	public:
		float x, y, z;
		Point() : x(0.0f), y(0.0f), z(0.0f) { }
		Point(float newx, float newy, float newz) : x(newx), y(newy), z(newz) { }
		// Point(Vector p) : x(p.x), y(p.y), z(p.z) { }
		/**
			 * \brief      Returns the x-coordinate of this two-dimensional vector.
			 * \return     The x-coordinate of the two-dimensional vector.
			 */
		// inline float x() const { return x; }

		float operator[](unsigned int i) const { assert(i<3); return (&x)[i]; }
		float &operator[](unsigned int i) { assert(i<3); return (&x)[i]; }
		/*
		float x() const { return x; }
		float y() const { return y; }
		float z() const { return z; }
		*/
		Point operator+(const Vector &vec) const { return Point(x + vec.x, y + vec.y, z + vec.z); }
		Point operator+(const Point &pt)   const { return Point(x + pt.x,  y + pt.y,  z + pt.z);  }
		Point operator-(const Vector &vec) const { return Point(x - vec.x, y - vec.y, z - vec.z); }
		Vector operator-(const Point &pt)  const {return Vector(x - pt.x,  y - pt.y,  z - pt.z);  }
		Point operator* (float c) const { return Point(c*x, c*y, c*z); }
		Point operator/ (float c) const { float cInverse = 1.0f / c; return Point(cInverse*x, cInverse*y, cInverse*z); }

		//Added by Jennie Shapira for testing a timestamp in TrackReader for ShadowAI
		bool operator==(float c) const { float epsilon = 0.001f; return ((c <= y+epsilon) && (c >= y-epsilon));}
		
		bool operator==(const Point &pt) const { return ((x == pt.x) && (y == pt.y) && (z == pt.z)); }
		bool operator!=(const Point &pt) const { return ((x != pt.x) || (y != pt.y) || (z != pt.z)); }

		bool operator<(const Point &pt) const { return x*x + y*y + z*z < pt.x*pt.x + pt.y*pt.y + pt.z*pt.z; }

		Vector vector()
		{
			return Vector(this->x, this->y, this->z);
		}
	};


	/**
	 * @brief A geometric 3D ray data type.
	 *
	 * A Ray is also essentially a line segment, where the line segment starts at pos, and
	 * ends at (pos + maxt*dir).  There are two common ways to initialize the ray that are
	 * useful for different purposes:
	 *
	 * -# #initWithLengthInterval() initializes the ray by <b>normalizing</b> the direction of the vector.  In this way, maxt
	 *    represents the length of the line segment (mint is some epsilon close to zero).
	 * -# #initWithUnitInterval() initializes the ray without modifying the magnitude of direction.  In this way, the interval
	 *    between mint and maxt is (0,1].  Aside from being useful in some cases, it also initializes faster than initializing
	 *    a length interval, because it does not need a costly normalization operation.
	 *
	 */
	class UTIL_API Ray
	{
	public:
		Point pos;
		Vector dir;
		float mint, maxt;
		/// Initializes a ray so that mint is an epsilon close to zero and maxt is the length of the line segment.
		void initWithLengthInterval(Point newPos, Vector newDir) { pos = newPos; dir = newDir; mint = _UTIL_GEOMETRY_EPSILON; maxt = dir.length(); dir = dir/maxt; }
		/// Initializes a ray so that the interval of mint and maxt is (0,1].
		void initWithUnitInterval(Point newPos, Vector newDir) { pos = newPos; dir = newDir; mint = _UTIL_GEOMETRY_EPSILON; maxt = 1.0f; } // if all else is equal, use this one, because it initializes faster.
		/// Returns the point location of the ray given parameter t
		Point eval(float t) const { return pos + dir * t; }
	};

	class UTIL_API Plane
	{
	public:
		/**
		 * \brief   A point on the plane.
		 */
		Point point;

		/**
		 * \brief   The normal to the plane.
		 */
		Vector normal;
	};


	class UTIL_API Circle
	{
	public:
		float x, z, r;
		Circle() : x(0.0f), z(0.0f), r(0.0f) { }
		Circle(float newx, float newz, float newRadius) : x(newx), z(newz), r(newRadius) { }

		Point Position()
		{
			return Point(x, 0.0f, z);
		}

		float Radius()
		{
			return r;
		}
	};


	/**
	 * @brief A geometric 3D axis-aligned box data type.
	 */
	class UTIL_API AxisAlignedBox {
	public:
		float xmin, xmax, ymin, ymax, zmin, zmax;
		AxisAlignedBox() : xmin(FLT_MAX), xmax(-FLT_MAX), ymin(FLT_MAX), ymax(-FLT_MAX), zmin(FLT_MAX), zmax(-FLT_MAX) { }
		AxisAlignedBox(float newxmin, float newxmax, float newymin, float newymax, float newzmin, float newzmax) : xmin(newxmin), xmax(newxmax), ymin(newymin), ymax(newymax), zmin(newzmin), zmax(newzmax) { }
		//void expandBounds(const Point &p) { xmin = (p.x < xmin) ? p.x : xmin; xmax = (p.x > xmax) ? p.x : xmax; ymin = (p.y < ymin) ? p.y : ymin; ymax = (p.y > ymax) ? p.y : ymax; zmin = (p.z < zmin) ? p.z : zmin; zmax = (p.z > zmax) ? p.z : zmax; }
		//void setBounds(const Point &center, float radius) { float r = fabsf(radius); xmin = center.x - r; xmax = center.x + r; ymin = center.y - r; ymax = center.y + r; zmin = center.z - r; zmax = center.z + r; }
		//void setBounds(float newxmin, float newxmax, float newymin, float newymax, float newzmin, float newzmax) { xmin = newxmin; xmax=newxmax; ymin=newymin; ymax=newymax; zmin=newzmin; zmax=newzmax; }
	};


	/// The ostream << operator to output a Vector data type
	static inline std::ostream &operator<<(std::ostream &out, const Vector &vec) { out << "(" << vec.x << "," << vec.y << "," << vec.z << ")"; return out; }
	/// The ostream << operator to output a Point data type
	static inline std::ostream &operator<<(std::ostream &out, const Point &pt) { out << "(" << pt.x << "," << pt.y << "," << pt.z << ")"; return out; }
	/// The ostream << operator to output a Ray data type
	static inline std::ostream &operator<<(std::ostream &out, const Ray &r) { out << "pos: " << r.pos << "dir: " << r.dir << " range [" << r.mint << "," << r.maxt << "]"; return out; }
	/// The ostream << operator to output a AxisAlignedBox data type
	static inline std::ostream &operator<<(std::ostream &out, const AxisAlignedBox &bb) { out << "(xmin: " << bb.xmin << ", xmax: " << bb.xmax << ", ymin: " << bb.ymin << ", ymax: " << bb.ymax << ", zmin: " << bb.zmin << ", zmax: " << bb.zmax << ")"; return out; }

	static Vector North(0.0f, 0.0f, 1.0f);
	static Vector South(0.0f, 0.0f, -1.0f);
	static Vector West(1.0f, 0.0f, 0.0f);
	static Vector East(-1.0f, 0.0f, 0.0f);

	/// An additional float-vector multiplication operator, so that c*vec is the same as vec*c.
	static inline Vector operator*(float c, const Vector &vec) { return Vector(c*vec.x, c*vec.y, c*vec.z); }
	/// An additional float-point multiplication operator, so that c*pt is the same as pt*c.
	static inline Point operator*(float c, const Point &pt) { return Point(c*pt.x, c*pt.y, c*pt.z); }
	/// Vector normalization; beware it is costly, using both a division and a square-root operation.
	static inline Vector normalize(const Vector &vec) { float lengthInv = 1.0f / sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);  return Vector(lengthInv * vec.x, lengthInv * vec.y, lengthInv * vec.z); }
	static inline Point normalize(const Point &vec) { float lengthInv = 1.0f / sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);  return Point(lengthInv * vec.x, lengthInv * vec.y, lengthInv * vec.z); }
	/// Returns the dot product of two vectors.
	static inline float dot(const Vector &vec1, const Vector &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; }
	// Don't ask.... (Glen)
	static inline float dot(const Point &vec1, const Vector &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; }
	static inline float dot(const Vector &vec1, const Point &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; }
	static inline float dot(const Point &vec1, const Point &vec2) { return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z; }
	/// Returns the distance between two points
	static inline float distanceBetween(const Point &pt1, const Point &pt2) { return (pt1 - pt2).length(); }
	/// Returns the squared distance between two points; faster than distanceBetween by avoiding the square root.
	static inline float distanceSquaredBetween(const Point &pt1, const Point &pt2) { return (pt1 - pt2).lengthSquared(); }
	/// Returns the cross product (a Vector data type) of two vectors
	static inline Vector cross(const Vector &vec1, const Vector &vec2) {
		return Vector((vec1.y * vec2.z) - (vec1.z * vec2.y),
					  (vec1.z * vec2.x) - (vec1.x * vec2.z),
					  (vec1.x * vec2.y) - (vec1.y * vec2.x));
	}




	/// Returns a vector that is rotated in the x-z plane by 90 degrees.
	static inline Vector rightSideInXZPlane(const Vector & vec) { return Vector(-vec.z, vec.y, vec.x); }

	/// Returns a vector that is rotated in the x-z plane by the specified angle in radians.
	static inline Vector rotateInXZPlane(const Vector & dir, float angle) {
		const float s = sin(angle);
		const float c = cos(angle);
		return Vector((dir.x * c) + (dir.z * s),  (dir.y),  (dir.z * c) - (dir.x * s));
	}

	/// Returns a point that is rotated in the x-z plane by the specified angle in radians.
	static inline Point rotateInXZPlane(const Point & p, float angle) {
		const float s = sin(angle);
		const float c = cos(angle);
		return Point((p.x * c) + (p.z * s),  (p.y),  (p.z * c) - (p.x * s));
	}

	/// Returns a vector that has the same direction as vec, but has a clamped magnitude; i.e. magnitude is min( ||vec||, maxLength ).
	static inline Vector clamp( const Vector & vec, float maxLength ) {
		if (vec.lengthSquared() > maxLength*maxLength) {
			return maxLength * normalize(vec);
		}
		return vec;
	}

	/// converts degrees to radians
	static inline double radians(double angle)
	{
		return angle*M_PI_OVER_180;
	}




	/// Returns true if the ray intersects the circle, and sets the t parameter if they intersect.
	static inline bool rayIntersectsCircle2D(const Point & circleCenter, float radius, const Ray &r, float &t)
	{
		float A = dot(r.dir,r.dir);
		Vector diff = r.pos - circleCenter;
		float B = 2.0f * dot(r.dir,diff);
		float C = dot(diff,diff) - radius*radius;

		float discrim = (B*B - 4*A*C);
		float sqrtDiscrim = sqrtf(discrim);

		if (discrim < 0.0f) return false;

		float t0 = (-B - sqrtDiscrim) / (2.0f*A);
		float t1 = (-B + sqrtDiscrim) / (2.0f*A);

		if (t0>t1) std::swap(t0,t1);  // so that the next few lines of code can assume that t0 < t1.

		if ((t0 > r.mint) && (t0 < r.maxt)) {
			t = t0;
			return true;
		}
		else if ((t1 > r.mint) && (t1 < r.maxt)) {
			t = t1;
			return true;
		}
		return false;
	}

	
	/// Returns true if the ray intersects the box, and sets the t parameter if they intersect.
	static inline bool rayIntersectsBox2D(float xmin, float xmax, float zmin, float zmax, const Ray &r, float &t)
	{// ray.dir does not NEED to be normalized
		float txnear, txfar, tznear, tzfar, invRayDirx, invRayDirz;
		float mint = r.mint;
		float maxt = r.maxt;

		invRayDirx = 1.0f / r.dir.x;
		txnear = (xmin-r.pos.x) * invRayDirx;
		txfar = (xmax -r.pos.x) * invRayDirx;
		if (txnear>txfar) std::swap(txnear,txfar);
		mint = txnear > mint ? txnear : mint;
		maxt = txfar < maxt ? txfar : maxt;
		if (mint > maxt) return false;

		invRayDirz = 1.0f / r.dir.z;
		tznear = (zmin-r.pos.z) * invRayDirz;
		tzfar = (zmax -r.pos.z) * invRayDirz;
		if (tznear>tzfar) std::swap(tznear,tzfar);
		mint = tznear > mint ? tznear : mint;
		maxt = tzfar < maxt ? tzfar : maxt;
		if (mint > maxt) return false;

		t = mint;
		return true;
	}


	/// Returns true if the two 2D circles overlap, false if they do not.
	static inline bool circleOverlapsCircle2D(const Point & c1, float r1, const Point & c2, float r2)
	{
		float distSquared = (c2-c1).lengthSquared();
		float distThreshold = (r1+r2);
		float distSquaredThreshold = distThreshold*distThreshold;
		return (distSquared<distSquaredThreshold);
	}


	/// Returns the max penetration if they overlap, 0.0, if they do not.
	static inline float computeCircleCirclePenetration2D(const Point & c1, float r1, const Point & c2, float r2)
	{
		float dist = (c2-c1).length();
		float distThreshold = (r1+r2);
		float penetration = distThreshold - dist;
		return (penetration < 0.0f) ? 0.0f : penetration;
	}


	/// Returns true if a 2D circle and 2D box overlap, false if they do not.
	static inline bool boxOverlapsCircle2D(float xmin, float xmax, float zmin, float zmax, const Point & circleCenter, float radius)
	{
		bool overlaps;
		float radiusSquared;

		// Translate coordinates, placing the circleCenter at the origin.
		xmin -= circleCenter.x;
		xmax -= circleCenter.x;
		zmin -= circleCenter.z;
		zmax -= circleCenter.z;
		radiusSquared = radius * radius;

		if (xmax < 0) { // R to left of circle center
			if (zmax < 0) {			// R in lower left corner
				overlaps = ((xmax * xmax + zmax * zmax) < radiusSquared);
			}
			else if (zmin > 0) { // R in upper left corner
				overlaps = ((xmax * xmax + zmin * zmin) < radiusSquared);
			}
			else {					// R due West of circle
				overlaps = ( fabsf(xmax) < radius);	
			}
		}
		else if (xmin > 0) { // R to right of circle center
			if (zmax < 0) {			// R in lower right corner
				overlaps = ((xmin * xmin + zmax * zmax) < radiusSquared);
			}
			else if (zmin > 0) {	// R in upper right corner
				overlaps = ((xmin * xmin + zmin * zmin) < radiusSquared);
			}
			else {					// R due East of circle
				overlaps = (xmin < radius);
			}
		}
		else { // R on circle vertical centerline
			if (zmax < 0) {			// R due South of circle
				overlaps = (fabsf(zmax) < radius);
			}
			else if (zmin > 0) {	// R due North of circle
				overlaps = (zmin < radius);
			}
			else {					// R contains circle centerpoint
				overlaps = true;
			}
		}

		return overlaps;
	}



	/**
	 * @brief Returns the amount that a box penetrates a circle; ranging from 0.0 to the radius of the circle.
	 * 
	 * When using this function, think of it as a "box penetrating a circle."
	 *
	 * There is no clear way to define how much a "circle penetrates a box" - the "correct" definition 
	 * changes depending on several criteria, and any choice feels ad hoc.  Particularly, the problem 
	 * is how to deal with very thin objects, and whether to take max penetration or min penetration 
	 * in various cases.  In some ways, the real solution would be to compute "area overlap" instead of distance 
	 * penetration.
	 *
	 * Our solution is to clamp penetration.  This allows us to compute "how much the box penetrates the circle", but the interpretation
	 * of "how much the circle penetrates the box" is still ill-defined.
	 *
	 */
	static inline float computeBoxCirclePenetration2D(float xmin, float xmax, float zmin, float zmax, const Point & circleCenter, float radius)
	{
		float radiusSquared;
		float penetration = 0.0f;

		// Translate coordinates, placing circleCenter at the origin.
		xmin -= circleCenter.x;
		xmax -= circleCenter.x;
		zmin -= circleCenter.z;
		zmax -= circleCenter.z;
		radiusSquared = radius * radius;

		if (xmax < 0) {		// R to left of circle center
			if (zmax < 0) {			// R in lower left corner
				penetration = radius - sqrtf(xmax * xmax + zmax * zmax);
			}
			else if (zmin > 0) {	// R in upper left corner
				penetration = radius - sqrtf(xmax * xmax + zmin * zmin);
			}
			else {					// R due West of circle
				penetration = radius - fabsf(xmax);
			}
		}
		else if (xmin > 0) {// R to right of circle center
			if (zmax < 0) {			// R in lower right corner
				penetration = radius - sqrtf(xmin * xmin + zmax * zmax);
			}
			else if (zmin > 0) {	// R in upper right corner
				penetration = radius - sqrtf(xmin * xmin + zmin * zmin);
			}
			else {					// R due East of circle
				penetration = radius - xmin;
			}
		}
		else {				// R on circle vertical centerline
			if (zmax < 0) {			// R due South of circle
				penetration = radius - fabsf(zmax);
			}
			else if (zmin > 0) {	// R due North of circle
				penetration = radius - zmin;
			}
			else { // R contains circle centerpoint
				// WARNING!!! Read the documentation of this function to understand this special case.
				// clamping the penetration.
				penetration = radius;
			}
		}
		return (penetration > 0) ? penetration : 0.0f;
	} 	


	/// Returns true if two rays intersect (which always happens in 2D unless the rays are parallel), and returns their respective t parameters (t1, t2) if they do intersect
	static inline bool intersectTwoRays2D( const Point & pos1, const Vector & dir1, float & t1, const Point & pos2, const Vector & dir2, float & t2 )
	{
		/**
		 * Each ray can be written as:
		 * \code
		 *   pos + t*dir
		 * \endcode
		 * The two rays intersect when:
		 * \code
		 *  pos1 + t1*dir1 == pos2 + t2*dir2,
		 * \endcode
		 * where t1, t2 are the unknowns we want to solve.
		 *
		 * Because we are in 2D, the above equation is actually two equations with two unknowns,
		 * which is a simple 2-d linear system as follows:
		 * \code
		 *   [ dir1.x  -dir2.x ]  [ t1 ]     [  pos2.x-pos1.x ]
		 *   [ dir1.z  -dir2.z ]  [ t2 ]  =  [  pos2.z-pos1.z ]
		 * \endcode
		 * which can be solved by directly inverting the matrix.  The inverse of the matrix is:
		 * \code
		 *                       [ -dir2.z  dir2.x ]
		 *    1.0/determinant *  [ -dir1.z  dir1.x ]
		 * \endcode
		 *
		 * So, to solve for (t1,t2), we multiply the inverse with the vector (dx,dz).
		 */

		float det = ( (dir1.z*dir2.x) - (dir1.x*dir2.z) );

		// check if their directions are the same (i.e. parallel, then they won't intersect)
		if (det == 0.0f) return false;

		float inverseDet = 1.0f / det;   // simplified
		float dx = pos2.x - pos1.x;
		float dz = pos2.z - pos1.z;
		t1 = (-dir2.z*dx + dir2.x*dz) * inverseDet;
		t2 = (-dir1.z*dx + dir1.x*dz) * inverseDet;

		return true;
	}

	static inline bool intersect2Lines2D(const Point & l1Start, const Point & l1End, const Point & l2Start, const Point & l2End,
			Point & intersectionPoint)
	{
		float l1_t = -1, l2_t = -1;

		intersectTwoRays2D( l1Start, l1End - l1Start, l1_t,
				l2Start, l2End - l2Start, l2_t);

		// For there to be an intersection both of the t values must be >= 0 and <= 1
		if ( l1_t > 1 || l1_t < 0 || l2_t > 1 || l2_t < 0)
		{
			return false;
		}

		intersectionPoint = l1Start + ((l1End - l1Start) * l1_t);
		return true;

	}

	/**
	 * Works for 3 points
	 */
	static inline Vector convertToBarycentric(std::vector<Point> points, Point point)
	{
	    Vector v0 = points[0] - points[2];
	    Vector v1 = points[1] - points[2];
	    Vector v2 = point - points[2];
	    float d00 = dot(v0, v0);
	    float d01 = dot(v0, v1);
	    float d11 = dot(v1, v1);
	    float d20 = dot(v2, v0);
	    float d21 = dot(v2, v1);
	    float denom = (d00 * d11) - (d01 * d01);


	    float a = (d11 * d20 - d01 * d21) / denom;
	    float b = (d00 * d21 - d01 * d20) / denom;
	    Vector weights(a, b, 1 - a - b);
	    return weights;
	}

	static inline Vector inverseDistance(std::vector<Point> points, Point point)
	{
	    float distance = 0.0;
	    for (size_t i=0; i < points.size(); i++)
	    {
	        distance = distance + (point-points.at(i)).length();
	    }
	    Vector weights((distance - (points[0]-point).length())/distance,
	    		(distance - (points[1]-point).length())/distance,
	    		(distance - (points[2]-point).length())/distance);

	    return weights;
	}

	static inline Vector normalizedInverseDistance(std::vector<Point> points, Point point)
	{

	    Vector weights = inverseDistance(points, point);
	    float norm = weights[0] + weights[1] + weights[2];

	    return weights/norm;
	}

	/**
	 * \brief      Computes the squared distance from a line segment with the
	 *             specified endpoints to a specified point.
	 * \param      a               The first endpoint of the line segment.
	 * \param      b               The second endpoint of the line segment.
	 * \param      c               The point to which the squared distance is to
	 *                             be calculated.
	 * \return     The squared distance from the line segment to the point.
	 */
	inline float distSqPointLineSegment(const Util::Point &a, const Util::Point &b,
										const Util::Point &c)
	{
		const float r = ((c - a) * (b - a)) / (b - a).lengthSquared();

		if (r < 0.0f) {
			return (c - a).lengthSquared();
		}
		else if (r > 1.0f) {
			return (c - b).lengthSquared();
		}
		else {
			return (c - (a + r * (b - a))).lengthSquared();
		}
	}

} // end namespace Util


#endif


