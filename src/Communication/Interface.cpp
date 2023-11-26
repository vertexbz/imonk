//
// Created by Adam Makswiej on 25/11/2023.
//

#include "Interface.hpp"
#include <api/HardwareSPI.h>
#include <hardware/spi.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <pins_arduino.h>


#undef PIN_SPI1_MISO
#define PIN_SPI1_MISO   (28u)



#define I_INSTANCE _I_INSTANCE(INTERFACE)
#define _I_INSTANCE(i) __I_INSTANCE(i)
#define __I_INSTANCE(i) instance## i

#define I_HANDLER _I_HANDLER(INTERFACE)
#define _I_HANDLER(i) __I_HANDLER(i)
#define __I_HANDLER(i) handler## i

#define I_DEV _I_DEV(INTERFACE)
#define _I_DEV(i) __I_DEV(i)
#define __I_DEV(i) spi## i

#define I_IRQ _I_IRQ(INTERFACE)
#define _I_IRQ(i) __I_IRQ(i)
#define __I_IRQ(i) SPI## i ##_IRQ

#define I_MISO _I_MISO(INTERFACE)
#define _I_MISO(i) __I_MISO(i)
#define __I_MISO(i) PIN_SPI## i ##_MISO
#define I_SS _I_SS(INTERFACE)
#define _I_SS(i) __I_SS(i)
#define __I_SS(i) PIN_SPI## i ##_SS
#define I_SCK _I_SCK(INTERFACE)
#define _I_SCK(i) __I_SCK(i)
#define __I_SCK(i) PIN_SPI## i ##_SCK
#define I_MOSI _I_MOSI(INTERFACE)
#define _I_MOSI(i) __I_MOSI(i)
#define __I_MOSI(i) PIN_SPI## i ##_MOSI

Communication::Interface::Interface() {

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


static Communication::Interface *instance0;
void Communication::Interface::handler0() {
    if (instance0 && spi_is_readable(spi0)) {
        const uint8_t received = spi_get_hw(spi0)->dr;
        const uint8_t response = instance0->process(received);
        spi_get_hw(spi0)->dr = response;
    }
}

static Communication::Interface *instance1;
void Communication::Interface::handler1() {
    if (instance1 && spi_is_readable(spi1)) {
        const uint8_t received = spi_get_hw(spi1)->dr;
        const uint8_t response = instance1->process(received);
        spi_get_hw(spi1)->dr = response;
    }
}

void Communication::Interface::begin() {
    uint32_t clock = 4000000;
    uint8_t mode = SPI_MODE0;
    spi_order_t order = SPI_MSB_FIRST;

//    DEBUGSPI("SPISlave::begin(%d), rx=%d, cs=%d, sck=%d, tx=%d\n", hwCS, _RX, _CS, _SCK, _TX);
    gpio_set_function(I_MISO, GPIO_FUNC_SPI);
    gpio_set_function(I_SS, GPIO_FUNC_SPI);
    gpio_set_function(I_SCK, GPIO_FUNC_SPI);
    gpio_set_function(I_MOSI, GPIO_FUNC_SPI);
//    if (_initted) {
//        DEBUGSPI("SPISlave: deinitting currently active SPI\n");
//        spi_deinit(_spi);
//    }
//    DEBUGSPI("SPISlave: initting SPI\n");
    spi_init(I_DEV, clock);
//    DEBUGSPI("SPISlave: actual baudrate=%u\n", spi_get_baudrate(I_DEV));
    spi_set_slave(I_DEV, true);
    spi_set_format(I_DEV, 8, cpol(mode),	cpha(mode), order);

    // Install our IRQ handler
    I_INSTANCE = this;
    irq_set_exclusive_handler(I_IRQ,  Communication::Interface::I_HANDLER);

    // Set to get IRQs on transmit and receive
    spi_get_hw(I_DEV)->dr = 0x69; // todo remove or change to 00
    spi_get_hw(I_DEV)->imsc = 2 | 4;// | 8 ; // RTIM + RXIM + TXIM (not RORIM)
    irq_set_enabled(I_IRQ, true);
}

uint8_t Communication::Interface::process(uint8_t byte) {
    // Upload: 01 [2: name len] 00 [name] 00 [4: len] 00 00
    //            01 [2: name len] 00 [name] 00 [4: len] 00 00
    return ~byte;
}
