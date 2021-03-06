#include <iostream>
#include <string>
#include <vector>
#include "logline.h"

//
// Log Line
//
logline::logline()
{
	logline_id = 0;
	logline_timestamp = 0;
	logline_len = 0;
	logline_len_dspy;
	logline_text = "";
}

logline::logline(std::string log_text)
{
	// TODO the string needs to be verified
	logline_id = 0;
	logline_timestamp = 0;
	logline_len = log_text.size();
	logline_len_dspy = 1;				// Start off displaying the first character only
	logline_text = log_text;
}

logline::logline(std::string log_text, bool notele)
{
	// TODO the string needs to be verified
	logline_id = 0;
	logline_timestamp = 0;
	logline_len = log_text.size();
	logline_len_dspy = logline_len;
	logline_text = log_text;
}

logline::~logline()
{

}

//
// Log Book
//
logbook::logbook(void)
{
	plogBook = new std::vector<logline>;
	plogHeader = new logline;
}

logbook::~logbook(void)
{
	delete plogBook;
}

std::vector<logline>* logbook::getLogBook()
{
	return plogBook;
}

void logbook::createLogBookEntry(std::string logtext)
{
	logline newLogEntry(logtext);

	plogBook->push_back(newLogEntry);
}

logline* logbook::getLogBookHeader()
{
	return plogHeader;
}

void logbook::createLogBookHeader(std::string headertext)
{
	logline newLogHeader(headertext, true);

	*plogHeader = newLogHeader;
}