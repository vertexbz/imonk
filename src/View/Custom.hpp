#pragma once

#include "../Widget/_widget_.hpp"
#include <memory>
#include "_view_.hpp"

namespace View {
class Custom final : public BaseView {
    std::vector<std::unique_ptr<Widget::BaseWidget>> _widgets{};

protected:
    Display::Color _background;

    void _init() override;

public:
    explicit Custom(const Custom *original); // quasi cloning for stageman
    explicit Custom(Display::Color background);

    void addWidget(std::unique_ptr<Widget::BaseWidget> widget);

    Display::Color background() override;

    void render(Display::Painter *painter) override;

    Widget::BaseWidget *slot(SlotId id) override;

    SlotId slots() override;
};
}
