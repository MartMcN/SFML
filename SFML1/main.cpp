#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <windows.h>
#include "logline.h"

// TODO
// Handle line spacing for various fonts
// Support Debug Tracing
// Column Support
// Header Row
// Scroll Bar
// Only draw whats needs on the screen.
// Stop scrolling when nothing on screen.
// Why the console
// Crash on exit


#define DEFAULT_WINDOW_WIDTH_PX		640
#define DEFAULT_WINDOW_HEIGHT_PX	480

#define TEXT_CHAR_HEIGHT_PX			20
#define TEXT_LINE_SEPERATOR_PX		6
#define MOUSE_WHEEL_MOVE_PX			20

#define DARK_GREY					0xA9A9A9FF

#define TELETYPE_SPEED_CHAR_MS		27

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
	uint16_t getViewSizeX();
	uint16_t getViewSizeY();
	void teleType();

public:
	logbook* test_book;
	sf::Clock clockTele;
	sf::Time timeTele;
	std::vector<sf::Text> lineTextVec;
	std::vector<_line_points> lineTextSepVec;
	int32_t screen_view_y_postion;

};


displayMngr::displayMngr()
{
	dispLogentryCount = 0;
	screen_view_y_postion = 0;
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
	
	lineTextVec.clear();
	lineTextSepVec.clear();
	for (dispLogentryCount = 0; dispLogentryCount < plog->size(); dispLogentryCount++)
	{
		sf::Text lineText;
		_line_points lineTextSep;

		// Text
		lineText.setFont(text_font);
		lineText.setFillColor(sf::Color::Green);
		lineText.setCharacterSize(TEXT_CHAR_HEIGHT_PX);
		lineText.setPosition(0, (float)dispLogentryCount * TEXT_CHAR_HEIGHT_PX);

		// Display string consist of the first char only
		std::string logline_partial_text = plog->at(dispLogentryCount).logline_text;
		uint16_t logline_partial_size = plog->at(dispLogentryCount).logline_len_dspy;
		lineText.setString(logline_partial_text.substr(0, logline_partial_size));
		lineTextVec.push_back(lineText);

		lineTextSep.line[0].position = sf::Vector2f((float)0, (float)(((dispLogentryCount + 1) * TEXT_CHAR_HEIGHT_PX) + TEXT_LINE_SEPERATOR_PX));
		lineTextSep.line[0].color = sf::Color(DARK_GREY);
		lineTextSep.line[1].position = sf::Vector2f((float)screen_width_px, (float)(((dispLogentryCount + 1) * TEXT_CHAR_HEIGHT_PX) + TEXT_LINE_SEPERATOR_PX));
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

uint16_t displayMngr::getViewSizeX()
{
	return screen_width_px;
}

uint16_t displayMngr::getViewSizeY()
{
	return screen_height_px;
}


volatile uint32_t timestamp;
void displayMngr::teleType()
{
	uint16_t logline_partial_size;
	uint16_t logline_size;
	volatile uint32_t timestamp;
	uint32_t current_timestamp;

	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

	// Get the time
	timeTele = clockTele.getElapsedTime();
	current_timestamp = timeTele.asMilliseconds();
	//std::cout << "CurrentTime:" << current_timestamp << std::endl;

	// Get pointer to the log book
	auto plog = test_book->getlogBook();

	// Loop over all log book entries to see if the text needs updating.
	mtx.lock();
	for (uint32_t i=0 ; i < plog->size(); ++i)
	{
		logline_size = plog->at(i).logline_len;
		logline_partial_size = plog->at(i).logline_len_dspy;
		
		// Check if all characters are upto date.
		if (logline_partial_size < logline_size)
		{
			timestamp = plog->at(i).logline_timestamp + TELETYPE_SPEED_CHAR_MS;
			//std::cout << "timestamp:" << timestamp << std::endl;

			// TODO - potentially more than one char
			if(current_timestamp > timestamp)
			{
				//std::cout << "UP!" << std::endl;
				plog->at(i).logline_len_dspy++;

				plog->at(i).logline_timestamp = current_timestamp;
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
		ptrlogbook->createLogBookEntry(buffer);
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1336));
		x++;

	}
}

int main()
{
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);

	// Load the fonts
	if (!text_font.loadFromFile("CourierPrime-Regular.ttf"))
	//if (!text_font.loadFromFile("IBMPlexMono-Regular.ttf"))
	{
		return 0;
	}

	// Create the Render Window
	sf::RenderWindow window(sf::VideoMode(DEFAULT_WINDOW_WIDTH_PX, DEFAULT_WINDOW_HEIGHT_PX), "LogLine");
	sf::View view = window.getDefaultView();

	// Create the display Manager
	displayMngr screenTextLines;

	//std::this_thread::sleep_for(std::chrono::milliseconds(1336));

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
				sf::FloatRect visable_area(0, (float)screenTextLines.screen_view_y_postion, (float)event.size.width, (float)event.size.height);
				screenTextLines.setViewSize(event.size.width, event.size.height);
				window.setView(sf::View(visable_area));
				break;
			}
			case sf::Event::KeyReleased:
			{
				break;
			}
			case sf::Event::MouseWheelScrolled:
			{

				if (event.mouseWheelScroll.delta > 0)
				{
					screenTextLines.screen_view_y_postion -= MOUSE_WHEEL_MOVE_PX;

					if (screenTextLines.screen_view_y_postion < 0)
						screenTextLines.screen_view_y_postion = 0;
				}
				else
					screenTextLines.screen_view_y_postion += MOUSE_WHEEL_MOVE_PX;



				sf::FloatRect visable_area(0, (float)screenTextLines.screen_view_y_postion,(float)screenTextLines.getViewSizeX(), (float)screenTextLines.getViewSizeY());
				window.setView(sf::View(visable_area));
				//std::cout << screenTextLines.screen_view_y_postion << "\n";
				break;
			}

			}
		}


		// Update the text
		screenTextLines.teleType();

		// Draw the screen
		screenTextLines.addNewDisplayData();

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

	return EXIT_SUCCESS;

}

