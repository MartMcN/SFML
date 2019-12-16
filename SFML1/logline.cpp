#include <iostream>
#include <string>
#include <vector>
#include "logline.h"

//
// Log Line
//
logline::logline()
{
	id = 0;
	timestamp = 0;
	logentry_text = "";
}

logline::logline(std::string log_text)
{
	// TODO verify the text
	logentry_text = log_text;
	logentry_text_len = log_text.size();
	
	// Start off displaying the first character only
	if(logentry_text_len)
		logentry_text_len_dspy = 1;

	std::cout << logentry_text << " " << logentry_text_len << std::endl;
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
}

logbook::~logbook(void)
{
	delete plogBook;
}

std::vector<logline>* logbook::getlogBook()
{
	return plogBook;
}
void logbook::addlogEntry(std::string logtext)
{

	logline newLogEntry(logtext);
	// TODO
	newLogEntry.id = 0;
	newLogEntry.timestamp = 0;
	plogBook->push_back(newLogEntry);
}