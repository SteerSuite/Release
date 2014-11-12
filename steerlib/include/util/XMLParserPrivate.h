//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_XML_PARSER_PRIVATE_H__
#define __UTIL_XML_PARSER_PRIVATE_H__

/// @file XMLParserPrivate.h
/// @brief Declares private helper functionality for parsing XML documents

#include <string>
#include <map>
#include "tinyxml/ticpp.h"

#include "Globals.h"


#ifdef _WIN32
// on win32, there is an unfortunate conflict between exporting symbols for a
// dynamic/shared library and STL code.  A good document describing the problem
// in detail is http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
// the "least evil" solution is just to simply ignore this warning.
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

namespace Util {


	/**
	 * @brief Enum describing the data types supported by the XMLParser.
	 *
	 * This enum is used to identify the data type of an element used for parsing in the XMLParser. 
	 *
	 */
	enum XMLTagTypeEnum {
		/// An XML Element only with children elements, no data.
		XML_DATA_TYPE_CONTAINER,
		/// An XML Element that is a leaf node with no data and no children elements.
		XML_DATA_TYPE_NO_DATA,
		/// An XML Element that has a signed integer value.
		XML_DATA_TYPE_SIGNED_INT,
		/// An XML Element that has an unsigned integer value.
		XML_DATA_TYPE_UNSIGNED_INT,
		/// An XML Element that has a floating-point value.
		XML_DATA_TYPE_FLOAT,
		/// An XML Element that has a double-precision value.
		XML_DATA_TYPE_DOUBLE,
		/// An XML Element that has a string value.
		XML_DATA_TYPE_STRING,
		/// An XML Element that has a boolean value.
		XML_DATA_TYPE_BOOLEAN,
		/// An XML Element that has three children tags, &lt;x&gt;, &lt;y&gt;, and &lt;z&gt;, each of which has a floating-point value.
		XML_DATA_TYPE_XYZ,
		/// An XML Element that has three children tags, &lt;r&gt;, &lt;g&gt;, and &lt;b&gt;, each of which has a floating-point value.
		XML_DATA_TYPE_RGB,
		/// An XML Element that has six children tags, &lt;xmin&gt;, &lt;xmax&gt;, &lt;ymin&gt;, &lt;ymax&gt;, &lt;zmin&gt;, &lt;zmax&gt;, each of which has a floating-point value.
		XML_DATA_TYPE_BOUNDING_BOX 
	};

	// forward declarations
	class UTIL_API XMLTag;
	class UTIL_API XMLParserCallbackInterface;


	/**
	 * @brief Protected data for an XMLTag object.
	 */
	class UTIL_API XMLTagPrivate {
	protected:
		/// The name of the tag.
		std::string _tagName;
		/// A description of the tag, that could perhaps become an XML comment annotating the tag.
		std::string _description;
		/// The data type of the tag.
		XMLTagTypeEnum _tagDataType;
		/// An optional pointer, if it is not NULL, then the referenced data will be initialized when this tag is encountered during parsing.
		void * _target;
		/// An STL map containing children XMLTag objects, using the children's tag name as the index/key.
		std::map<std::string, Util::XMLTag*> _tagMap;
		/// An optional pointer, if it is not NULL, then XMLParserCallbackInterface::startElement() and XMLParserCallbackInterface::endElement() will be called before and after parsing the element (respectively).
		XMLParserCallbackInterface * _callbackInterface;
	};

	/**
	 * @brief Protected data for the XMLParser class.
	 */
	class UTIL_API XMLParserPrivate {
	protected:
		/// The XMLTag representing the root element of the XML file
		XMLTag * _rootTag;
	};


} // end namespace Util


#ifdef _WIN32
#pragma warning( pop )
#endif

#endif
