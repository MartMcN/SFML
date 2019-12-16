#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

#include "logline.h"

#define DEFAULT_WINDOW_WIDTH_PX		640
#define DEFAULT_WINDOW_HEIGHT_PX	480

#define TEXT_CHAR_HEIGHT			20
#define TEXT_LINE_SEPERATOR_PX		6

#define DARK_GREY	0xA9A9A9FF

#define TELETYPE_SPEED_CHAR_MS		27		// 100Ms per character 10 chars a second

struct _line_points {
	sf::Vertex line[2];
};

// Global Data
sf::Font text_font;


std::mutex mtx;

class displayMngr {
private:
	uint32_t dispLogentryCount;
	uint16_t screen_width_px;
	uint16_t screen_height_px;

public:
	displayMngr();
	~displayMngr();
	void addNewDisplayData();
	bool checkDisplayUpdated();
	void setViewSize(uint16_t x, uint16_t y);
	void teleType();

public:
	logbook* test_book;
	sf::Clock clockTele;
	sf::Time timeTele;
	std::vector<sf::Text> lineTextVec;
	std::vector<_line_points> lineTextSepVec;

};


displayMngr::displayMngr()
{
	dispLogentryCount = 0;
	screen_width_px = DEFAULT_WINDOW_WIDTH_PX;
	screen_height_px = DEFAULT_WINDOW_HEIGHT_PX;

	test_book = new logbook();
}

displayMngr::~displayMngr()
{
}

void displayMngr::addNewDisplayData()
{
	auto plog = test_book->getlogBook();
	timeTele = clockTele.getElapsedTime();

	// Init the display data for each new entry in the log
	//for (dispLogentryCount; dispLogentryCount < plog->size(); dispLogentryCount++)
	for (dispLogentryCount = 0; dispLogentryCount < plog->size(); dispLogentryCount++)
	{
		sf::Text lineText;
		_line_points lineTextSep;

		// Set the initial time stamp
		//plog->at(dispLogentryCount).timestamp = timeTele.asMilliseconds();

		// Text
		lineText.setFont(text_font);
		lineText.setFillColor(sf::Color::Green);
		lineText.setCharacterSize(TEXT_CHAR_HEIGHT);
		lineText.setPosition(0, (float)dispLogentryCount * TEXT_CHAR_HEIGHT);

		// Display string consist of the first char only
		std::string logentry_partial_text = plog->at(dispLogentryCount).logentry_text;
		uint16_t logentry_partial_size = plog->at(dispLogentryCount).logentry_text_len_dspy;
		lineText.setString(logentry_partial_text.substr(0, logentry_partial_size));
		lineTextVec.push_back(lineText);


		lineTextSep.line[0].position = sf::Vector2f((float)0, (float)(((dispLogentryCount + 1) * TEXT_CHAR_HEIGHT) + TEXT_LINE_SEPERATOR_PX));
		lineTextSep.line[0].color = sf::Color(DARK_GREY);
		lineTextSep.line[1].position = sf::Vector2f((float)screen_width_px, (float)(((dispLogentryCount + 1) * TEXT_CHAR_HEIGHT) + TEXT_LINE_SEPERATOR_PX));
		lineTextSep.line[1].color = sf::Color(DARK_GREY);

		lineTextSepVec.push_back(lineTextSep);
	}
}

bool displayMngr::checkDisplayUpdated()
{
	auto plog = test_book->getlogBook();

	if (dispLogentryCount < plog->size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void displayMngr::setViewSize(uint16_t x, uint16_t y)
{
	screen_width_px = x;
	screen_height_px = y;

}

volatile uint32_t timestamp;
void displayMngr::teleType()
{
	uint16_t logentry_partial_size;
	uint16_t logentry_size;
	volatile uint32_t timestamp;
	uint32_t current_timestamp;

	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	// Get the time
	timeTele = clockTele.getElapsedTime();
	current_timestamp = timeTele.asMilliseconds();
	std::cout << "CurrentTime:" << current_timestamp << std::endl;

	// Get pointer to the log book
	auto plog = test_book->getlogBook();

	// Loop over all log entries to see if the text needs updating.
	mtx.lock();
	for (uint32_t i=0 ; i < plog->size(); ++i)
	{
		logentry_size = plog->at(i).logentry_text_len;
		logentry_partial_size = plog->at(i).logentry_text_len_dspy;
		
		// Check if all characters are upto date.
		if (logentry_partial_size < logentry_size)
		{
			timestamp = plog->at(i).timestamp + TELETYPE_SPEED_CHAR_MS;
			std::cout << "timestamp:" << timestamp << std::endl;

			// TODO - potentially more than one char
			if(current_timestamp > timestamp)
			{
				std::cout << "UP!" << std::endl;
				plog->at(i).logentry_text_len_dspy++;

				plog->at(i).timestamp = current_timestamp;
			}


		}
	}
	mtx.unlock();
}



// Thread for debug data generation.
void test_data_thread(logbook* ptrlogbook)
{
	uint16_t x = 0;

	while (1)
	{ 
		std::string buffer = "OH NO TONY!!!!!!! YIPPY KIYEAH MFFFFF yippee ki yay Construct mutex (public member function )";
		buffer += std::to_string(x);
		mtx.lock();
		ptrlogbook->addlogEntry(buffer);
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1336));
		x++;

	}
}

int main()
{
	// Load the fonts
	if (!text_font.loadFromFile("CourierPrime-Regular.ttf"))
	{
		return 0;
	}

	// Create the Render Window
	sf::RenderWindow window(sf::VideoMode(DEFAULT_WINDOW_WIDTH_PX, DEFAULT_WINDOW_HEIGHT_PX), "LogLine");
	sf::View view = window.getDefaultView();

	// Create the display Manager
	displayMngr screenTextLines;

	// Thread to generate some test data
	std::thread worktask(test_data_thread, screenTextLines.test_book);

	sf::Clock clock; // starts the clock
	sf::Time el = clock.getElapsedTime();

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}

			case sf::Event::Resized:
			{
				sf::FloatRect visable_area(0, 0, (float)event.size.width, (float)event.size.height);
				screenTextLines.setViewSize(event.size.width, event.size.height);
				window.setView(sf::View(visable_area));
				break;
			}
			case sf::Event::KeyReleased:
			{
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				std::cout << event.mouseButton.x << " " << event.mouseButton.y << std::endl;
				break;
			}
			}
		}

		// Check for new display data
		//if (screenTextLines.checkDisplayUpdated())
		{
			// Add new data
			screenTextLines.addNewDisplayData();
		}

		screenTextLines.teleType();

		window.clear(sf::Color::Black);

		mtx.lock();

		// Draw Line Seperators
		for (auto it = screenTextLines.lineTextSepVec.begin(); it != screenTextLines.lineTextSepVec.end(); ++it)
		{
			_line_points seperator_line;
			seperator_line = *it;
			window.draw(seperator_line.line, 2, sf::Lines);
		}

		// Draw Text
		for (auto it = screenTextLines.lineTextVec.begin(); it != screenTextLines.lineTextVec.end(); ++it)
		{
			window.draw(*it);
		}

		mtx.unlock();
		window.display();


	}

	return 0;
}

