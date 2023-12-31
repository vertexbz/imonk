//
// Created by Adam Makswiej on 25/05/2022.
//

#ifndef SPI_SLAVE_EXECCOMMAND_HPP
#define SPI_SLAVE_EXECCOMMAND_HPP

#include "Command.hpp"
#include "../../../../src/helper.callback.hpp"

namespace Lib::SPI::Slave {
class ExecCommand : public Command {
public:
    _SPIS_CB_(void, Callback);
protected:
    Callback _callback = nullptr;
    _SPIS_CB_CTX(context);
    bool _deferred = false;

    [[nodiscard]] Value execute() const;

    volatile struct {
        volatile bool execute;
    } _flag {
        .execute = false
    };
public:
    explicit ExecCommand(ID id, const Callback &callback, bool deferred = false);
    // explicit ExecCommand(ID id, const Callback &callback, void *context = nullptr, bool deferred = false);

    Response handle(bool initial, Value input) override;

    bool expectedInput(Value value) override;

    void loop() override; // needed only when deferred

    _SPIS_CB_CTX_DECL(callback, context, Callback);

    void defer(bool deferred = true);
};
}

#endif //SPI_SLAVE_EXECCOMMAND_HPP
