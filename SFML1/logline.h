#pragma once


class logline {
public:
	uint32_t logline_id;
	uint32_t logline_timestamp;
	std::string logline_text;
	uint16_t logline_len;
	uint16_t logline_len_dspy;

	logline();
	logline(std::string log_text);
	logline(std::string log_text, bool notele);
	~logline();
};

class logbook
{
private:
	logline* plogHeader = NULL;
	std::vector<logline>* plogBook = NULL;

public:
	logbook();
	~logbook();

	std::vector<logline>* getLogBook();
	void createLogBookEntry(std::string logtext);

	logline* getLogBookHeader();
	void createLogBookHeader(std::string headertext);
};