//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __UTIL_COMMAND_LINE_OPTIONS_PARSER_H__
#define __UTIL_COMMAND_LINE_OPTIONS_PARSER_H__

/// @file CommandLineParser.h
/// @brief Declares a helpful Util::CommandLineParser class.

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

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
	 * @brief Enum describing the data types supported by the CommandLineParser.
	 *
	 * This enum is used to identify the data type of an element used for parsing in the CommandLineParser.
	 * Note that options with multiple args of the same data type are supported by specifying how many
	 * args an option consumes, so this enum only needs to support simple data types.
	 *
	 */
	enum CommandLineOptionTypeEnum {
		/// A command line option with no extra args; useful for true/false flags.
		OPTION_DATA_TYPE_NO_DATA,
		/// A command line option that has signed integer values.
		OPTION_DATA_TYPE_SIGNED_INT,
		/// A command line option that has unsigned signed integer values.
		OPTION_DATA_TYPE_UNSIGNED_INT,
		/// A command line option that has floating-point values.
		OPTION_DATA_TYPE_FLOAT,
		/// A command line option that has double-precision values.
		OPTION_DATA_TYPE_DOUBLE,
		/// A command line option that has string values.
		OPTION_DATA_TYPE_STRING,
		/// A command line option that can occur multiple times, treated as strings.
		OPTION_DATA_TYPE_MULTI_INSTANCE_STRING,
	};

	/**
	 * @brief Easy-to-use class for parsing command line arguments.
	 *
	 * This class allows you to specify the command line options that you expect to see when parsing a command line.
	 * To use this class, simply specify each arg you want to parse using #addOption().
	 *
	 * This parser only initializes options that have a "flag" (i.e., usually starts with a hyphen "-" symbol) followed by
	 * a specific number of "values" that follow.
	 * If you want to allow "required args" that do not have an associated option flag, use the leftoverArgs parameter
	 * from the #parse() function, which returns a list of args that were not recognized as options.
	 *
	 * Consider using Boost options parser if you need more elaborate functionality.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Simply call #addOption() for each option you would like to parse.  In this function, you describe: an
	 * optional target variable you would like to be initialized (use NULL if there is no target), how many args
	 * the option will consume, the target data type, and an optional secondary boolean target.
	 *
	 * For example:
	 * \code
	 * // initialize data to hard-coded defaults, just in case the option is not used on the command line.
	 * Point myPoint = Point(0.0f, 0.0f, 0.0f);
	 * bool pointSpecified = false;
	 * bool verbose = false;
	 *
	 * Util::CommandLineParser opts;
	 * opts.addOption( "-position", (void*)&myPoint, OPTION_DATA_TYPE_FLOAT, 3, &pointSpecified, true);
	 * opts.addOption( "-verbose", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &verbose, true);
	 * opts.addOption( "-silent", NULL, OPTION_DATA_TYPE_NO_DATA, 0, &verbose, false);
	 *
	 * // this version of parse() throws a Util::GenericException if there are unrecognized args.
	 * opts.parse(argc, argv, true, true);
	 * \endcode
	 *
	 * Once you have specified all the options you want to support, call #parse(), specifying the argc and argv, and
	 * all your target variables will be initialized when parse returns.  Optionally, you can constrain the number of
	 * "non-option" args that are expected, so that #parse() will thrown an exception if there are too few or too many
	 * non-option args.
	 */
	class UTIL_API CommandLineParser {
	public:
		CommandLineParser();
		
		/// @brief Adds an option.
		///
		/// The data type passed by target should be a pointer to the same type specified by optionDataType. For 
		/// nulti-instance data types (i.e. the option can occur multiple times, each time adding to a list of values), the target
		/// should be a pointer to an STL vector of the corresponding data type.
		void addOption(const std::string & option, void * target, CommandLineOptionTypeEnum optionDataType,
		    unsigned int numArgs=1, bool * flagTarget = NULL, bool flagIfOptionSpecified=true);

		/// Parses the command line; places unrecognized args into leftoverArgs.
		void parse( int argc, char** argv, bool skipFirstArg, std::vector<char*> &leftoverArgs);
		/// Parses the command line, allowing no leftover unparsed args;  if there are any unknonwn options and throwExceptionIfUnrecognizedArgs is true, then an exception will be thrown.
		void parse( int argc, char** argv, bool skipFirstArg, bool throwExceptionIfUnrecognizedArgs);

	protected:

		struct OptionInfo {
			CommandLineOptionTypeEnum dataType;
			unsigned int numArgs;
			void * target;
			bool * flagTarget;
			bool flagIfOptionSpecified;
		};


		std::vector<std::string> _nonOptionArgs;
		/// Maps the option string (e.g. "-version") to the option's information, including the target variable to initialize.
		std::map<std::string, CommandLineParser::OptionInfo> _options;
	};

} // end namespace Util

#ifdef _WIN32
#pragma warning( pop )
#endif


#endif
