#pragma once
#include "_view_.hpp"
#include "../Widget/String.hpp"

namespace View {
class Initial : public BaseView {
    Widget::String line;
    Widget::String line2;
public:
    explicit Initial();

    void render(Display::Painter *painter) override;
};
}
