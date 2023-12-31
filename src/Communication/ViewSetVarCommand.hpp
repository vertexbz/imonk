#pragma once

#include <StageManager.hpp>
#include <lib/spi/slave.impl.hpp>
#include "../Widget/_widget_.hpp"

namespace View {
class Custom;
}

class StageManager;

namespace Communication {
class ViewSetVarCommand : public Lib::SPI::Slave::Command {
protected:
    Lib::SPI::Slave::TypedInputBuffer<uint8_t> _view_id_buffer{};
    Lib::SPI::Slave::InputCommand _view_id;
    Lib::SPI::Slave::TypedInputBuffer<uint8_t> _slot_id_buffer{};
    Lib::SPI::Slave::InputCommand _slot_id;
    Lib::SPI::Slave::TypedInputBuffer<Widget::BaseWidget::TypeId> _type_id_buffer{};
    Lib::SPI::Slave::InputCommand _type_id;
    Lib::SPI::Slave::VariableInputBuffer _value_buffer{};
    Lib::SPI::Slave::InputCommand _value;
    volatile uint8_t _stage = 0;
    volatile bool _execute = false;
    StageManager *_stage_manager;

    bool expectedInput(Value value) override;

    Response handle(bool initial, Value value) override;

    void execute();

public:
    ViewSetVarCommand(ID id, StageManager *stage_manager);

    void loop() override;
};
}
