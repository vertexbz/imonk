#pragma once

#include "../StageManager.hpp"
#include <lib/spi/slave.impl.hpp>


namespace Communication {
class ViewStageCommand : public Lib::SPI::Slave::Command {
    static StageManager::StageId _nextStageId();
protected:
    Lib::SPI::Slave::VariableInputBuffer _name_buffer{};
    Lib::SPI::Slave::InputCommand _name;
    Lib::SPI::Slave::TypedOutputBuffer<StageManager::StageId> _id_buffer{};
    Lib::SPI::Slave::OutputCommand _id;
    volatile uint8_t _stage = 0;
    volatile bool _execute = false;
    StageManager *_stage_manager;

    bool expectedInput(Value value) override;

    Response handle(bool initial, Value value) override;

    void execute();

public:
    explicit ViewStageCommand(ID id, StageManager *stage_manager);

    void loop() override;
};
}
