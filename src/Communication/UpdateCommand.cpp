//
// Created by Adam Makswiej on 10/12/2023.
//

#include "UpdateCommand.hpp"
#include <pico/ota_command.h>

const auto fw_name = "/firmware.bin";


Communication::UpdateCommand::UpdateCommand(ID id, FS *fs): NamedUploadCommand(id, fs, fw_name) {
    _fs.callback_final([this, fs](void*) { initUpdate(fs); });
}

static uint32_t crc(const void *d, uint32_t len) {
    uint32_t crc = 0xffffffff;
    const auto *data = static_cast<const uint8_t *>(d);
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xedb88320;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}

void Communication::UpdateCommand::initUpdate(FS *fs) {
    auto fw = fs->open(fw_name, "r");
    if (!fw) {
        return;
    }
    const uint32_t fileLength = fw.size();
    fw.close();

    auto *page = new OTACmdPage;
    memset(page, 0, sizeof(*page));
    memcpy(page->sign, "Pico OTA", sizeof(page->sign));
    page->count = 0;

    page->cmd[page->count].command = _OTA_WRITE;
    strncpy(page->cmd[page->count].write.filename, &fw_name[1], sizeof(page->cmd[page->count].write.filename));
    page->cmd[page->count].write.fileOffset = 0;
    page->cmd[page->count].write.fileLength = fileLength;
    page->cmd[page->count].write.flashAddress = XIP_BASE;
    page->count++;

    page->crc32 = crc(page, offsetof(OTACmdPage, crc32));

    auto cmd = fs->open(_OTA_COMMAND_FILE, "w");
    if (!cmd) {
        return;
    }
    cmd.write(reinterpret_cast<uint8_t *>(page), sizeof(*page));
    cmd.close();

    delay(1000);
    rp2040.reboot();
}

