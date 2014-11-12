//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file XMLParser.cpp
/// @brief Implements the Util::XMLParser functionality

#include <iostream>
#include <fstream>
#include <assert.h>

#include "tinyxml/ticpp.h"
#include "util/Misc.h"
#include "util/XMLParser.h"
#include "util/GenericException.h"

using namespace std;
using namespace Util;




XMLTag::XMLTag(const std::string & tagName, const std::string & description, XMLTagTypeEnum tagDataType, void * dataTarget, XMLParserCallbackInterface * callbackInterface)
{
	_tagName = tagName;
	_description = description;
	_tagDataType = tagDataType;
	_target = dataTarget;
	_callbackInterface = callbackInterface;
	_tagMap.clear();
}


XMLTag * XMLTag::createChildTag(const std::string & childTagName, const std::string & description, XMLTagTypeEnum tagDataType, void * dataTarget, XMLParserCallbackInterface * callbackInterface)
{
	// child tag should not exist yet for this parent.
	if (_tagMap.find(childTagName) != _tagMap.end()) {
		throw GenericException("Cannot create a duplicate child tag <" + childTagName + "> for the parent tag <" + _tagName + ">.\n");
	}

	// create the child
	XMLTag * child = new XMLTag(childTagName, description, tagDataType, dataTarget, callbackInterface);

	// add the tag to the list - the following STL notation actually creates the new entry if it doesn't exist already, which is the case here.
	_tagMap[childTagName] = child;

	return child;
}


void XMLTag::addChildTag(XMLTag * existingTag)
{
	std::string childTagName = existingTag->getTagName();

	// child tag should not exist yet for this parent.
	if (_tagMap.find(childTagName) != _tagMap.end()) {
		throw GenericException("Cannot add a duplicate child tag <" + childTagName + "> for the parent tag <" + _tagName + ">.\n");
	}

	// add the tag to the list - the following STL notation actually creates the new entry if it doesn't exist already, which is the case here.
	_tagMap[childTagName] = existingTag;
}


XMLTag * XMLTag::getChildTag(const std::string & childTagName)
{
	std::map<std::string,XMLTag*>::iterator iter;
	iter = _tagMap.find(childTagName);
	if (iter == _tagMap.end()) {
		return NULL;
	}
	else {
		return (*iter).second;
	}
}


