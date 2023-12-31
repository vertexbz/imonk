//
// Created by Adam Makswiej on 22/05/2022.
//

#include "lib/spi/slave/Interface.hpp"
#include "lib/spi/slave/Command.hpp"
#include <stdlib.h>
#include <api/HardwareSPI.h>
#include <hardware/spi.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <pins_arduino.h>

static Lib::SPI::Slave::Interface *instance0 = nullptr;
static Lib::SPI::Slave::Interface *instance1 = nullptr;


#define PICK(A, B) (_iface == 0 ? A##0##B : A##1##B)

Lib::SPI::Slave::Interface::Interface(const uint8_t iface) : _iface(iface) {
    _miso = PICK(PIN_SPI, _MISO);
    _ss = PICK(PIN_SPI, _SS);
    _sck = PICK(PIN_SPI, _SCK);
    _mosi = PICK(PIN_SPI, _MOSI);
}
void Lib::SPI::Slave::Interface::handler0() {
    if (instance0 && spi_is_readable(spi0)) {
        const uint8_t received = spi_get_hw(spi0)->dr;
        const uint8_t response = instance0->interrupt(received);
        if (spi_get_hw(spi0)->sr & SPI_SSPSR_TFE_BITS > 0) {
            spi_get_hw(spi0)->dr = response;
        }
    }
}

void Lib::SPI::Slave::Interface::handler0_ss(unsigned int gpio, uint32_t event_mask) {
    if (instance0 && static_cast<uint8_t>(instance0->_ss) == gpio && instance0->_lastCommand) {
        instance0->_lastCommand = nullptr;
    }
}

void Lib::SPI::Slave::Interface::handler1() {
    if (instance1 && spi_is_readable(spi1)) {
        const uint8_t received = spi_get_hw(spi1)->dr;
        const uint8_t response = instance1->interrupt(received);
        if (spi_get_hw(spi1)->sr & SPI_SSPSR_TFE_BITS > 0) {
            spi_get_hw(spi1)->dr = response;
        }
    }
}

void Lib::SPI::Slave::Interface::handler1_ss(unsigned int gpio, uint32_t event_mask) {
    if (instance1 && static_cast<uint8_t>(instance1->_ss) == gpio && instance1->_lastCommand) {
        instance1->_lastCommand = nullptr;
    }
}

inline spi_cpol_t cpol(uint8_t mode) {
    switch (mode) {
        case SPI_MODE2:
        case SPI_MODE3:
            return SPI_CPOL_1;
        case SPI_MODE0:
        case SPI_MODE1:
        default:
            return SPI_CPOL_0;
    }
}

inline spi_cpha_t cpha(uint8_t mode) {
    switch (mode) {
        case SPI_MODE1:
        case SPI_MODE3:
            return SPI_CPHA_1;
        case SPI_MODE0:
        case SPI_MODE2:
        default:
            return SPI_CPHA_0;
    }
}


Lib::SPI::Slave::Interface *Lib::SPI::Slave::Interface::addCommand(Command *command) {
    if (_commands && _commandCount == 0) {
        return this;
    }

    const size_t index = _commandCount;
    const size_t listSize = ++_commandCount * sizeof(Command*);
    if (index == 0) {
        _commands = static_cast<Command **>(malloc(listSize));
    } else {
        _commands = static_cast<Command **>(realloc(_commands, listSize));
    }

    _commands[index] = command;
    return this;
}


#define FOREACH(A) do { Command *command; uint8_t _current_command_i = 0; while (_current_command_i < _commandCount && (command = _commands[_current_command_i++])) A } while(0)

void Lib::SPI::Slave::Interface::setup() {
    uint32_t clock = 4000000;
    uint8_t mode = SPI_MODE3;
    spi_order_t order = SPI_MSB_FIRST;

    //    DEBUGSPI("SPISlave::begin(%d), rx=%d, cs=%d, sck=%d, tx=%d\n", hwCS, _RX, _CS, _SCK, _TX);
    gpio_set_function(static_cast<uint8_t>(_miso), GPIO_FUNC_SPI);
    gpio_set_function(static_cast<uint8_t>(_ss), GPIO_FUNC_SPI);
    gpio_set_function(static_cast<uint8_t>(_sck), GPIO_FUNC_SPI);
    gpio_set_function(static_cast<uint8_t>(_mosi), GPIO_FUNC_SPI);
    //    if (_initted) {
    //        DEBUGSPI("SPISlave: deinitting currently active SPI\n");
    //        spi_deinit(_spi);
    //    }
    //    DEBUGSPI("SPISlave: initting SPI\n");
    spi_init(PICK(spi,), clock);
    //    DEBUGSPI("SPISlave: actual baudrate=%u\n", spi_get_baudrate(PICK(spi)));
    spi_set_slave(PICK(spi,), true);
    spi_set_format(PICK(spi,), 8, cpol(mode),	cpha(mode), order);

    // Install our IRQ handler
    if (_iface == 0) {
        instance0 = this;
    } else {
        instance1 = this;
    }
    irq_set_exclusive_handler(PICK(SPI, _IRQ),  PICK(handler,));

    gpio_set_irq_enabled_with_callback(static_cast<uint8_t>(_ss), GPIO_IRQ_EDGE_RISE, true, PICK(handler, _ss));

    // Set to get IRQs on transmit and receive
    // spi_get_hw(PICK(spi,))->dr = Command::BadCall;
    spi_get_hw(PICK(spi,))->imsc = 2;// | 4;// | 8 ; // RTIM + RXIM // + TXIM (not RORIM)
    irq_set_enabled(PICK(SPI, _IRQ), true);

    FOREACH({
        command->setup();
    });
}

uint8_t Lib::SPI::Slave::Interface::execute(Command *command, bool initial, Command::Value value) {
    const Command::Response &result = command->handle(initial, value);
    if (result.next) {
        _lastCommand = command;
    } else {
        _lastCommand = nullptr;
    }
    return result.value;
}

Lib::SPI::Slave::Command::Value Lib::SPI::Slave::Interface::interrupt(Command::Value value) {
    if (_lastCommand) {
        if (_lastCommand->expectedInput(value)) {
            return execute(_lastCommand, false, value);
        }
        _lastCommand = nullptr;
    }

    FOREACH({
        if (command->id() == value) {
            return execute(command, true, 0);
        }
    });

    return Command::BadCall;
}

void Lib::SPI::Slave::Interface::end() {
    irq_remove_handler(PICK(SPI, _IRQ), PICK(handler,));
    spi_deinit(PICK(spi,));
    gpio_set_function(static_cast<uint8_t>(_miso), GPIO_FUNC_SIO);
    gpio_set_function(static_cast<uint8_t>(_ss), GPIO_FUNC_SIO);
    gpio_set_function(static_cast<uint8_t>(_sck), GPIO_FUNC_SIO);
    gpio_set_function(static_cast<uint8_t>(_mosi), GPIO_FUNC_SIO);
}

Lib::SPI::Slave::Interface::~Interface() {
    Interface::end();
    FOREACH({
        delete command;
    });
    delete _commands;
}