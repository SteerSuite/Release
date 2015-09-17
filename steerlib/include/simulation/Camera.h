//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __STEERLIB_CAMERA_H__
#define __STEERLIB_CAMERA_H__

/// @file Camera.h
/// @brief Declares a simple camera class
/// @todo
///   - document this file and the .cpp file
///   - make the code more SteerLib style

#include <vector>
#include "util/Geometry.h"
#include "Globals.h"

#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace SteerLib {

	class STEERLIB_API CameraView {
		public:
			/// Position of the camera
			Util::Point position;
			/// Point that the camera should look at
			Util::Point lookat;
			/// Up direction relative to the camera
			Util::Vector up;
			/// Field of view (fov) in the y direction
			float fovy;

			/// Default constructor, initializes the camera to the origin, looking down the positive z-axis.
			CameraView() {
				position = Util::Point(0.0f, 0.0f, 0.0f);
				lookat = Util::Point(0.0f, 0.0f, 1.0f);
				up = Util::Vector(0.0f, 1.0f, 0.0f);
			}

			/// Initializes the camera to the desired position, looking at the desired target.
			CameraView(const Util::Point & newPos, const Util::Point & newLookat, const Util::Vector & newUp, const float newFovy) {
				set(newPos, newLookat, newUp, newFovy);
			}

			/// Sets the camera to the desired position, looking at the desired target, with the specified up orientation and vertical field-of-view.
			inline void set(const Util::Point & newPos, const Util::Point & newLookat, const Util::Vector & newUp, const float newFovy) {
				position = newPos;
				lookat = newLookat;
				up = newUp;
				fovy = newFovy;
			}
	};
	
	
	class STEERLIB_API Camera
		{
		public:
			Camera();
			
			void reset();
			
			void setView(const CameraView & view);
			void setView(const Util::Point & pos, const Util::Point & lookat, const Util::Vector & up, const float fovy);
			
			int addPointOfInterest(const Util::Point & pos, const Util::Point & lookat, const Util::Vector & up);
			int addPointOfInterest(const Util::Point & pos, const Util::Point & lookat);
			
			void useNextPointOfInterest();
			
			void update(float totalTime, float elapsedTime);
			void apply();

			void apply_stereo(bool right);
			
			void nudgeRotate(float tilt, float swivel);
			void nudgeZoom(float zoom);
			void nudgePosition(float xNudge, float yNudge);
			
			Util::Point position();
			Util::Point lookat();
			Util::Vector up();
			const float fovy() const;
			
			CameraView nextPointOfInterestView();
			
		private:
			Util::Point setPosition(const Util::Point & pos);
			Util::Point setLookat(const Util::Point & lookat);
			Util::Vector setUp(const Util::Vector & up);
			
			CameraView m_currentView;
			CameraView m_targetView;
			
			bool m_isInterpolating;
			float m_interpolationTime;
			
			// pois = Points of Interest
			std::vector<CameraView> m_pois;
			int m_currentPoi;
			
			static const float INTERPOLATE_TIME;
		};
	
} // namespace SteerLib

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
