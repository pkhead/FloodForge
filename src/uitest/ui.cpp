#include <cassert>
#include <algorithm>
#include "ui.hpp"
#include "../Draw.hpp"
#include "../font/Fonts.hpp"

static ui::vec2 mousePos;

static bool mouseButtons[3] = {false, false, false};
static bool lastMouseButtons[3] = {false, false, false};
static bool mouseInputSunk = false;
ui::Ref<> ui::root;

static std::shared_ptr<ui::UiElement> activeElement;

void ui::update() {
    if (!root) return;

    if (activeElement) {
        activeElement->update();
        activeElement->handleInput();
    } else {
        root->update();
        root->handleInput();
    }
}

void ui::draw() {
    if (!root) return;
    root->draw();
}

void ui::beginFrame() {
    memcpy(lastMouseButtons, mouseButtons, sizeof(lastMouseButtons));
    mouseInputSunk = false;
}

void ui::setMousePos(vec2 p_mousePos) {
    mousePos = p_mousePos;
}

void ui::setMouseButton(MouseButton button, bool isDown) {
    assert(button >= 0 && button <= 3);
    mouseButtons[button] = isDown;
}

static bool mouseDown(ui::MouseButton button) {
    return mouseButtons[button];
}

static bool mousePressed(ui::MouseButton button) {
    return mouseButtons[button] && !lastMouseButtons[button];
}

static bool mouseReleased(ui::MouseButton button) {
    return !mouseButtons[button] && lastMouseButtons[button];
}

ui::UiElement::UiElement() {
    useAnchors = false;
    anchorX = 0.0f;
    anchorY = 0.0f;
}

ui::UiElement::~UiElement() {
    if (!_parent.expired()) {
        auto paren = _parent.lock();
        paren->remove(this);
    }
}

void ui::UiElement::update() {
    auto &paren = parent();
    if (paren && useAnchors) {
        if (useAnchors) {
            switch (anchorX) {
                case -1:
                    pos.x = 0.0f;
                    break;

                case 0:
                    pos.x = (paren->size.x - size.x) / 2.0f;
                    break;

                case 1:
                    pos.x = paren->size.x - size.x;
                    break;

                default:
                    assert(false && "invalid anchorX");
            }

            switch (anchorY) {
                case -1:
                    pos.y = 0.0f;
                    break;

                case 0:
                    pos.y = (paren->size.y - size.y) / 2.0f;
                    break;

                case 1:
                    pos.y = paren->size.y - size.y;
                    break;

                default:
                    assert(false && "invalid anchorY");
            }
        }
    }
    
    if (paren) {
        _globalPos = paren->_globalPos + pos;
    } else {
        _globalPos = pos;
    }

    for (auto &child : _children) {
        child->update();
    }
}

void ui::UiElement::draw() const {
    for (auto &child : _children) {
        child->draw();
    }
}

bool ui::UiElement::handleInput() {
    for (auto it = _children.rbegin(); it != _children.rend(); it++) {
        auto &child = *it;
        bool sunk = child->handleInput();

        if (sunk)
            return true;
    }

    return false;
}

void ui::UiElement::add(ui::Ref<> element) {
    assert(element);
    element->_parent = weak_from_this();
    _children.push_back(std::move(element));
}

void ui::UiElement::remove(ui::Ref<> &element) {
    auto it = std::find(_children.begin(), _children.end(), element);
    if (it != _children.end()) {
        _children.erase(it);
    }
}

void ui::UiElement::remove(ui::UiElement *element) {
    for (auto it = _children.begin(); it != _children.end(); it++) {
        if (it->get() == element) {
            _children.erase(it);
            break;
        }
    }
}

ui::Button::Button(const std::string &text) {
    this->text = text;
    fontSize = 18.f;
    _isHovered = false;
    pos = Vector2(0.f, 0.f);
    size = Vector2(
        Fonts::rainworld->getTextWidth(this->text, fontSize) + 10.0,
        20.0f
    );
}

void ui::Button::update() {
    UiElement::update();
    _isHovered = false;
}

bool ui::Button::handleInput() {
    if (UiElement::handleInput()) return true;
    bool isActive = activeElement.get() == this;

    _isHovered =
        mousePos.x > _globalPos.x && mousePos.x < _globalPos.x + size.x &&
        mousePos.y > _globalPos.y && mousePos.y < _globalPos.y + size.y;
    
    if (_isHovered && mousePressed(MOUSE_BUTTON_LEFT)) {
        activeElement = shared_from_this();
    }

    if (mouseReleased(MOUSE_BUTTON_LEFT) && activeElement.get() == this) {
        activeElement = nullptr;
        if (_isHovered && clickHandler) {
            clickHandler(signal);
        }
    }

    return _isHovered;
}

void ui::Button::draw() const {
    Draw::begin(Draw::PrimitiveType::QUADS);

    if (_isHovered || activeElement.get() == this) {
        Draw::color(0.6, 0.6, 0.6);
    } else {
        Draw::color(1, 1, 1);
    }

    Draw::vertex(_globalPos);
    Draw::vertex(_globalPos + Vector2(size.x, 0.f));
    Draw::vertex(_globalPos + Vector2(size.x, size.y));
    Draw::vertex(_globalPos + Vector2(0.f, size.y));
    Draw::end();

    Draw::color(1, 0, 0);
    auto textWidth = Fonts::rainworld->getTextWidth(this->text, fontSize);
    Vector2 center = _globalPos + (size - Vector2(textWidth, -fontSize)) * 0.5f;

    Fonts::rainworld->writeCentered(text, center.x, center.y, fontSize, 0);
}