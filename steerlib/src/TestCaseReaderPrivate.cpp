//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//
//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file TestCaseReaderPrivate.cpp
/// @brief Helper functions to read complex data types from an SteerSuite XML test case.

#include "testcaseio/TestCaseIO.h"
#include "testcaseio/Behaviour.h"
#include "util/Geometry.h"
#include "util/Misc.h"
#include "util/GenericException.h"

using namespace std;
using namespace SteerLib;
using namespace Util;


AxisAlignedBox TestCaseReaderPrivate::_getBoundsFromXMLElement(const ticpp::Element * subRoot)
{
	AxisAlignedBox bounds;
	bool xminSpecified = false;
	bool xmaxSpecified = false;
	bool yminSpecified = false;
	bool ymaxSpecified = false;
	bool zminSpecified = false;
	bool zmaxSpecified = false;

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();
		if (childTagName == "xmin") {
			child->GetText(&bounds.xmin);
			xminSpecified = true;
		}
		else if (childTagName == "xmax") {
			child->GetText(&bounds.xmax);
			xmaxSpecified = true;
		}
		else if (childTagName == "ymin") {
			child->GetText(&bounds.ymin);
			yminSpecified = true;
		}
		else if (childTagName == "ymax") {
			child->GetText(&bounds.ymax);
			ymaxSpecified = true;
		}
		else if (childTagName == "zmin") {
			child->GetText(&bounds.zmin);
			zminSpecified = true;
		}
		else if (childTagName == "zmax") {
			child->GetText(&bounds.zmax);
			zmaxSpecified = true;
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + " while parsing region bounds.\n");
		}
	}

	if ( ! (xminSpecified && xmaxSpecified && yminSpecified && ymaxSpecified && zminSpecified && zmaxSpecified)) {
		throw GenericException("Inside the element " + subRoot->Value() + " at line " + toString(subRoot->Row()) + ": please specify all bounds, <xmin>, <xmax>, <ymin>, <ymax>, <zmin>, <zmax>.");
	}

	return bounds;
}


void TestCaseReaderPrivate::_getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Vector & xyzTuple, bool & isRandom)
{
	Point p;
	_getXYZOrRandomFromXMLElement(subRoot,p,isRandom);
	xyzTuple.x = p.x;
	xyzTuple.y = p.y;
	xyzTuple.z = p.z;
}

void TestCaseReaderPrivate::_getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Color & xyzTuple, bool & isRandom)
{
	bool RTagSpecified = false;
	bool GTagSpecified = false;
	bool BTagSpecified = false;

	isRandom = false; // assume its false, until we see the flag is specified.

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "r") {
			child->GetText(&xyzTuple.r);
			RTagSpecified = true;
		}
		else if (childTagName == "g") {
			child->GetText(&xyzTuple.g);
			GTagSpecified = true;
		}
		else if (childTagName == "b") {
			child->GetText(&xyzTuple.b);
			BTagSpecified = true;
		}
		else if (childTagName == "random") {
			if (child->GetText() == "true") {
				isRandom = true;
			}
			else if (child->GetText() == "false") {
				isRandom = false;
			}
			else {
				throw GenericException("In tag <" + childTagName + "> found on line " + toString(child->Row()) + ": invalid boolean value, use string literal \"true\" or \"false\".");
			}
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	if ((RTagSpecified != GTagSpecified) || (RTagSpecified!=BTagSpecified)) {
		throw GenericException("Inside the element <" + subRoot->Value() + "> at line " + toString(subRoot->Row()) + ": please specify <r>, <g> and <b> values.");
	}

	// make sure the test case did not accidentally specify an exact point and then specify random=true.
	if (RTagSpecified == isRandom) {
		throw GenericException("Inside the element <" + subRoot->Value() + "> at line " + toString(subRoot->Row()) + ": either <r>, <g>, <b> tuple or <random>true</random> is allowed, not both.");
	}
}


