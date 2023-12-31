//
// Created by Adam Makswiej on 25/05/2022.
//

#include "../include/lib/spi/slave/ExecCommand.hpp"
#include "./helper.hpp"

Lib::SPI::Slave::ExecCommand::ExecCommand(const ID id, const Callback &callback, const bool deferred) : Command(id) {
    _callback = callback;
    _deferred = deferred;
}

// Lib::SPI::Slave::ExecCommand::ExecCommand(const ID id, const Callback &callback, void *context, const bool deferred) : Command(id) {
//     _callback = callback;
//     _deferred = deferred;
//     _context = context;
// }

Lib::SPI::Slave::Command::Response Lib::SPI::Slave::ExecCommand::handle(const bool initial, Value input) {
    if (initial) {
        if (_deferred) {
            _flag.execute = true;
            return stop(id());
        }

        return stop(execute());
    }

    return stop(BadCall);
}

bool Lib::SPI::Slave::ExecCommand::expectedInput(Value value) {
    return false;
}

Lib::SPI::Slave::Command::Value Lib::SPI::Slave::ExecCommand::execute() const {
    _SPIS_CB_IF_CALL(callback, context);
    return 0;
}

void Lib::SPI::Slave::ExecCommand::loop() {
    if (_deferred && _flag.execute) {
        _flag.execute = false;
        (void)execute();
    }
}

_SPIS_CB_CTX_DEF(Lib::SPI::Slave::ExecCommand, callback, context, Callback)

void Lib::SPI::Slave::ExecCommand::defer(const bool deferred) {
    _deferred = deferred;
}
