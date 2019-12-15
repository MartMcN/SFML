#include <string>
#include <vector>


class logline {
public:
	std::uint32_t id;
	std::uint32_t timestamp;
	std::uint32_t color;
	std::string logentry_text;

	logline();
	logline(std::string log_text);
	~logline();
};


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

class logbook
{
	std::vector<logline>* plogBook = NULL;

public:
	logbook();
	~logbook();

	std::vector<logline> *getlogBook();
	void addlogEntry(std::string logtext);

};

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