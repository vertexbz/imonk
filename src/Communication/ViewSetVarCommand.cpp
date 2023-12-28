//
// Created by Adam Makswiej on 22/12/2023.
//

#include "ViewSetVarCommand.hpp"
#include "../StageManager.hpp"

Communication::ViewSetVarCommand::ViewSetVarCommand(const ID id, StageManager *stage_manager)
: Command(id), _view_id(id, &_view_id_buffer), _slot_id(0, &_slot_id_buffer), _type_id(0, &_type_id_buffer), _value(0, &_value_buffer), _stage_manager(stage_manager) {
    _view_id.checksum(false);
    _view_id_buffer.validator([this](uint8_t id) -> bool {
        return _stage_manager->stagedId() == id || _stage_manager->currentId();
    });
    _slot_id.checksum(false);
    _slot_id_buffer.validator([this](uint8_t id) -> bool {
        auto view = _stage_manager->viewByStageId(_view_id_buffer.value());
        return view && id < view->slots();
    });
    _type_id.checksum(false);
    _type_id_buffer.validator([this](Widget::BaseWidget::TypeId id) -> bool {
        auto view = _stage_manager->viewByStageId(_view_id_buffer.value());
        if (!view) {
            return false;
        }
        auto slot = view->slot(_slot_id_buffer.value());
        if (!slot) {
            return false;
        }

        return slot->typeId() == id;
    });
    _value.checksum(true);
}

void Communication::ViewSetVarCommand::execute() {
    auto view = _stage_manager->viewByStageId(_view_id_buffer.value());
    if (!view) {
        return;
    }
    auto slot = view->slot(_slot_id_buffer.value());
    if (!slot || slot->typeId() != _type_id_buffer.value()) {
        return;
    }

    slot->setValue(_type_id_buffer.value(), _value_buffer.value());
}

bool Communication::ViewSetVarCommand::expectedInput(Value value) {
    return _stage != 0xFF;
}

Lib::SPI::Slave::Command::Response Communication::ViewSetVarCommand::handle(const bool initial, const Value value) {
    if (initial) {
        _stage = 0;
    }
    if (_stage == 0) {
        auto response = passTo(&_view_id, initial, value);
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
        return passTo(&_slot_id, true, value);
    }

    if (_stage == 2) {
        auto response = passTo(&_slot_id, false, value);
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

    if (_stage == 3) {
        ++_stage;
        return passTo(&_type_id, true, value);
    }

    if (_stage == 4) {
        auto response = passTo(&_type_id, false, value);
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

    if (_stage == 5) {
        ++_stage;
        const auto response = passTo(&_value, true, value);
        // pre-init size for bool/int/float
        if (_type_id_buffer.value() == Widget::BaseWidget::TypeId::BOOL) {
            (void)passTo(&_value, false, 1);
            (void)passTo(&_value, false, 0);
        } else if (_type_id_buffer.value() == Widget::BaseWidget::TypeId::INTEGER || _type_id_buffer.value() == Widget::BaseWidget::TypeId::FLOAT) {
            (void)passTo(&_value, false, 4);
            (void)passTo(&_value, false, 0);
        }
        return response;
    }

    if (_stage == 6) {
        const auto response = passTo(&_value, false, value);
        if (!response.next) {
            _stage = 0xFF;
            if (response.value == OK) {
                _execute = true;
            }
        }
        return response;
    }

    return Response{
        .value = BadCall,
        .next = false
    };
}

void Communication::ViewSetVarCommand::loop() {
    if (_execute) {
        _execute = false;
        execute();
    }
}