void TestCaseReaderPrivate::_getXYZOrRandomFromXMLElement(const ticpp::Element * subRoot, Util::Point & xyzTuple, bool & isRandom)
{
	bool XTagSpecified = false;
	bool YTagSpecified = false;
	bool ZTagSpecified = false;

	isRandom = false; // assume its false, until we see the flag is specified.

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "x") {
			child->GetText(&xyzTuple.x);
			XTagSpecified = true;
		}
		else if (childTagName == "y") {
			child->GetText(&xyzTuple.y);
			YTagSpecified = true;
		}
		else if (childTagName == "z") {
			child->GetText(&xyzTuple.z);
			ZTagSpecified = true;
		}
		else if (childTagName == "random") {
			if (child->GetText() == "true") {
				isRandom = true;
			}
			else if (child->GetText() == "false") {
				isRandom = false;
			}
			else {
				throw GenericException("In tag <" + childTagName + "> found on line " + toString(child->Row()) + ": invalid boolean value, use string literal \"true\" or \"false\".");
			}
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	if ((XTagSpecified != YTagSpecified) || (XTagSpecified!=ZTagSpecified)) {
		throw GenericException("Inside the element <" + subRoot->Value() + "> at line " + toString(subRoot->Row()) + ": please specify <x>, <y> and <z> values.");
	}

	// make sure the test case did not accidentally specify an exact point and then specify random=true.
	if (XTagSpecified == isRandom) {
		throw GenericException("Inside the element <" + subRoot->Value() + "> at line " + toString(subRoot->Row()) + ": either <x>, <y>, <z> tuple or <random>true</random> is allowed, not both.");
	}
}

void TestCaseReaderPrivate::_parseDoorwayXML(const ticpp::Element * subRoot, double & value, double & radius, bool & isRandom)
{
	bool valueTagSpecified = false;
	

	isRandom = false; // assume its false, until we see the flag is specified.

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "value") {
			child->GetText(&value);
			valueTagSpecified = true;
		}
		else if ( childTagName == "doorRadius" )
		{
			child->GetText(&radius);
		}
		else if (childTagName == "random") {
			if (child->GetText() == "true") {
				isRandom = true;
			}
			else if (child->GetText() == "false") {
				isRandom = false;
			}
			else {
				throw GenericException("In tag <" + childTagName + "> found on line " + toString(child->Row()) + ": invalid boolean value, use string literal \"true\" or \"false\".");
			}
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	// make sure the test case did not accidentally specify an exact point and then specify random=true.
	if ( valueTagSpecified == isRandom )
	{
		throw GenericException("Inside the element <" + subRoot->Value() + "> at line " + toString(subRoot->Row()) + ": either <value> </value>s or <random>true</random> is allowed, not both.");
	}
}



void TestCaseReaderPrivate::_parseTestCaseDOM(const ticpp::Element * root)
{
	bool headerSpecified = false;

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(root); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// it's the address of the iterator's current content.
		if (childTagName == "header") {
			_parseHeader(&(*child)); 
			headerSpecified = true;
		}
		else if (headerSpecified) {
			if (childTagName == "suggestedCameraView") {
				_parseCameraView(&(*child)); 
			}
			else if (childTagName == "agent") {
				_parseAgent(&(*child)); 
			}
			else if (childTagName == "agentEmitter") {
				_parseAgentEmitter(&(*child)); 
			}
			else if (childTagName == "agentRegion") {
				_parseAgentRegion(&(*child)); 
			}
			else if (childTagName == "obstacle") {
				_parseBoxObstacle(&(*child)); 
			}
			else if (childTagName == "polygonObstacle") {
				_parsePolygonObstacle(&(*child)); 
			}
			else if (childTagName == "orientedBoxObstacle") {
				_parseOrientedBoxObstacle(&(*child));
			}
			else if (childTagName == "orientedWallObstacle") 
			{
				_parseOrientedWallObstacle(&(*child));
			}
			else if (childTagName == "circleObstacle") {
				_parseCircleObstacle(&(*child));
			}
			else if (childTagName == "obstacleRegion") {
				_parseObstacleRegion(&(*child)); 
			}
			else {
				throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
			}
		}
		else {
			throw GenericException("<header> must be the first part of the test case.");
		}
	}
}


