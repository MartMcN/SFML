#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <windows.h>
#include "fps.hpp"
#include "logline.h"

// TODO
// Handle line spacing for various fonts
// Support Debug Tracing
// Column Support
// Scroll Bar
// Cross compile
// How to get back the resource window
// error handling
// overflow checks
// text anamations sparkle

// Learning:
// thread detach?

#define DEFAULT_WINDOW_WIDTH_PX		640
#define DEFAULT_WINDOW_HEIGHT_PX	480

#define TEXT_CHAR_HEIGHT_PX			20
#define TEXT_LINE_SEPERATOR_PX		6
#define MOUSE_WHEEL_MOVE_PX			20

#define VERTICAL_SCROLL_LINE_LIMIT	10

#define DARK_GREY					0xA9A9A9FF

#define TELETYPE_SPEED_CHAR_MS		27
#define TEST_GENERATION_SPEED		5

struct _line_points {
	sf::Vertex line[2];
};

// Global Data
sf::Font text_font;
std::mutex mtx;


class displayMngr {
private:
	uint16_t screen_width_px;
	uint16_t screen_height_px;
	uint32_t screen_vertical_position;

public:
	displayMngr();
	~displayMngr();
	void updateDataForView();

	void setViewSize(uint16_t x, uint16_t y);
	uint16_t getViewSizeX();
	uint16_t getViewSizeY();
	void updateViewVerticalPosition(int32_t);
	uint32_t getViewVerticalPosition();
	void moveViewVerticalBottom();
	void moveViewVerticalTop();
	
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
	screen_vertical_position = 0;
	screen_width_px = DEFAULT_WINDOW_WIDTH_PX;
	screen_height_px = DEFAULT_WINDOW_HEIGHT_PX;

	test_book = new logbook();
}

displayMngr::~displayMngr()
{
}

