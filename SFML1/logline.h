#pragma once


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

class logbook
{
	std::vector<logline>* plogBook = NULL;

public:
	logbook();
	~logbook();

	std::vector<logline>* getlogBook();
	void addlogEntry(std::string logtext);

};