void TestCaseReaderPrivate::_parseHeader(const ticpp::Element * subRoot)
{
	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "version") {
			child->GetText(&_header.version);
			if ( !(_header.version == "1.0" || _header.version == "1.2" || _header.version == "1.1") ) {
				throw GenericException("This test case reader supports version 1.0;  The test case format is version " + _header.version);
			}
		}
		else if (childTagName == "name") {
			child->GetText(&_header.name);
		}
		else if (childTagName == "worldBounds") {
			_header.worldBounds = _getBoundsFromXMLElement(&(*child));
		}
		else if (childTagName == "description") {
			child->GetText(&_header.description);
		}
		else if (childTagName == "passingCriteria") {
			child->GetText(&_header.passingCriteria);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}
}

void TestCaseReaderPrivate::_parseCameraView(const ticpp::Element * subRoot)
{
	CameraView cvi;

	cvi.position = Point(0.0f, 30.0f, -0.5f);
	cvi.lookat = Point(0.0f, 0.0f, 0.0f);
	cvi.up = Vector(0.0f, 1.0f, 0.0f);
	cvi.fovy = 45.0f;
	
	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "position") {
			bool isRandom;
			_getXYZOrRandomFromXMLElement(&(*child), cvi.position, isRandom);
			if (isRandom) throw GenericException("Camera position (line " + toString(child->Row()) + ") should not be random.");
		}
		else if (childTagName == "lookat") {
			bool isRandom;
			_getXYZOrRandomFromXMLElement(&(*child), cvi.lookat, isRandom);
			if (isRandom) throw GenericException("Camera lookat (line " + toString(child->Row()) + ") should not be random.");
		}
		else if (childTagName == "up") {
			bool isRandom;
			_getXYZOrRandomFromXMLElement(&(*child), cvi.up, isRandom);
			if (isRandom) throw GenericException("Camera up vector (line " + toString(child->Row()) + ") should not be random.");
		}
		else if (childTagName == "fovy") {
			child->GetText(&cvi.fovy);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	_cameraViews.push_back(cvi);
}

void TestCaseReaderPrivate::_parseAgent(const ticpp::Element * subRoot)
{
	RawAgentInfo newAgent;
	newAgent.regionBounds = _header.worldBounds;
	newAgent.color = Util::Color(-1,-1,-1);

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "name") {
			child->GetText(&newAgent.name);
		}
		else if (childTagName == "initialConditions") {
			_parseInitialConditions(&(*child), newAgent);
		}
		else if (childTagName == "goalSequence") {
			_parseGoalSequence(&(*child), newAgent.goals);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	_rawAgents.push_back(newAgent);
}

void TestCaseReaderPrivate::_parseAgentEmitter(const ticpp::Element * subRoot)
{
	RawAgentInfo newAgent;
	newAgent.regionBounds = _header.worldBounds;
	newAgent.color = Util::Color(-1,-1,-1);

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "name") {
			child->GetText(&newAgent.name);
		}
		else if (childTagName == "initialConditions") {
			_parseInitialConditions(&(*child), newAgent);
		}
		else if (childTagName == "goalSequence") {
			_parseGoalSequence(&(*child), newAgent.goals);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	_rawAgentEmitters.push_back(newAgent);
}

void TestCaseReaderPrivate::_parseAgentRegion(const ticpp::Element * subRoot)
{
	unsigned int numAgents;
	RawAgentInfo newAgent;
	newAgent.name = "";
	newAgent.isPositionRandom = true;

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "numAgents") {
			child->GetText(&numAgents);
		}
		else if (childTagName == "regionBounds") {
			newAgent.regionBounds = _getBoundsFromXMLElement(&(*child));
		}
		else if (childTagName == "initialConditions") {
			_parseInitialConditions(&(*child), newAgent);
		}
		else if (childTagName == "goalSequence") {
			_parseGoalSequence(&(*child), newAgent.goals);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	for (unsigned int i=0; i<numAgents; i++) {
		_rawAgents.push_back(newAgent);
	}

}

void TestCaseReaderPrivate::_parseBoxObstacle(const ticpp::Element * subRoot)
{
	RawBoxObstacleInfo *obst = new RawBoxObstacleInfo;
	obst->isObstacleRandom = false;
	obst->obstacleBounds = _getBoundsFromXMLElement(subRoot);
	
	// assuming no bugs, these values do not matter when isObstacleRandom==false.
	obst->height = 0.0f;
	obst->regionBounds = _header.worldBounds;
	obst->size = 0.0f;

	_rawObstacles.push_back(obst);
}

