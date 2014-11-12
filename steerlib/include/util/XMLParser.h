//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_XML_PARSER_H__
#define __UTIL_XML_PARSER_H__

/// @file XMLParser.h
/// @brief Declares a helpful Util::XMLParser class and related data structures.

#include "Globals.h"
#include "util/XMLParserPrivate.h"

namespace Util {

	// forward declaration
	class UTIL_API XMLTag;  

	/**
	 * @brief Inherit and implement this virtual interface if you want to receive callbacks from the XMLParser.
	 *
	 * It is not necessary to implement the callback interface if you only have a simple XML file with once
	 * instance per tag.  In that case, you can simply bind your data using the features of XMLTag and XMLParser.
	 * This class is helpful for more complex cases, for example, when you have multiple instances of a tag, or
	 * if you want to parse the tinyxml DOM yourself for particular elements.
	 *
	 * @todo
	 *  - add more functionality in the callback that allows writing multi-instance tags.
	 */
	class UTIL_API XMLParserCallbackInterface {
	public:
		virtual ~XMLParserCallbackInterface() { }
		/// Called by XMLTag::parse() when the appropriate element (given here as subRoot) is beginning to be parsed.
		virtual void startElement( Util::XMLTag * tag, const ticpp::Element * subRoot ) { }
		/// Called by XMLTag::parse() when the appropriate element (given here as subRoot) is to be parsed.
		virtual void endElement( Util::XMLTag * tag, const ticpp::Element * subRoot ) { }
		/// Called by XMLTag::outputFormattedXML() when this tag/sub-tree should be serialized into XML.
		virtual void outputFormattedXML(std::ostream &out, const std::string & indentation) { }
	};


	/**
	 * @brief Stores user-specified meta-data for each tag when parsing an XML file.
	 *
	 * To programatically define the format of your XML file, use this class functionality.
	 *
	 * @todo
	 *  - implement proper refcount/deallocation somehow
	 *
	 * @see
	 *  - the XMLParser documentation that describes how to use these classes.
	 */
	class UTIL_API XMLTag : public XMLTagPrivate {
	public:
		XMLTag(const std::string & tagName, const std::string & description, XMLTagTypeEnum tagDataType=XML_DATA_TYPE_CONTAINER, void * dataTarget=NULL, XMLParserCallbackInterface * callbackInterface=NULL );

		/// @name Defining the XML format to be parsed
		/// @brief Use these functions to create a DOM-like tree of XMLTag objects, which defines the XML tags that should be parsed.
		//@{
		/// Creates a new XMLTag object, adds it as a child tag of this XMLTag, and returns a reference to the new child.
		XMLTag * createChildTag(const std::string & childTagName, const std::string & description, XMLTagTypeEnum tagDataType=XML_DATA_TYPE_CONTAINER, void * dataTarget=NULL, XMLParserCallbackInterface * callbackInterface=NULL );
		/// Adds an existing XMLTag object as a child tag of this XMLTag.
		void addChildTag(XMLTag * existingTag);
		/// Returns a reference to an existing child tag keyed/indexed by the child tag's name.
		XMLTag * getChildTag(const std::string & childTagName);
		//@}

		/// Returns the name of this XMLTag.
		std::string getTagName() { return _tagName; }
		/// Returns a description string of this XMLTag.
		std::string getDescription() { return _description; }
		/// Returns the data type of this XMLTag.
		XMLTagTypeEnum getDataType() { return _tagDataType; }
		/// Binds a new target to the tag, so that next time the tag occurs, it can initialize a new target.
		void setNewTarget(void * newTarget) { _target = newTarget; }
		/// Recursively parse this XMLTag and all its children.
		void parse(const ticpp::Element * subRoot, bool throwIfUnrecognizedTag );
		/// Recursively output properly-indented XML associated with this element and all its children.  Currently only supports XML elements that occur exactly once.
		void outputFormattedXML(std::ostream &out, const std::string & indentation);
	};


	/**
	 * @brief The public interface for a simple XML parser; can bind data directly, and/or can use callbacks in the XMLParserCallbackInterface.
	 *
	 * This is a very simplified XML parser that works on top of tinyxml.  The main 
	 * reason someone may want to use this class over other existing parsers
	 * is to quickly get started parsing a particular XML format.  All you need 
	 * to do is programmatically describe the XML tags you expect to parse, and the XMLParser
	 * can automatically initialize your data for you, or give you callbacks (or both).
	 *
	 * Specifically, this class supports flexible data binding and/or callbacks during parsing.  Note 
	 * that this is not a streaming callback-oriented parser; tinyxml actually creates the full DOM 
	 * first, and the (optional) user's callback functions actually receive the entire tinyxml DOM 
	 * sub-tree associated with the element being parsed.
	 *
	 *
	 * <h3> How to use this XML parser </h3>
	 *
	 * The elements in the XML file you wish to parse are specified programmatically using XMLTag objects.
	 * You can (1) allocate XMLTags and then add them as children to other tags, or you can (2) create 
	 * child XMLTag objects directly from existing XMLTag objects.
	 *
	 * The function args the user must specify when creating/adding XMLTag objects have three
	 * purposes:
	 *  - <b>Required parsing information:</b> The name of the tag, a description, and the data type
	 *    of the corresponding element are necessary for the XMLParser to understand the XML structure
	 *    that should be parsed.
	 *  - <b>Optional data binding:</b> For a few simple data types and some complex data types, 
	 *    you can optionally specify a pointer reference to your own variables, which will be
	 *    automatically initialized.  Note, this is only useful for parsing elements that can
	 *    only appear once.  For XML tag elements that may appear many times, (for example, a new
	 *    agent should be allocated and initialized for every &lt;agent&gt; tag) use callbacks.
	 *  - <b>Optional callback interface:</b> You can specify an XMLCallbackInterface that is
	 *    called when the particular XML element starts and ends.  The entire DOM sub-tree associated
	 *    with the element would be passed to the callback, which can be very useful or entirely ignored,
	 *    depending on your parsing needs.
	 *   
	 * Once you have finished specifying the structure you wish to parse, simply call parseXMLFile() to
	 * parse a specific XML file.
	 * 
	 */
	class UTIL_API XMLParser : public XMLParserPrivate {
	public:
		XMLParser() { _rootTag = NULL; }
		/// Creates the root element of the XML that is expected to be parsed, and returns a reference to that tag.
		XMLTag * createRootTag(const std::string & rootTagName, const std::string & description, XMLTagTypeEnum tagDataType=XML_DATA_TYPE_CONTAINER, void * dataTarget=NULL, XMLParserCallbackInterface * callbackInterface=NULL );
		/// Returns an XMLTag reference to the root tag.
		XMLTag * getRootTag() { return _rootTag; }
		/// Parses an XML file, assumes that the structure of the XML was already programmatically created by the user, with a tree of XMLTag objects.
		void parseXMLFile(const std::string & filename, bool throwIfUnrecognizedTag );
		/// Outputs properly indented XML of the entire specified XML structure.  Currently has limited support.
		void writeXMLFile(const std::string & filename);
	};


} // end namespace Util

#endif