void XMLTag::parse(const ticpp::Element * subRoot, bool throwIfUnrecognizedTag )
{
	if (_callbackInterface != NULL) _callbackInterface->startElement(this, subRoot);

	//
	// if a tag is a "container", then we won't parse any data from it.
	// if a tag is not a container, we will not recursively try to parse it.
	// this limits the possible XML that we can parse, but we can still
	// support a very functional subset of XML.
	//

	if (_tagDataType != XML_DATA_TYPE_CONTAINER) {
		switch (_tagDataType) {
			case XML_DATA_TYPE_NO_DATA:
				// nothing to do;
				break;
			case XML_DATA_TYPE_SIGNED_INT:
				{
					int integerValue;
					subRoot->GetText(&integerValue);
					if (_target != NULL) *((int*)(_target)) = integerValue;
				}
				break;
			case XML_DATA_TYPE_UNSIGNED_INT:
				{
					unsigned int unsignedValue;
					subRoot->GetText(&unsignedValue);
					if (_target != NULL) *((unsigned int*)(_target)) = unsignedValue;
				}
				break;
			case XML_DATA_TYPE_FLOAT:
				{
					float floatValue;
					subRoot->GetText(&floatValue);
					if (_target != NULL) *((float*)(_target)) = floatValue;
				}
				break;
			case XML_DATA_TYPE_DOUBLE:
				{
					double doubleValue;
					subRoot->GetText(&doubleValue);
					if (_target != NULL) *((double*)(_target)) = doubleValue;
				}
				break;
			case XML_DATA_TYPE_STRING:
				{
					std::string str = subRoot->GetText(false);
					if (_target != NULL) *((std::string*)(_target)) = str;
				}
				break;
			case XML_DATA_TYPE_BOOLEAN:
				{
					bool booleanValue;
					if (subRoot->GetText() == "true") {
						booleanValue = true;
					}
					else if (subRoot->GetText() == "false") {
						booleanValue = false;
					}
					else {
						throw GenericException("invalid boolean value for the <" + _tagName + "> tag, use string literals \"true\" or \"false\".");
					}
					if (_target != NULL) *((bool*)(_target)) = booleanValue;
				}
				break;
			case XML_DATA_TYPE_XYZ:
				{
					ticpp::Iterator<ticpp::Element> childElem;
					float vec[3];
					for (childElem = childElem.begin(subRoot); childElem != childElem.end(); ++childElem ) {
						if (childElem->Value() == "x") {
							childElem->GetText(&vec[0]);
						}
						else if (childElem->Value() == "y") {
							childElem->GetText(&vec[1]);
						}
						else if (childElem->Value() == "z") {
							childElem->GetText(&vec[2]);
						}
						else {
							throw GenericException("invalid tag " + childElem->Value() + " for a 3-element array; use \"<x>\", \"<y>\", or \"<z>\"");
						}
					}
					if (_target != NULL) {
						((float*)(_target))[0] = vec[0];
						((float*)(_target))[1] = vec[1];
						((float*)(_target))[2] = vec[2];
					}
				}
				break;
			case XML_DATA_TYPE_RGB:
				{
					ticpp::Iterator<ticpp::Element> childElem;
					float color[3];
					for (childElem = childElem.begin(subRoot); childElem != childElem.end(); ++childElem ) {
						if (childElem->Value() == "r") {
							childElem->GetText(&color[0]);
						}
						else if (childElem->Value() == "g") {
							childElem->GetText(&color[1]);
						}
						else if (childElem->Value() == "b") {
							childElem->GetText(&color[2]);
						}
						else {
							throw GenericException("invalid tag " + childElem->Value() + " for a 3-element array; use \"<r>\", \"<g>\", or \"<b>\"");
						}
					}
					if (_target != NULL) {
						((float*)(_target))[0] = color[0];
						((float*)(_target))[1] = color[1];
						((float*)(_target))[2] = color[2];
					}
				}
				break;
			case XML_DATA_TYPE_BOUNDING_BOX:
				{
					ticpp::Iterator<ticpp::Element> childElem;
					float bounds[6];
					for (childElem = childElem.begin(subRoot); childElem != childElem.end(); ++childElem ) {
						if (childElem->Value() == "xmin") {
							childElem->GetText(&bounds[0]);
						}
						else if (childElem->Value() == "xmax") {
							childElem->GetText(&bounds[1]);
						}
						else if (childElem->Value() == "ymin") {
							childElem->GetText(&bounds[2]);
						}
						else if (childElem->Value() == "ymax") {
							childElem->GetText(&bounds[3]);
						}
						else if (childElem->Value() == "zmin") {
							childElem->GetText(&bounds[4]);
						}
						else if (childElem->Value() == "zmax") {
							childElem->GetText(&bounds[5]);
						}
					}
					if (_target != NULL) {
						((float*)(_target))[0] = bounds[0];
						((float*)(_target))[1] = bounds[1];
						((float*)(_target))[2] = bounds[2];
						((float*)(_target))[3] = bounds[3];
						((float*)(_target))[4] = bounds[4];
						((float*)(_target))[5] = bounds[5];
					}
				}
				break;
			default:
				std::cerr << "ERROR: XMLTag::parse(): Never expected to reach here.  There is probably a simple control-flow bug.\n";
				break;
		}
	}
	else if (_tagDataType == XML_DATA_TYPE_CONTAINER) {
		//
		// iterate over the children tags in the xml DOM
		//
		ticpp::Iterator<ticpp::Element> child;
		for (child = child.begin(subRoot); child != child.end(); ++child ) {
			std::string nextTagFromFile = child->Value();

			std::map<std::string, XMLTag*>::iterator iter;
			iter = _tagMap.find(nextTagFromFile);
			if (iter != _tagMap.end()) {
				XMLTag * subTag = (*iter).second;
				subTag->parse(&(*child), throwIfUnrecognizedTag); // & and * do not cancel each other out here; its the address of the iterator's current content.
			}
			else {
				if ((!_callbackInterface) && ( throwIfUnrecognizedTag )) {
					throw GenericException("Unexpected tag <" + nextTagFromFile + "> found on line " + toString(child->Row()) + "\n");
			    }
			}
		}
	}
	else {
		// do not expect to get here in code.
		assert(false);
	}

	if (_callbackInterface != NULL) _callbackInterface->endElement(this, subRoot);
}


