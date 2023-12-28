//
// Created by Adam Makswiej on 22/12/2023.
//

#include "ViewStageCommand.hpp"
#include "../StageManager.hpp"


StageManager::StageId Communication::ViewStageCommand::_nextStageId() {
    static StageManager::StageId id = 0;
    if (++id == 0 || id == 0xFF) {
        id = 1;
    }
    return id;
}

Communication::ViewStageCommand::ViewStageCommand(const ID id, StageManager *stage_manager)
: Command(id), _name(id, &_name_buffer), _id(0, &_id_buffer), _stage_manager(stage_manager) {
    _name.checksum(false);
    _name_buffer.validator([this](const Lib::SPI::Slave::Buffer::Data data, const Lib::SPI::Slave::Buffer::Size size) -> bool {
        if (size > 0 && data[size] == 0) {
            return _stage_manager->hasView(reinterpret_cast<char*>(data));
        }
        return false;
    });
}

void Communication::ViewStageCommand::execute() {
    _stage_manager->stage(_id_buffer.value(), reinterpret_cast<char*>(_name_buffer.value()));
}

void Communication::ViewStageCommand::loop() {
    if (_execute) {
        _execute = false;
        execute();
    }
}

bool Communication::ViewStageCommand::expectedInput(Value value) {
    return _stage != 0xFF;
}

Lib::SPI::Slave::Command::Response Communication::ViewStageCommand::handle(const bool initial, const Value value) {
    if (initial) {
        _stage = 0;
    }
    if (_stage == 0) {
        auto response = passTo(&_name, initial, value);
        if (!response.next) {
            response.next = true;
            if (response.value == OK) {
                ++_stage;
                response.value = ok;
            } else {
                _stage = 0xFF;
            }
        }
        return response;
    }

    if (_stage == 1) {
        ++_stage;
        _id_buffer.update(_nextStageId());
        return passTo(&_id, true, value);
    }

    if (_stage == 2) {
        const auto response = passTo(&_id, false, value);
        if (!response.next) {
            _stage = 0xFF;
            _execute = true;
        }
        return response;
    }

    return Response{
        .value = BadCall,
        .next = false
    };
}