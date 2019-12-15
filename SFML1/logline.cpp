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
	color = 0;
	logentry_text = "";
}

logline::logline(std::string log_text)
{
	id = 0;
	timestamp = 0;
	color = 0;

	// Need to check verify is aok data
	logentry_text = log_text;
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
	newLogEntry.id = 0;
	newLogEntry.timestamp = 0;
	newLogEntry.color = 0;

	plogBook->push_back(newLogEntry);
}