void TestCaseReaderPrivate::_parsePolygonObstacle(const ticpp::Element * subRoot)
{
	RawPolygonObstacleInfo *obst = new RawPolygonObstacleInfo;
	obst->isObstacleRandom = false;
	//obst.obstacleBounds = _getBoundsFromXMLElement(subRoot);

	ticpp::Iterator<ticpp::Element> child;

	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		if (childTagName == "vertex") {
			Util::Point vertex;
			_getXYZOrRandomFromXMLElement(&(*child), vertex, obst->isObstacleRandom);
			obst->vertices.push_back(vertex);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	//obst->obstacleBounds = AxisAlignedBox(obst->position.x-obst->radius, obst->position.x+obst->radius, obst->position.y, obst->position.y+obst->height, obst->position.z-obst->radius, obst->position.z+obst->radius);
	ObstacleInitialConditions *ic = obst->getObstacleInitialConditions();
	ObstacleInterface *o = ic->createObstacle();
	obst->obstacleBounds = o->getBounds();

	delete o;
	delete ic;

	_rawObstacles.push_back(obst);
}

void TestCaseReaderPrivate::_parseOrientedBoxObstacle(const ticpp::Element * subRoot)
{
	RawOrientedBoxObstacleInfo *obst = new RawOrientedBoxObstacleInfo;
	obst->isObstacleRandom = false;
	//obst.obstacleBounds = _getBoundsFromXMLElement(subRoot);

	ticpp::Iterator<ticpp::Element> child;

	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		if (childTagName == "thetaY") {
			child->GetText(&obst->thetaY);
		}
		else if (childTagName == "size") {
			_getXYZOrRandomFromXMLElement(&(*child), obst->size, obst->isObstacleRandom);
		}
		else if (childTagName == "position") {
			_getXYZOrRandomFromXMLElement(&(*child), obst->position, obst->isObstacleRandom);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	//obst->obstacleBounds = AxisAlignedBox(obst->position.x-obst->radius, obst->position.x+obst->radius, obst->position.y, obst->position.y+obst->height, obst->position.z-obst->radius, obst->position.z+obst->radius);
	ObstacleInitialConditions *ic = obst->getObstacleInitialConditions();
	ObstacleInterface *o = ic->createObstacle();
	obst->obstacleBounds = o->getBounds();

	delete o;
	delete ic;

	_rawObstacles.push_back(obst);
}

void TestCaseReaderPrivate::_parseOrientedWallObstacle(const ticpp::Element * subRoot)
{
	RawOrientedWallObstacleInfo *obst = new RawOrientedWallObstacleInfo;
	obst->isObstacleRandom = false;
	//obst.obstacleBounds = _getBoundsFromXMLElement(subRoot);

	ticpp::Iterator<ticpp::Element> child;

	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		if (childTagName == "thetaY") {
			child->GetText(&obst->thetaY);
		}
		else if (childTagName == "size") {
			_getXYZOrRandomFromXMLElement(&(*child), obst->size, obst->isObstacleRandom);
		}
		else if (childTagName == "position") {
			_getXYZOrRandomFromXMLElement(&(*child), obst->position, obst->isObstacleRandom);
		}
		else if (childTagName == "doorway") {
			_parseDoorwayXML(&(*child), obst->doorLocation, obst->doorRadius, obst->isObstacleRandom);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	//obst->obstacleBounds = AxisAlignedBox(obst->position.x-obst->radius, obst->position.x+obst->radius, obst->position.y, obst->position.y+obst->height, obst->position.z-obst->radius, obst->position.z+obst->radius);
	ObstacleInitialConditions *ic = obst->getObstacleInitialConditions();
	ObstacleInterface *o = ic->createObstacle();
	obst->obstacleBounds = o->getBounds();
	delete o;
	delete ic;

	/*
	for (size_t i = 0; i < ics.size(); i++)
	{
		RawOrientedBoxObstacleInfo * boi = new RawOrientedBoxObstacleInfo();
		boi->position = ics.at(i)->position;
		boi->thetaY = ics.at(i)->thetaY;
		boi->size.x = ics.at(i)->lengthX;
		boi->size.z = ics.at(i)->lengthZ;
		boi->size.y = ics.at(i)->height;
		boi->isObstacleRandom = false;
		ObstacleInterface *o = ics.at(i)->createObstacle();
		boi->obstacleBounds = o->getBounds();
		delete o;
		delete ics[i];

		_rawObstacles.push_back(boi);
	}*/
	// delete obst;
	_rawObstacles.push_back(obst);

}

void TestCaseReaderPrivate::_parseCircleObstacle(const ticpp::Element * subRoot)
{
	RawCircleObstacleInfo *obst = new RawCircleObstacleInfo;
	obst->isObstacleRandom = false;
	//obst.obstacleBounds = _getBoundsFromXMLElement(subRoot);

	ticpp::Iterator<ticpp::Element> child;

	for (child = child.begin(subRoot); child != child.end(); child++ ) {
		std::string childTagName = child->Value();

		if (childTagName == "radius") {
			child->GetText(&obst->radius);
		}
		else if (childTagName == "height") {
			child->GetText(&obst->height);
		}
		else if (childTagName == "position") {
			_getXYZOrRandomFromXMLElement(&(*child), obst->position, obst->isObstacleRandom);
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	obst->obstacleBounds = AxisAlignedBox(obst->position.x-obst->radius, obst->position.x+obst->radius, obst->position.y, obst->position.y+obst->height, obst->position.z-obst->radius, obst->position.z+obst->radius);

	// assuming no bugs, these values do not matter when isObstacleRandom==false.
	//obst.height = 0.0f;
	//obst.regionBounds = _header.worldBounds;
	//obst.size = 0.0f;

	_rawObstacles.push_back(obst);
}

void TestCaseReaderPrivate::_parseObstacleRegion(const ticpp::Element * subRoot)
{
	RawBoxObstacleInfo *obst = new RawBoxObstacleInfo;
	unsigned int numObstacles;
	obst->isObstacleRandom = true;
	obst->obstacleBounds = AxisAlignedBox(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);  // assuming no bugs, this value does not matter when isObstacleRandom==true.

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "numObstacles") {
			child->GetText(&numObstacles);
		}
		else if (childTagName == "obstacleSize") {
			child->GetText(&obst->size);
		}
		else if (childTagName == "obstacleHeight") {
			child->GetText(&obst->height);
		}
		else if (childTagName == "regionBounds") {
			obst->regionBounds = _getBoundsFromXMLElement(&(*child));
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + "\n");
		}
	}

	for (unsigned int i=0; i<numObstacles; i++) {
		_rawObstacles.push_back(obst);
	}
}


void TestCaseReaderPrivate::_parseGoalSequence(const ticpp::Element * subRoot, std::vector<AgentGoalInfo> & goals)
{

	ticpp::Iterator<ticpp::Element> child;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		AgentGoalInfo newGoal;

		// re-initialize the goal to dummy values, since not everything will get initialized depending on the goal type.
		newGoal.targetIsRandom = false;
		newGoal.targetDirection = Vector(1.0f, 0.0f, 0.0f);
		newGoal.targetLocation = Point(0.0f, 0.0f, 0.0f);
		newGoal.targetName = "";
		newGoal.timeDuration = 100000.0f;
		newGoal.flowType = "";

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "seekStaticTarget") {
			newGoal.goalType = GOAL_TYPE_SEEK_STATIC_TARGET;
		}
		else if (childTagName == "fleeStaticTarget") {
			newGoal.goalType = GOAL_TYPE_FLEE_STATIC_TARGET;
		}
		else if (childTagName == "seekDynamicTarget") {
			newGoal.goalType = GOAL_TYPE_SEEK_DYNAMIC_TARGET;
		}
		else if (childTagName == "fleeDynamicTarget") {
			newGoal.goalType = GOAL_TYPE_FLEE_DYNAMIC_TARGET;
		}
		else if (childTagName == "flowStaticDirection") {
			newGoal.goalType = GOAL_TYPE_FLOW_STATIC_DIRECTION;
		}
		else if (childTagName == "flowDynamicDirection") {
			newGoal.goalType = GOAL_TYPE_FLOW_DYNAMIC_DIRECTION;
		}
		else if (childTagName == "groupTarget") {
			newGoal.goalType = GOAL_TYPE_GROUP_TARGET;
		}
		else if (childTagName == "idle") {
			newGoal.goalType = GOAL_TYPE_IDLE;
		}
		else if (childTagName == "seekAxisAlignedBoxRegion")
		{
			newGoal.goalType = GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL;
		}
		else {
			throw GenericException("Unexpected tag <" + childTagName + "> found on line " + toString(child->Row()) + " while parsing goal sequence.\n");
		}

		ticpp::Iterator<ticpp::Element> goalSpecs;
		for (goalSpecs = goalSpecs.begin(&(*child)); goalSpecs != goalSpecs.end(); goalSpecs++ ) {
			std::string specName = goalSpecs->Value();

			if (specName == "targetLocation") {
				_getXYZOrRandomFromXMLElement(&(*goalSpecs), newGoal.targetLocation, newGoal.targetIsRandom);
			}
			else if (specName == "random") {
				if (goalSpecs->GetText() == "true") {
					newGoal.targetIsRandom = true;
				}
			}
			else if (specName == "targetDirection") {
				_getXYZOrRandomFromXMLElement(&(*goalSpecs), newGoal.targetDirection, newGoal.targetIsRandom);
			}
			else if (specName == "targetName") {
				goalSpecs->GetText(&newGoal.targetName);
			}
			else if (specName == "timeDuration") {
				goalSpecs->GetText(&newGoal.timeDuration);
			}
			else if (specName == "type") {
				goalSpecs->GetText(&newGoal.flowType);
			}
			else if (specName == "desiredSpeed") {
				goalSpecs->GetText(&newGoal.desiredSpeed);
			}
			else if (specName == "goalRegionBounds")
			{
				newGoal.targetRegion = _getBoundsFromXMLElement(&(*goalSpecs));
			}
			else if (specName == "Behaviour")
			{
				// std::cout << "Found a fresh behaviour" << std::endl;
				// goalSpecs->GetText(&newGoal.desiredSpeed);
				Behaviour * behave = new Behaviour();
				_getBehaviorFromXMLElement(&(*goalSpecs), (behave));
				newGoal.targetBehaviour.setSteeringAlg(behave->getSteeringAlg());
				int j;
				for ( j =0; j < behave->getParameters().size(); j++)
				{
					newGoal.targetBehaviour.addParameter( behave->getParameters().at(j) );
				}
				/*
				std::cout << "steering algorithm for behaviour is " << behave->getSteeringAlg() << std::endl;
				int q;
				for (q=0; q < behave->getParameters().size(); q++ )
				{
					std::cout << " Parameter" << std::endl;
					std::cout << "\tkey: " << behave->getParameters().at(q).key
							<< " value " << behave->getParameters().at(q).value << std::endl;
				}
				*/
			}
		}

		goals.push_back(newGoal);

	}

}

