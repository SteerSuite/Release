//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __UTIL_DRAWLIB_H__
#define __UTIL_DRAWLIB_H__

/// @file DrawLib.h
/// @brief Declares Util::DrawLib functionality, which provides convenient wrappers to many drawing functions.

#ifdef ENABLE_GUI

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>

#include "util/Geometry.h"
#include "util/Color.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Globals.h"


namespace Util {

	/**
	 * @brief A collection of useful functions for drawing and displaying graphics using openGL.
	 *
	 * #init() must be called explicitly by the user;  We cannot rely on static initialization
	 * because openGL may not be initialized.
	 *
	 * If you use this library, in particular the inlined functions, you may need to link openGL into your code.
	 *
	 * <b> Warning!!! </b>
	 * DrawLib is one of the few places where static functionality is used in %SteerLib.  This is
	 * necessary so that it can use openGL nicely across many modules.  Usually this is not a
	 * problem, because SteerLib is a shared library.  However, if you must statically link
	 * SteerLib or DrawLib for some reason, beware that multiple instances of DrawLib may occur,
	 * which may cause unexpected bugs like objects disappearing.
	 *
	 *
	 * @todo
	 *  - Consider making the display list functionality public.
	 *  - Fill-in some additional shape-drawing functionality
	 *  - Add support for text drawing functions in this library.
	 *
	 */
	class UTIL_API DrawLib {
	public:

		/// Call this after initializing openGL to initialize DrawLib;
		static void init();

		/// Sets background color
		static void setBackgroundColor(const Color& color);

		/// @name Lighting functionality
		//@{
		static void enableLights();
		static void disableLights();
		/// Updates the light positions; needs to be called every frame, right after setting the camera view.
		static void positionLights();
		//@}

		/// @name OpenGL wrappers
		/// @brief wrappers using our Util geometry data types;  If you use these in a module, you will also need to link with openGL.
		//@{
		static inline void glVertex(const Point & vertex) { glVertex3f(vertex.x, vertex.y, vertex.z); }
		static inline void glNormal(const Vector & normal) { glNormal3f(normal.x, normal.y, normal.z); }
		static inline void glTranslate(const Point & pos) { glTranslatef(pos.x, pos.y, pos.z); }
		static inline void glColor(const Color & color) { glColor3f(color.r, color.g, color.b); }
		//@}

		/// @name Primitive shapes drawing functionality
		/// @brief make sure to set the color yourself, for example, using glColor3f(...) or DrawLib::glColor()
		//@{
		static void drawLine(const Point & startPoint, const Point & endPoint);
		static void drawLine(const Point & startPoint, const Point & endPoint, const Color & color);
		static void drawLineAlpha(const Point & startPoint, const Point & endPoint, const Color &color, float alpha);
		static void drawLine(const Point & startPoint, const Point & endPoint, const Color &color, float thickness);
		static void drawRay(const Ray & r, const Color & color);
		static void drawQuad(const Point & a, const Point & b, const Point & c, const Point & d);
		// static void drawCircle(const Point & center, float radius);
		static void drawBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
		static void drawBoxWireFrame(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
		// static void drawOrientedBox(...);
		// static void drawOrientedBoxWireFrame(...);
		static void drawCylinder(const Point & pos, float radius, float ymin, float ymax);
		static void drawCylinder(const Point & pos, float radius, float ymin, float ymax, Color color);
		// static void drawCylinderWireFrame(const Point & pos, float radius, float ymin, float ymax);
		// static void drawSphere(const Point & center, float radius);
		static void drawCube();
		static void drawSphere();
		//@}

		/// @name Composite shapes drawing functionality
		//@{
		static void drawStar(const Point & pos, const Vector & dir, float radius, const Color & color);
		static void drawHighlight(const Point & pos, const Vector & dir, float radius, const Color& color = Color(0.9f,0.9f,0.1f));
		static void drawFlag(const Point & loc, const Color& color = Color(0.9f, 0.0f, 0.0f), float scale = 1.0f);
		static void drawCircle(const Point & loc, const Color& color = Color(0.9f, 0.9f, 0.0f), float scale = 1.0f, int points = 12);
		// static void drawFlag(const Point & loc, const Color& color = Color(0.9f, 0.0f, 0.0f), float scale = 1.0f);
		static void drawAgentDisc(const Point & pos, const Vector & dir, float radius, const Color& color = Color(0.9f,0.4f,0.1f));
		static void drawAgentDisc(const Point & pos, float radius, const Color& color = Color(0.9f,0.4f,0.1f));
		static void drawAgentDisc(const Point & pos, const Vector & dir, const Vector & up, float radius, const Color& color);
		//@}

	private:
		/// @name Blocked functionality
		/// @brief these functions are not publically accessible / not implemented, to protect DrawLib from being instanced twice.
		//@{
		/// Initializes all drawing functionality, called only the first time _getInstance() is called.
		DrawLib();
		~DrawLib() {}
		DrawLib(const DrawLib & );  // not implemented, not copyable
		DrawLib& operator= (const DrawLib & );  // not implemented, not assignable
		//@}


		/// @name Display list utilities
		//@{
		static int _startDefiningDisplayList();
		static int _endDefiningDisplayList(int displayList);
		static int _drawDisplayList(int displayList);

		static int _buildAgentDisplayList();
		static int _buildAgentDotDisplayList();
		static int _buildFlagDisplayList();
		
		static int _buildCubeDisplayList();
		static int _buildSphereDisplayList();
		static int _buildCylinderDisplayList();
		//@}

		static std::vector<GLuint> _displayLists;
		static int _currentDisplayListBeingWritten;
		static int _agentDisplayList;
		static int _agentDotDisplayList;
		static int _flagDisplayList;
		
		static int _cubeDisplayList;
		static int _sphereDisplayList;
		static int _cylinderDisplayList;

		// quadrics for shape drawings
		static GLUquadricObj* _quadric;

		static bool _initialized;
	};

} // end namespace Util


#ifdef _WIN32
#pragma warning( pop )
#endif


#endif // ifdef ENABLE_GUI

#endif
