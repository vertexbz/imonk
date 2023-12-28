#pragma once

#include "../Display/View.hpp"

namespace Widget {
class BaseWidget;
}

namespace View {
    class BaseView : public Display::View {
    public:
        typedef uint8_t SlotId;

        Display::Color background() override {
            return 0x333333;
        }

        virtual Widget::BaseWidget* slot(SlotId id) { return nullptr; }
        virtual SlotId slots() { return 0; }
    };
}
