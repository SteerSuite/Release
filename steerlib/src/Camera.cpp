//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


/// @file Camera.cpp
/// @brief Implements the SteerLib::Camera class.
///
/// @todo
///   - document this file
///   - fix coding style
///   - options may be accessible now, is there anything to change because of this?

#include <iostream>


#include "simulation/Camera.h"
#include "util/DrawLib.h"
#include "util/GenericException.h"

using namespace std;
using namespace SteerLib;
using namespace Util;

const float Camera::INTERPOLATE_TIME = 0.7f;


Camera::Camera()
{
	m_pois.clear();
	reset();
}

// This sets the default position of the camera on start of simulation
void Camera::reset()
{

	m_currentView.set(Point(0.0f, 37.0f, 40), Point(0.0f, 0.0f, -5), Vector(0.0f, 1.0f, 0.0f), 45);
	m_targetView.set(Point(0.0f, 37.0f, 40), Point(0.0f, 0.0f, -5), Vector(0.0f, 1.0f, 0.0f), 45);

	m_isInterpolating = false;
	m_interpolationTime = 0.f;

	m_currentPoi = -1;
	useNextPointOfInterest();
}

void Camera::setView(const CameraView & view)
{
	m_targetView = view;

	m_interpolationTime = 0.f;
	m_isInterpolating = true;
}

void Camera::setView(const Point & pos, const Point & lookat, const Vector & up, float fovy)
{
	setView(CameraView(pos, lookat, up, fovy));
}

int Camera::addPointOfInterest(const Point & pos, const Point & lookat, const Vector & up)
{
	CameraView poi;
	poi.position = pos;
	poi.lookat = lookat;
	poi.up = up;

	m_pois.push_back(poi);
	
	if(m_pois.size() == 1)
	{
		m_currentPoi = 0;
		setView(m_pois[0]);
	}

	return (int)m_pois.size() - 1;
}

int Camera::addPointOfInterest(const Point & pos, const Point & lookat)
{
	return addPointOfInterest(pos, lookat, Vector(0.0f, 1.0f, 0.0f));
}

void Camera::useNextPointOfInterest()
{
	if(m_pois.size() == 0)
		return;

	m_currentPoi++;
	if(m_currentPoi >= (int)m_pois.size())
		m_currentPoi = 0;

	setView(m_pois[m_currentPoi]);
}

void Camera::update(float totalTime, float elapsedTime)
{
	if (!m_isInterpolating)
		return;

	float timeRange = INTERPOLATE_TIME - m_interpolationTime;

	if(timeRange - elapsedTime < 0)
	{
		m_currentView = m_targetView;
		m_isInterpolating = false;
		return;
	}

	float wTarg = elapsedTime / timeRange;
	float wCurr = 1.0f - wTarg;

	m_currentView.position = wCurr * m_currentView.position + wTarg*m_targetView.position;
	m_currentView.lookat = wCurr * m_currentView.lookat + wTarg*m_targetView.lookat;
	m_currentView.up = wCurr * m_currentView.up + wTarg*m_targetView.up;
	m_currentView.fovy = wCurr * m_currentView.fovy + wTarg*m_targetView.fovy; // not sure if it's best to interpolate FOV

	m_interpolationTime += elapsedTime;
}

void Camera::apply()
{
#ifdef ENABLE_GUI
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_currentView.position.x, m_currentView.position.y, m_currentView.position.z,
		m_currentView.lookat.x, m_currentView.lookat.y, m_currentView.lookat.z,
		m_currentView.up.x, m_currentView.up.y, m_currentView.up.z);
#else
	throw GenericException("Camera::apply() cannot be called, this version of SteerLib compiled without GUI functionality.");
#endif
}

