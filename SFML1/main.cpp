#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include "logLine.cpp"

#define DEFAULT_WINDOW_WIDTH_PX		640
#define DEFAULT_WINDOW_HEIGHT_PX	480

#define TEXT_CHAR_HEIGHT			20
#define TEXT_LINE_SEPERATOR_PX		6

#define DARK_GREY	0xA9A9A9FF


struct _line_points {
	sf::Vertex line[2];
};


// Global Data
logbook test_book;
sf::Font text_font;
std::vector<sf::Text> lineTextVec;
std::vector<_line_points> lineTextSepVec;
uint32_t dispLogentryCount = 0;
uint16_t screen_width_px = DEFAULT_WINDOW_WIDTH_PX;
uint16_t screen_height_px = DEFAULT_WINDOW_HEIGHT_PX;

void getDataKeyboard()
{
	char textline[240];

	// Add some dumy text
	test_book.addlogEntry("This is the first go");
	test_book.addlogEntry("Whats this");
	test_book.addlogEntry("On no Tony!!!!");

	while (1)
	{
		std::cin.getline(textline, 240);
		test_book.addlogEntry(textline);
	}
}

void updateDisplayData()
{
	auto plog = test_book.getlogBook();

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


int main()
{
	// Thread to get text from the console
	std::thread workerThread(getDataKeyboard);


	// Create the Render Window
	sf::RenderWindow window(sf::VideoMode(screen_width_px, screen_height_px), "LogLine");
	sf::View view = window.getDefaultView();

	// Load the font
	if (!text_font.loadFromFile("CourierPrime-Regular.ttf"))
	{
		return 0;
	}


	updateDisplayData();

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
				updateDisplayData();

				// More text on a button push.
				//auto it = lineTextVec.rbegin();
				//sf::Text lineText = *it;
				//auto position = lineText.getPosition();
				//position.y += TEXT_CHAR_HEIGHT;
				//lineText.setPosition(position);
				//lineText.setString("NEW");
				//lineTextVec.push_back(lineText);

				//// Add a seperator line
				//_line_points lineTextSep;
				//lineTextSep.line[0] = { sf::Vector2f(0, position.y + TEXT_CHAR_HEIGHT + TEXT_LINE_SEPERATOR_PX), sf::Color(DARK_GREY) };
				//lineTextSep.line[1] = { sf::Vector2f(screen_width_px, position.y + TEXT_CHAR_HEIGHT + TEXT_LINE_SEPERATOR_PX), sf::Color(DARK_GREY) };
				//lineTextSepVec.push_back(lineTextSep);

				break;
			}
			}
		}

		window.clear(sf::Color::Black);

		// Draw Line Seperators
		for (auto it = lineTextSepVec.begin(); it != lineTextSepVec.end(); ++it)
		{
			_line_points seperator_line;
			seperator_line = *it;
			seperator_line.line[1].position.x = screen_width_px;
			window.draw(seperator_line.line, 2, sf::Lines);
		}

		// Draw Text
		for (auto it = lineTextVec.begin(); it != lineTextVec.end(); ++it)
		{
			window.draw(*it);
		}

		window.display();
	}

	return 0;
}

