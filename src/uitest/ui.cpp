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












void ui::UiInteractable::update() {
    UiElement::update();

    for (auto &interact : _interactables) {
        interact.isHovered = false;
    }
}

bool ui::UiInteractable::interactInput(int index, Interactable &interact) {
    vec2 pos = _globalPos + interact.pos;

    interact.isHovered =
        mousePos.x > pos.x && mousePos.x < pos.x + interact.size.x &&
        mousePos.y > pos.y && mousePos.y < pos.y + interact.size.y;
    
    if (interact.isHovered && mousePressed(MOUSE_BUTTON_LEFT)) {
        activeElement = shared_from_this();
        _activeInteractable = index;
        event(index, EVENT_PRESS);
    }

    if (mouseReleased(MOUSE_BUTTON_LEFT) && activeElement.get() == this && _activeInteractable == index) {
        activeElement = nullptr;
        if (interact.isHovered) {
            event(index, EVENT_CLICK);
        }
        event(index, EVENT_RELEASE);
    }

    return interact.isHovered;
}

bool ui::UiInteractable::handleInput() {
    bool isActive = activeElement.get() == this;
    if (isActive) {
        interactInput(_activeInteractable, _interactables[_activeInteractable]);
        return true;
    }

    int i = 0;
    for (auto it = _interactables.begin(); it != _interactables.end(); it++) {
        if (interactInput(i, *it)) {
            return true;
        }

        i++;
    }

    return UiElement::handleInput();
}

















ui::UiButton::UiButton(const std::string &text) {
    this->text = text;
    fontSize = 18.f;
    pos = Vector2(0.f, 0.f);
    size = Vector2(
        Fonts::rainworld->getTextWidth(this->text, fontSize) + 10.0,
        20.0f
    );

    _interactables.push_back(Interactable( vec2(), vec2() ));
}

void ui::UiButton::update() {
    UiInteractable::update();
    _interactables[0].size = size;
}

void ui::UiButton::event(int index, EventKind event) {
    if (event == EVENT_CLICK) {
        clickHandler(signal);
    }
}

void drawRect(ui::vec2 pos, ui::vec2 size) {
    Draw::begin(Draw::PrimitiveType::QUADS);
    Draw::vertex(pos);
    Draw::vertex(pos + Vector2(size.x, 0.f));
    Draw::vertex(pos + Vector2(size.x, size.y));
    Draw::vertex(pos + Vector2(0.f, size.y));
    Draw::end();
}

void ui::UiButton::draw() const {
    if (_interactables[0].isHovered || activeElement.get() == this) {
        Draw::color(0.6, 0.6, 0.6);
    } else {
        Draw::color(1, 1, 1);
    }

    drawRect(_globalPos, size);

    Draw::color(1, 0, 0);
    auto textWidth = Fonts::rainworld->getTextWidth(this->text, fontSize);
    Vector2 center = _globalPos + (size - Vector2(textWidth, -fontSize)) * 0.5f;

    Fonts::rainworld->writeCentered(text, center.x, center.y, fontSize, 0);
}









ui::UiHSplit::UiHSplit() {
    split_pos = 40.0f;
    dragging = false;
    _interactables.push_back(Interactable( vec2(0.0f, 0.0f), vec2(0.0f, 0.0f) ));
}

void ui::UiHSplit::update() {
    const float split_width = 8.0f;
    _interactables[0].pos.x = split_pos - split_width / 2.f;
    _interactables[0].size.x = split_width;
    _interactables[0].size.y = size.y;

    if (children().size() == 2) {
        auto &first = children()[0];
        auto &second = children()[1];

        first->pos = vec2(0.0f, 0.0f);
        first->size = vec2(split_pos - split_width / 2 - 4.f, size.y);

        second->pos = vec2(split_pos + split_width / 2 + 4.f, 0.0f);
        second->size = vec2(size.x - second->pos.x, size.y);
    }

    UiInteractable::update();
}

bool ui::UiHSplit::handleInput() {
    if (UiInteractable::handleInput()) {
        if (dragging) split_pos = mousePos.x - _globalPos.x;
        return true;
    }

    return false;
}

void ui::UiHSplit::event(int index, EventKind ev) {
    switch (ev) {
        case UiInteractable::EVENT_PRESS:
            dragging = true;
            break;

        case UiInteractable::EVENT_RELEASE:
            dragging = false;
            break;

        default: break;
    }
}

void ui::UiHSplit::draw() const {
    UiInteractable::draw();
    Draw::color(0.5f, 0.5f, 0.5f);
    drawRect(_globalPos + _interactables[0].pos, _interactables[0].size);
}