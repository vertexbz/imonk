#pragma once
#include "../Display/View.hpp"

namespace View {
    class FirmwareUpgrade : public Display::View {
    public:
        FirmwareUpgrade();
        void render(Display::Painter *painter) override;
    };
}
