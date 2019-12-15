#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include "logline.h"

#define DEFAULT_WINDOW_WIDTH_PX		640
#define DEFAULT_WINDOW_HEIGHT_PX	480

#define TEXT_CHAR_HEIGHT			20
#define TEXT_LINE_SEPERATOR_PX		6

#define DARK_GREY	0xA9A9A9FF

struct _line_points {
	sf::Vertex line[2];
};

// Global Data
sf::Font text_font;


class displayMngr {
private:
	logbook *test_book;
	uint32_t dispLogentryCount;
	uint16_t screen_width_px;
	uint16_t screen_height_px;

public:
	displayMngr();
	~displayMngr();
	void updateDisplayData();
	bool checkDisplayUpdated();
	void setViewSize(uint16_t x, uint16_t y);
	void genTestData();

public:
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

void displayMngr::updateDisplayData()
{
	auto plog = test_book->getlogBook();

	// Init the display data for each entry in the log
	for (dispLogentryCount; dispLogentryCount < plog->size(); dispLogentryCount++)
	{
		sf::Text lineText;
		_line_points lineTextSep;

		// Text
		lineText.setFont(text_font);
		lineText.setFillColor(sf::Color::Green);
		lineText.setCharacterSize(TEXT_CHAR_HEIGHT);
		lineText.setPosition(0, (float)dispLogentryCount * TEXT_CHAR_HEIGHT);
		lineText.setString(plog->at(dispLogentryCount).logentry_text);
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


void displayMngr::genTestData()
{
	test_book->addlogEntry("oh no Tony!!!!");	
}



int main()
{

	// Load the fonts
	if (!text_font.loadFromFile("CourierPrime-Regular.ttf"))
	{
		return 0;
	}

	// Create the Render Window
	uint16_t screen_width_px = DEFAULT_WINDOW_WIDTH_PX;
	uint16_t screen_height_px = DEFAULT_WINDOW_HEIGHT_PX;
	sf::RenderWindow window(sf::VideoMode(screen_width_px, screen_height_px), "LogLine");
	sf::View view = window.getDefaultView();

	// Create the display Manager
	displayMngr screenTextLines;
	// Thread generate some test data
	screenTextLines.genTestData();
	screenTextLines.genTestData();
	screenTextLines.genTestData();


	screenTextLines.updateDisplayData();

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
				screen_width_px = event.size.width;
				screen_height_px = event.size.height;
				window.setView(sf::View(visable_area));
				break;
			}
			case sf::Event::KeyReleased:
			{
				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				// 
				std::cout << event.mouseButton.x << " " << event.mouseButton.y << std::endl;

				// Check of there is more entries in the log book.
				if (screenTextLines.checkDisplayUpdated())
				{
					screenTextLines.updateDisplayData();
				}
				break;
			}
			}
		}

		window.clear(sf::Color::Black);

		// Draw Line Seperators
		for (auto it = screenTextLines.lineTextSepVec.begin(); it != screenTextLines.lineTextSepVec.end(); ++it)
		{
			_line_points seperator_line;
			seperator_line = *it;
			seperator_line.line[1].position.x = screen_width_px;
			window.draw(seperator_line.line, 2, sf::Lines);
		}

		// Draw Text
		for (auto it = screenTextLines.lineTextVec.begin(); it != screenTextLines.lineTextVec.end(); ++it)
		{
			window.draw(*it);
		}

		window.display();
	}

	return 0;
}