/*
 * Parses out the Behaviour information of something like
 * 	<Behaviour>
		<SteeringAlgorithm>
		pprAI
		</SteeringAlgorithm>
		<Parameters>
		<parameter>
			<key>max_speed</key>
			<value>4.73</value>
		</parameter>
		<parameter>
			<key>typical_speed</key>
			<value>2.73</value>
		</parameter>
		</Parameters>
	</Behaviour>
 */
void TestCaseReaderPrivate::_getBehaviorFromXMLElement(const ticpp::Element * subRoot, Behaviour * behavior)
{
	ticpp::Iterator<ticpp::Element> behaviourSpecs;
	for (behaviourSpecs = behaviourSpecs.begin(subRoot);
			behaviourSpecs != behaviourSpecs.end(); behaviourSpecs++ )
	{
		std::string behaveName = behaviourSpecs->Value();
		// std::cout << "element name found " << behaveName << std::endl;
		if (behaveName == "SteeringAlgorithm")
		{
			// _getXYZOrRandomFromXMLElement(&(*goalSpecs), newGoal.targetLocation, newGoal.targetIsRandom);
			std::string tmpS;
			behaviourSpecs->GetText(&tmpS);
			behavior->setSteeringAlg(tmpS);
		}
		else if (behaveName == "Parameters")
		{
			ticpp::Iterator<ticpp::Element> param;
			for (param = param.begin(&(*behaviourSpecs));
						param != param.end(); param++ )
			{
				std::string paramString = param->Value();
				BehaviourParameter behaviourParam;
				// std::cout << " " << paramString << std::endl;

				ticpp::Iterator<ticpp::Element> paramSpecs;
				for (paramSpecs = paramSpecs.begin(&(*param));
							paramSpecs != paramSpecs.end(); paramSpecs++ )
				{
					std::string paramEl = paramSpecs->Value();
					// std::cout << "\t" << paramEl << std::endl;
					if ( paramEl == "key" )
					{
						paramSpecs->GetText(& behaviourParam.key);
					}
					else if ( paramEl == "value")
					{
						paramSpecs->GetText(& behaviourParam.value);
					}

				}

				behavior->addParameter(behaviourParam);
			}
		}
	}

}

