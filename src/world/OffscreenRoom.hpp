#include "Room.hpp"
#include "../font/Fonts.hpp"

#ifndef OFFSCREEN_ROOM_HPP
#define OFFSCREEN_ROOM_HPP

class OffscreenRoom : public Room {
	public:
		OffscreenRoom(std::string path, std::string name) {
			this->path = path;
			this->roomName = name;

			position = new Vector2(
				0.0f,
				0.0f
			);

			// coord = new Vector2(72 * 0.5, 43 * -0.5);

			width = 72;
			height = 43;

			valid = false;
			
			layer = 0;
			subregion = -1;
			water = -1;

			geometry = nullptr;
			
			tag = "OffscreenRoom";
		}

		~OffscreenRoom() {
		}

		void draw(Vector2 mousePosition, double lineSize) override {
			Draw::color(1.00f, 1.00f, 1.00f);
			fillRect(position.x, position.y, position.x + width, position.y - height);

			Draw::color(0.00f, 0.00f, 0.00f);
			Fonts::rainworld->writeCentred(this->roomName, position.x + (width * 0.5), position.y - (height * 0.5), 5, CENTRE_XY);

			if (inside(mousePosition)) {
				Draw::color(0.00f, 0.75f, 0.00f);
			} else {
				Draw::color(0.75f, 0.75f, 0.75f);
			}
			strokeRect(position.x, position.y, position.x + width, position.y - height);
		}
};

#endif