void XMLTag::outputFormattedXML(std::ostream & out, const std::string & indentation )
{
	if (_tagDataType == XML_DATA_TYPE_CONTAINER) {
		//
		// for a container tag, output it and recursively output its children.
		//
		out << indentation << "<" << _tagName << ">\n";
		if (_description != "") out << indentation << "<!--\n" << indentation << _description << "\n" << indentation << "-->\n";

		// if there is a callback interface, then use that first.
		if (_callbackInterface) _callbackInterface->outputFormattedXML(out, indentation + "    ");

		// if there is not a callback interface, then iterate over its children.
		std::map<std::string, XMLTag*>::iterator  tagIter;

		// first, recursively output all data tags (skipping container tags)
		for (tagIter = _tagMap.begin(); tagIter != _tagMap.end(); ++tagIter) {
			XMLTag * child = (*tagIter).second;
			if (child->getDataType() != XML_DATA_TYPE_CONTAINER) {
				child->outputFormattedXML(out, indentation + "    ");
			}
		}

		// after that, now recursively output all container tags.
		for (tagIter = _tagMap.begin(); tagIter != _tagMap.end(); ++tagIter) {
			XMLTag * child = (*tagIter).second;
			if (child->getDataType() == XML_DATA_TYPE_CONTAINER) {
				child->outputFormattedXML(out, indentation + "    ");
			}
		}

		out << indentation << "</" << _tagName << ">\n";
	}
	else {
		//
		// for a data tag, output its XML based on its data type.
		//
		if ((_target == NULL) && (_tagDataType != XML_DATA_TYPE_NO_DATA)) {
			throw GenericException("ERROR: currently do not support enough features when writing XML to a file. for now, expecting _target to be non-NULL.\n");
		}
		if (_description != "") out << indentation << "<!-- " << _description << " -->\n";

		out << indentation << "<" << _tagName << ">";
		switch (_tagDataType) {
			case XML_DATA_TYPE_NO_DATA:
				// output nothing
				break;
			case XML_DATA_TYPE_SIGNED_INT:
				out << *((int*)_target);
				break;
			case XML_DATA_TYPE_UNSIGNED_INT:
				out << *((unsigned int*)_target);
				break;
			case XML_DATA_TYPE_FLOAT:
				out << *((float*)_target);
				break;
			case XML_DATA_TYPE_DOUBLE:
				out << *((double*)_target);
				break;
			case XML_DATA_TYPE_STRING:
				out << *((std::string*)_target);
				break;
			case XML_DATA_TYPE_BOOLEAN:
				out << (*((bool*)_target) ? "true" : "false");
				break;
			case XML_DATA_TYPE_XYZ:
				out << " ";
				out << "<x>" << ((float*)_target)[0] << "</x> ";
				out << "<y>" << ((float*)_target)[1] << "</y> ";
				out << "<z>" << ((float*)_target)[2] << "</z> ";
				break;
			case XML_DATA_TYPE_RGB:
				out << " ";
				out << "<r>" << ((float*)_target)[0] << "</r> ";
				out << "<g>" << ((float*)_target)[1] << "</g> ";
				out << "<b>" << ((float*)_target)[2] << "</b> ";
				break;
			case XML_DATA_TYPE_BOUNDING_BOX:
				break;
			default:
				throw GenericException("Never expected to reach here.  There is probably a simple control-flow bug.\n");
		}
		out << "</" << _tagName << ">\n";
	}
}



XMLTag * XMLParser::createRootTag(const std::string & rootTagName, const std::string & description, XMLTagTypeEnum tagDataType, void * dataTarget, XMLParserCallbackInterface * callbackInterface)
{
	if (_rootTag != NULL) {
		throw GenericException("createRootTag was called twice; can only create a root tag once.\n");
	}
	_rootTag = new XMLTag(rootTagName, description, tagDataType, dataTarget, callbackInterface);
	return _rootTag;
}


void XMLParser::parseXMLFile(const std::string & filename, bool throwIfUnrecognizedTag )
{
	ticpp::Document doc(filename);
	doc.LoadFile();

	ticpp::Element * rootElem = doc.FirstChildElement();
	std::string rootTagNameFromFile = rootElem->Value();

	// if the root tag doesn't match our expected root tag, its an error.
	if (_rootTag->getTagName() != rootTagNameFromFile) {
		throw GenericException("XML file " + filename + " has an incorrect root tag, <" + rootTagNameFromFile + ">, expected <" + _rootTag->getTagName() + ">.\n"
			+ "perhaps it is not the file you intended to parse.");
	}
	
	// recursively parse each tag.
	_rootTag->parse( rootElem, throwIfUnrecognizedTag );
}

void XMLParser::writeXMLFile(const std::string & filename)
{
	std::ofstream outFile;
	outFile.open(filename.c_str());
	_rootTag->outputFormattedXML(outFile, "");
}
