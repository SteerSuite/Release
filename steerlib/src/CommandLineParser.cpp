//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file CommandLineParser.cpp
/// @brief Implements the Util::CommandLineParser class

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "util/CommandLineParser.h"
#include "util/GenericException.h"

using namespace Util;


//
// constructor
//
CommandLineParser::CommandLineParser()
{
	_nonOptionArgs.clear();
	_options.clear();
}


void CommandLineParser::addOption(const std::string & option, void * target, CommandLineOptionTypeEnum optionDataType,
		    unsigned int numArgs, bool * flagTarget, bool flagIfOptionSpecified)
{
	CommandLineParser::OptionInfo op;
	op.dataType = optionDataType;
	op.target = target;
	op.numArgs = numArgs;
	op.flagTarget = flagTarget;
	op.flagIfOptionSpecified = flagIfOptionSpecified;

	_options[option] = op;
}


//
// parse(): the real guts of the CommandLineParser is here.
//
void CommandLineParser::parse( int argc, char** argv, bool skipFirstArg, std::vector<char*> &leftoverArgs )
{
	//
	// some declarations and initialization
	//
	CommandLineParser::OptionInfo optionToParse;
	std::map<std::string, CommandLineParser::OptionInfo>::iterator op;
	int i = 0;
	if (skipFirstArg) i = 1;
	leftoverArgs.clear();


	// iterate over every option; internally this loop will increment 'i' as needed up to the next option.
	while (i < argc) {
		op = _options.find(argv[i]);
		if (op == _options.end()) {
			//
			// in this case, we did not find the option, so add it to the leftoverArgs (and check for errors later).
			//
			leftoverArgs.push_back(argv[i]);
		}
		else {
			//
			// in this case, we did find the option, so parse it.
			//
			optionToParse = (*op).second;
			char * indicator;
			switch (optionToParse.dataType) {
				case OPTION_DATA_TYPE_SIGNED_INT:
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// convert it with error checking into a signed integer
						int integerValue = strtol(argv[i],&indicator,0);
						if ((indicator==argv[i]) || (indicator != argv[i] + strlen(argv[i]))) {
							// TODO: this still doesnt reliably detect extremely large integer values as an error.
							throw GenericException("value for the " + (*op).first + " option is not a valid integer.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) ((int*)(optionToParse.target))[n] = integerValue;
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_UNSIGNED_INT:
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// convert it with error checking into a signed integer
						unsigned int unsignedValue = strtoul(argv[i],&indicator,0);
						if ((indicator==argv[i]) || (indicator != argv[i] + strlen(argv[i]))) {
							// TODO: this still doesnt reliably detect extremely large integer values as an error.
							throw GenericException("ERROR: value for the " + (*op).first + " option is not a valid unsigned integer.");
						}
						if (argv[i][0] == '-') {
							// if conversion was successful, but the arg was originally negative, don't allow it
							throw GenericException("ERROR: value for the " + (*op).first + " option cannot be negative.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) ((unsigned int*)(optionToParse.target))[n] = unsignedValue;
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_FLOAT:
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// convert it with error checking into a signed integer
						double floatValue = strtod(argv[i],&indicator); // will be cast to a float
						if ((indicator==argv[i]) || (indicator != argv[i] + strlen(argv[i]))) {
							// TODO: this still doesnt reliably detect extremely large integer values as an error.
							throw GenericException("ERROR: value for the " + (*op).first + " option is not a valid floating-point number.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) ((float*)(optionToParse.target))[n] = (float)floatValue;
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_DOUBLE:
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// convert it with error checking into a signed integer
						double doubleValue = strtod(argv[i],&indicator); // will be cast to a float
						if ((indicator==argv[i]) || (indicator != argv[i] + strlen(argv[i]))) {
							// TODO: this still doesnt reliably detect extremely large integer values as an error.
							throw GenericException("ERROR: value for the " + (*op).first + " option is not a valid double-precision floating-point number.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) ((double*)(optionToParse.target))[n] = doubleValue;
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_STRING:
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) ((std::string*)(optionToParse.target))[n] = std::string(argv[i]);
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_MULTI_INSTANCE_STRING:
					if (optionToParse.numArgs != 1) {
						throw GenericException("multi-instance options with multiple args are not supported by this version of CommandLineParser.");
					}
					// even though numArgs is constrained to 1, just keeping the for-loop for uniformity and convenience anyway,
					// to minimize chances of creating bugs when changing this code later.
					for (unsigned int n=0; n<optionToParse.numArgs; n++) {
						i++;
						// make sure another arg exists
						if (i >= argc) {
							throw GenericException("expected a value for the " + (*op).first + " option.");
						}
						// then initialize the target with this value.
						if (optionToParse.target != NULL) (*((std::vector<std::string>*)(optionToParse.target))).push_back(std::string(argv[i]));
					}
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				case OPTION_DATA_TYPE_NO_DATA:
					if (optionToParse.flagTarget != NULL)  *(optionToParse.flagTarget) = optionToParse.flagIfOptionSpecified;
					break;
				default:
					std::cerr << "CommandLineParser::parse(): Never expected to reach here.  There is probably a simple control-flow bug.\n";
					assert(false);
					break;
			}
		}
		i++;
	}
}


void CommandLineParser::parse( int argc, char** argv, bool skipFirstArg, bool throwExceptionIfUnrecognizedArgs)
{
	std::vector<char*> ignoredLeftoverArgs;
	parse(argc, argv, skipFirstArg, ignoredLeftoverArgs);

	if (throwExceptionIfUnrecognizedArgs && (ignoredLeftoverArgs.size() > 0)) {
		throw GenericException("Unrecognized option: " + std::string(ignoredLeftoverArgs[0]));
	}
}



