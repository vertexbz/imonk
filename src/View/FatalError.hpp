#pragma once
#include "../Display/View.hpp"

namespace View {
    class FatalError : public Display::View {
    public:
        FatalError();
        void render(Display::Painter *painter) override;
    };
}