void TestCaseReaderPrivate::_parseInitialConditions(const ticpp::Element * subRoot, RawAgentInfo & newAgent)
{
	ticpp::Iterator<ticpp::Element> child;
	newAgent.colorSet = false;
	for (child = child.begin(subRoot); child != child.end(); child++ ) {

		std::string childTagName = child->Value();

		// NOTE: in the following code, '&' and '*' do not cancel each other out;
		// its the address of the iterator's current content.
		if (childTagName == "speed") {
			child->GetText(&newAgent.speed);
		}
		else if (childTagName == "radius") {
			child->GetText(&newAgent.radius);
		}
		else if (childTagName == "position") {
			_getXYZOrRandomFromXMLElement(&(*child), newAgent.position, newAgent.isPositionRandom);
		}
		else if (childTagName == "direction") {
			_getXYZOrRandomFromXMLElement(&(*child), newAgent.direction, newAgent.isDirectionRandom);
		}
		else if (childTagName == "color")
		{
			_getXYZOrRandomFromXMLElement(&(*child), newAgent.color, newAgent.isColorRandom);
			newAgent.colorSet=true;
		}
		#ifdef VARIABLE_SPAWN_TIME
		else if(childTagName == "spawn_time"){
			child->GetText(&newAgent.spawnTime);
		}
		else if(childTagName == "despawn_time"){
			child->GetText(&newAgent.despawnTime);
		}
		#endif
	}
}





