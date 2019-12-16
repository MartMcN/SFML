#pragma once


class logline {
public:
	uint32_t id;
	uint32_t timestamp;
	std::string logentry_text;
	uint16_t logentry_text_len;
	uint16_t logentry_text_len_dspy;


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