void Camera::apply_stereo(bool right)
{
#ifdef ENABLE_GUI
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const float eyeSep = 0.3f; // TODO: make configurable
	Vector vd = m_currentView.lookat - m_currentView.position;
	Vector rightVec = cross(normalize(m_currentView.lookat - m_currentView.position), m_currentView.up);
	Point position = m_currentView.position + rightVec * eyeSep * (right ? 1 : -1);
	Point lookat = m_currentView.lookat + rightVec * eyeSep * (right ? 1 : -1);

	gluLookAt(position.x, position.y, position.z,
		lookat.x, lookat.y, lookat.z,
		m_currentView.up.x, m_currentView.up.y, m_currentView.up.z);
#else
	throw GenericException("Camera::apply_stereo() cannot be called, this version of SteerLib compiled without GUI functionality.");
#endif
}

void Camera::nudgeRotate(float tilt, float swivel)
{
	// stop interpolation movement
	m_isInterpolating = false;


	// figure out camera position

	Vector arm = m_currentView.position - m_currentView.lookat;
	//float armL = arm.length();

	float cosT = cos(tilt);
	float sinT = sin(tilt);

	float yL = arm.y;
	float xzL = sqrt(arm.x*arm.x + arm.z*arm.z);

	float newY = yL * cosT + xzL * sinT;
	float newXZ = xzL * cosT - yL * sinT;

	float xzScaler = fabsf(newXZ / xzL);

	float cosS = cos(swivel);
	float sinS = sin(swivel);

	float newX = xzScaler * (arm.x * cosS + arm.z * sinS);
	float newZ = xzScaler * (arm.z * cosS - arm.x * sinS);

	Vector offset(newX, newY, newZ);

	// bump the camera
	m_currentView.position = m_currentView.lookat + offset;
}

void Camera::nudgeZoom(float zoom)
{
	// stop interpolation movement
	m_isInterpolating = false;


	// get zoom amount

	Vector arm = m_currentView.position - m_currentView.lookat;

	// don't zoom if it'll change the direction of the arm
	if(arm.length() + zoom < .05)
		return;

	// we want the slightly exponential form, it is intuitively nicer for zooming.
	Vector offset = zoom*arm;

	// bump the camera
	m_currentView.position = m_currentView.position + offset;
}

void Camera::nudgePosition(float xNudge, float yNudge)
{
	// stop interpolation movement
	m_isInterpolating = false;

	// x and y, as the params see it, is relative to the camera view
	// gotta figure out what the "x" and "y" axis are for the camera

	// first constraint: we want to move in a plane parallel to the ground

	// "x" axis is then to the right of the camera.
	// "y" axis is the forward direction of the camera

	Vector arm = m_currentView.lookat - m_currentView.position;

	float distance = sqrt(arm.length());

	Vector forward = arm;
	forward.y = 0.f;
	forward = normalize(forward);

	// we have forward, or the "y" axis.  "x" axis is then the forward rotated 90 degrees
	Vector side(forward.z, 0.f, -forward.x);

	xNudge *= distance;
	yNudge *= distance;

	Vector adjustment = distance * (xNudge * side + yNudge * forward);

	// we have the adjustments, now apply them
	m_currentView.position = m_currentView.position + adjustment;
	m_currentView.lookat = m_currentView.lookat + adjustment;
}

Point Camera::position()
{
	return m_currentView.position;
}

Point Camera::lookat()
{
	return m_currentView.lookat;
}

Vector Camera::up()
{
	return m_currentView.up;
}

const float Camera::fovy() const
{
	return m_currentView.fovy;
}

CameraView Camera::nextPointOfInterestView()
{
	if(m_pois.size() == 0)
	{
		return m_currentView;
	}

	int nextPoi = m_currentPoi + 1;
	
	if(nextPoi >= (int)m_pois.size())
		nextPoi = 0;

	return m_pois[nextPoi];
}

Point Camera::setPosition(const Point & pos)
{
	return m_targetView.position = pos;
}

Point Camera::setLookat(const Point & lookat)
{
	return m_targetView.lookat = lookat;
}

Vector Camera::setUp(const Vector & up)
{
	return m_targetView.up = up;
}