void TestCaseReaderPrivate::_initObstacleInitialConditions( BoxObstacleInitialConditions & o, const AxisAlignedBox & bounds )
{
	o.xmin = bounds.xmin;
	o.xmax = bounds.xmax;
	o.ymin = bounds.ymin;
	o.ymax = bounds.ymax;
	o.zmin = bounds.zmin;
	o.zmax = bounds.zmax;
}


void TestCaseReaderPrivate::_initAgentInitialConditions( AgentInitialConditions & a, const RawAgentInfo & agent )
{

	// just a safety check;
	// positions should be initialized before you reach this code.
	// at that time you can explicitly set this to false.
	assert(agent.isPositionRandom == false);

	a.name = agent.name;
	a.position = agent.position;
	a.colorSet = agent.colorSet;
	a.color = agent.color;

	if (agent.isDirectionRandom) {
		// choose a random number;
		float randomAngle = (float)_randomNumberGenerator.rand( 2.0f * M_PI );
		a.direction = Vector( cosf(randomAngle),  0.0f,  sinf(randomAngle) );
	}
	else {
		a.direction = agent.direction;
	}
	a.randBox = agent.regionBounds;

	a.radius = agent.radius;
	a.speed = agent.speed;
	a.goals = agent.goals;  // note, this is a STL vector being copied into another STL vector.
}

void TestCaseReaderPrivate::_initAgentEmitterInitialConditions( AgentInitialConditions & a, const RawAgentInfo & agent )
{

	// just a safety check;
	// positions should be initialized before you reach this code.
	// at that time you can explicitly set this to false.
	assert(agent.isPositionRandom == false);

	a.name = agent.name;
	a.position = agent.position;
	a.colorSet = agent.colorSet;
	a.color = agent.color;

	if (agent.isDirectionRandom) {
		// choose a random number;
		float randomAngle = (float)_randomNumberGenerator.rand( 2.0f * M_PI );
		a.direction = Vector( cosf(randomAngle),  0.0f,  sinf(randomAngle) );
	}
	else {
		a.direction = agent.direction;
	}
	a.randBox = agent.regionBounds;

	a.radius = agent.radius;
	a.speed = agent.speed;
	a.goals = agent.goals;  // note, this is a STL vector being copied into another STL vector.
}
