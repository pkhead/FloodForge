#include <vector>
#include <memory>
#include <functional>

#include "../math/Vector.hpp"

namespace ui {
    class UiElement;

    template <typename T = UiElement>
    using Ref = std::shared_ptr<T>;

    using vec2 = Vector2f;

    enum MouseButton {
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_RIGHT
    };

    extern void beginFrame();
    extern void setMousePos(vec2 mousePos);
    extern void setMouseButton(MouseButton button, bool isDown);
    extern void update();
    extern void draw();
    extern Ref<> root;

    /**
     * @brief Base class for all UI elements
     */
    class UiElement : public std::enable_shared_from_this<UiElement> {
    private:
        std::weak_ptr<UiElement> _parent;
        std::vector<Ref<>> _children;

        void remove(UiElement *element);
    
    protected:
        vec2 _globalPos;
    
    public:
        UiElement();
        virtual ~UiElement();

        vec2 pos;
        vec2 size;

        bool useAnchors;
        int anchorX;
        int anchorY;

        inline const Ref<> parent() const {
            return _parent.lock();
        }

        inline const std::vector<Ref<>>& children() const {
            return _children;
        }

        void add(Ref<> element);
        void remove(Ref<> &element);

        virtual void draw() const;
        virtual void update();
        virtual bool handleInput();

        friend class UiContainer;
    };

    class Button : public UiElement {
    private:
        bool _isHovered;
        
    public:
        std::string text;
        std::string signal;

        float fontSize;
        Button(const std::string &text);

        std::function<void(const std::string &signal)> clickHandler;

        inline static std::shared_ptr<Button> create(const std::string &text) {
            return std::make_shared<Button>(text);
        }

        void draw() const override;
        bool handleInput() override;
        void update() override;
    };
}