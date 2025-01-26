#include "../gl.h"

#include <iostream>
#include <cmath>

#include "../Constants.hpp"
#include "../Window.hpp"
#include "../Utils.hpp"
#include "../Texture.hpp"
#include "../math/Vector.hpp"
#include "../math/Rect.hpp"
#include "../font/Fonts.hpp"
#include "../Theme.hpp"
#include "../Draw.hpp"
#include "ui.hpp"

#define TEXTURE_PATH (BASE_PATH + "assets/")

void applyFrustumToOrthographic(Vector2 position, float rotation, Vector2 scale, float left, float right, float bottom, float top, float nearVal, float farVal) {
	left *= scale.x;
	right *= scale.x;
	bottom *= scale.y;
	top *= scale.y;

	left += position.x;
	right += position.x;
	bottom += position.y;
	top += position.y;

	float cosRot = std::cos(rotation);
	float sinRot = std::sin(rotation);

	GLfloat rotationMatrix[16] = {
		cosRot,  sinRot, 0, 0,
		-sinRot, cosRot, 0, 0,
		0,       0,      1, 0,
		0,       0,      0, 1
	};

	Draw::matrixMode(Draw::PROJECTION);
	Draw::loadIdentity();
	Draw::ortho(left, right, bottom, top, nearVal, farVal);

	Draw::multMatrix(Matrix4(rotationMatrix));
}

void applyFrustumToOrthographic(Vector2 position, float rotation, Vector2 scale) {
	applyFrustumToOrthographic(position, rotation, scale, -1.0f, 1.0f, -1.0f, 1.0f, 0.000f, 100.0f);
}

static void clickHandler(const std::string &sig) {
	std::cout << sig << "\n";
}

int main() {
	loadTheme();
	
	Window *window = new Window(960, 720);
	window->setIcon(TEXTURE_PATH + "MainIcon.png");
	window->setTitle("FloodForge World Editor");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}

	Fonts::init();
	//Popups::init();
	Draw::init();

    ui::Ref<ui::UiElement> root = ui::root = std::make_shared<ui::UiElement>();

	{
		auto button = ui::Button::create("Button 1");
		button->signal = "BUT1";
		button->clickHandler = clickHandler;
		root->add(button);
	}

	{
		auto button = ui::Button::create("Button 2");
		button->signal = "BUT2";
		button->clickHandler = clickHandler;
		root->add(button);

		button->pos.x = 50.0f;
	}

	while (window->isOpen()) {
		glfwPollEvents();

		window->ensureFullscreen();

		int width;
		int height;
		glfwGetWindowSize(window->getGLFWWindow(), &width, &height);

		double mouseX, mouseY;
		ui::beginFrame();
		ui::setMousePos({ (float)window->GetMouse()->X(), height - (float)window->GetMouse()->Y() });
		ui::setMouseButton(ui::MOUSE_BUTTON_LEFT, glfwGetMouseButton(window->getGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		ui::setMouseButton(ui::MOUSE_BUTTON_MIDDLE, glfwGetMouseButton(window->getGLFWWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
		ui::setMouseButton(ui::MOUSE_BUTTON_RIGHT, glfwGetMouseButton(window->getGLFWWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
		ui::update();

		glViewport(0, 0, width, height);
        Draw::matrixMode(Draw::PROJECTION);
        Draw::loadIdentity();
        Draw::ortho(0.0f, (float)width, 0.0f, (float)height, 0.0f, 1.0f);

		window->clear();
		glDisable(GL_DEPTH_TEST);

		ui::draw();

		//glViewport(offsetX, offsetY, size, size);
		window->render();
	}

	Fonts::cleanup();
	//MenuItems::cleanup();
	//Shaders::cleanup();
	Draw::cleanup();

	return 0;
}