void displayMngr::updateDataForView()
{
	uint32_t dispLogindex = 0;

	sf::Text lineText;
	_line_points lineTextSep;

	auto plog = test_book->getLogBook();
	lineTextVec.clear();
	lineTextSepVec.clear();

	// Set the default Text options
	lineText.setFont(text_font);
	lineText.setStyle(sf::Text::Regular);
	lineText.setFillColor(sf::Color::Green);
	lineText.setCharacterSize(TEXT_CHAR_HEIGHT_PX);

	// Postion the header a the top of the view
	auto header = test_book->getLogBookHeader();
	lineText.setPosition(0, (float)getViewVerticalPosition());
	lineText.setString(header->logline_text);
	lineTextVec.push_back(lineText);

	lineTextSep.line[0].position = sf::Vector2f((float)0, (float)(getViewVerticalPosition() + TEXT_CHAR_HEIGHT_PX + TEXT_LINE_SEPERATOR_PX));
	lineTextSep.line[0].color = sf::Color(DARK_GREY);
	lineTextSep.line[1].position = sf::Vector2f((float)screen_width_px, (float)(getViewVerticalPosition() + TEXT_CHAR_HEIGHT_PX + TEXT_LINE_SEPERATOR_PX));
	lineTextSep.line[1].color = sf::Color(DARK_GREY);
	lineTextSepVec.push_back(lineTextSep);

	// Calculate which logline the view starts at
	while ((dispLogindex * TEXT_CHAR_HEIGHT_PX) <= getViewVerticalPosition())
	{
		dispLogindex++;
	}
	//std::cout << "ViewDataUpdate Starts at index " << dispLogindex << std::endl;

	// Calculate view data
	for (dispLogindex; dispLogindex < plog->size(); dispLogindex++)
	{
		// Text
		lineText.setPosition(0, (float)dispLogindex * TEXT_CHAR_HEIGHT_PX);

		// Display string consist of the first char only
		std::string logline_partial_text = plog->at(dispLogindex).logline_text;
		uint16_t logline_partial_size = plog->at(dispLogindex).logline_len_dspy;
		lineText.setString(logline_partial_text.substr(0, logline_partial_size));
		lineTextVec.push_back(lineText);

		lineTextSep.line[0].position = sf::Vector2f((float)0, (float)(((dispLogindex + 1) * TEXT_CHAR_HEIGHT_PX) + TEXT_LINE_SEPERATOR_PX));
		lineTextSep.line[0].color = sf::Color(DARK_GREY);
		lineTextSep.line[1].position = sf::Vector2f((float)screen_width_px, (float)(((dispLogindex + 1) * TEXT_CHAR_HEIGHT_PX) + TEXT_LINE_SEPERATOR_PX));
		lineTextSep.line[1].color = sf::Color(DARK_GREY);

		lineTextSepVec.push_back(lineTextSep);

		// Stop display data calculation when the bottom of the view is reached
		if ((dispLogindex * TEXT_CHAR_HEIGHT_PX) > (getViewVerticalPosition() + getViewSizeY()))
		{
			//std::cout << "ViewDataUpdate Complete at index " << dispLogindex << std::endl;
			break;
		}
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

void displayMngr::updateViewVerticalPosition(int32_t update_postion)
{

	if (update_postion > 0)
	{
		// Limit the scrolling so some log lines remain on the screen
		uint32_t lastlogpostion;

		auto plog = test_book->getLogBook();
		lastlogpostion = plog->size() * TEXT_CHAR_HEIGHT_PX;

		if(lastlogpostion > screen_vertical_position + (VERTICAL_SCROLL_LINE_LIMIT * TEXT_CHAR_HEIGHT_PX) )
			screen_vertical_position += update_postion;
	}
	else
	{
		int32_t position;
		position = screen_vertical_position;
		position += update_postion;

		if (position < 0)
			screen_vertical_position = 0;
		else
			screen_vertical_position = position;
	}
}

void displayMngr::moveViewVerticalBottom()
{
		uint32_t logpostion;

		auto plog = test_book->getLogBook();
		logpostion = (plog->size() * TEXT_CHAR_HEIGHT_PX);

		if (logpostion >= (VERTICAL_SCROLL_LINE_LIMIT * TEXT_CHAR_HEIGHT_PX))
		{
			logpostion -= (VERTICAL_SCROLL_LINE_LIMIT * TEXT_CHAR_HEIGHT_PX);

			if (screen_vertical_position <= logpostion)
				updateViewVerticalPosition(logpostion - screen_vertical_position);
		}
}

void displayMngr::moveViewVerticalTop()
{
	screen_vertical_position = 0;
}

uint32_t displayMngr::getViewVerticalPosition()
{
	return screen_vertical_position;
}

void displayMngr::teleType()
{
	uint16_t logline_partial_size;
	uint16_t logline_size;
	volatile uint32_t timestamp;
	uint32_t current_timestamp;

	// Get the time
	timeTele = clockTele.getElapsedTime();
	current_timestamp = timeTele.asMilliseconds();
	//std::cout << "CurrentTime:" << current_timestamp << std::endl;

	// Get pointer to the log book
	auto plog = test_book->getLogBook();

	// Loop over all log book entries to see if the text needs updating.
	for (uint32_t i=0 ; i < plog->size(); ++i)
	{
		logline_size = plog->at(i).logline_len;
		logline_partial_size = plog->at(i).logline_len_dspy;
		
		// Check if all characters are upto date.
		if (logline_partial_size < logline_size)
		{
			timestamp = plog->at(i).logline_timestamp + TELETYPE_SPEED_CHAR_MS;
			//std::cout << "timestamp:" << timestamp << std::endl;

			// TODO - potentially more than one char should be updated
			if(current_timestamp > timestamp)
			{
				//std::cout << "UP!" << std::endl;
				plog->at(i).logline_len_dspy++;

				plog->at(i).logline_timestamp = current_timestamp;
			}

		}
	}
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
		std::this_thread::sleep_for(std::chrono::milliseconds(TEST_GENERATION_SPEED));
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

	// Main thread
	//std::cout << "Main ID:" << std::hex << std::this_thread::get_id() << std::endl << std::dec;

	// Create the header line
	screenTextLines.test_book->createLogBookHeader("THIS IS THE HEADER");

	// Thread to generate some test data
	std::thread worktask(test_data_thread, screenTextLines.test_book);
	worktask.detach();
	//std::cout << "TreadId:" << std::hex << worktask.get_id() << std::endl << std::dec;

	// Starts the clock
	sf::Clock clock;

	//
	FPS fps_info;

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
				return EXIT_SUCCESS;
			}

			case sf::Event::Resized:
			{
				sf::FloatRect visable_area(0, (float)screenTextLines.getViewVerticalPosition(), (float)event.size.width, (float)event.size.height);
				screenTextLines.setViewSize(event.size.width, event.size.height);
				window.setView(sf::View(visable_area));
				break;
			}
			case sf::Event::KeyPressed:
			{
				sf::Keyboard key;
				if (event.key.code == key.PageDown)
				{
					screenTextLines.moveViewVerticalBottom();
					sf::FloatRect visable_area(0, (float)screenTextLines.getViewVerticalPosition(), (float)screenTextLines.getViewSizeX(), (float)screenTextLines.getViewSizeY());
					window.setView(sf::View(visable_area));
				}
				if (event.key.code == key.PageUp)
				{
					screenTextLines.moveViewVerticalTop();
					sf::FloatRect visable_area(0, (float)screenTextLines.getViewVerticalPosition(), (float)screenTextLines.getViewSizeX(), (float)screenTextLines.getViewSizeY());
					window.setView(sf::View(visable_area));
				}
			}
			case sf::Event::KeyReleased:
			{
				//std::cout << fps_info.getFPS() << std::endl;
				break;
			}
			case sf::Event::MouseWheelScrolled:
			{

				if (event.mouseWheelScroll.delta > 0)
				{
					screenTextLines.updateViewVerticalPosition(-MOUSE_WHEEL_MOVE_PX);
				}
				else
					screenTextLines.updateViewVerticalPosition(MOUSE_WHEEL_MOVE_PX);

				sf::FloatRect visable_area(0, (float)screenTextLines.getViewVerticalPosition(),(float)screenTextLines.getViewSizeX(), (float)screenTextLines.getViewSizeY());
				window.setView(sf::View(visable_area));
				//std::cout << screenTextLines.getViewVerticalPosition() << "\n";
				break;
			}

			}
		}

		mtx.lock();

		// Update the text
		screenTextLines.teleType();

		// Draw the view
		screenTextLines.updateDataForView();

		mtx.unlock();


		window.clear(sf::Color::Black);

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



		window.display();

		fps_info.update();

	}

	return EXIT_SUCCESS;

}

