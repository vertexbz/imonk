//
// Created by Adam Makswiej on 25/11/2023.
//

#include "./Interface.hpp"
#include "./UpdateCommand.hpp"
#include "./CRCFileUploadCommand.hpp"
#include "./DirectoryManifest.hpp"
#include "../Filesystem/Filesystem.hpp"

using namespace Lib::SPI::Slave::CommandFactory;
using Data = Lib::SPI::Slave::Buffer::Data;
using Size = Lib::SPI::Slave::Buffer::Size;


struct Version {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t micro = 1;
};

namespace Communication {
class SceneStageCommand : public Lib::SPI::Slave::Command {
public:
    typedef uint8_t StageId;
private:
    static StageId _nextStageId() {
        static StageId id = 0;
        if (++id == 0 || id == 0xFF) {
            id = 1;
        }
        return id;
    }
protected:

    bool expectedInput(Value value) override {
        return _stage != Lib::SPI::Slave::FileCommandStage::Unknown;
    }

    Response handle(const bool initial, const Value value) {
        if (initial) {
            _stage = Lib::SPI::Slave::FileCommandStage::InitSize;
        }

        if (_stage == FileCommandStage::InitSize) {
            ++_stage;
            return _size.handle(true, value);
        }

        if (_stage == FileCommandStage::Size) {
            auto response = _size.handle(false, value);
            if (!response.next) {
                response.next = true;
                if (response.value == OK) {
                    ++_stage;
                    response.value = ok;
                } else {
                    _stage = FileCommandStage::Unknown;
                }
            }
            return response;
        }

        if (_stage == FileCommandStage::InitData) {
            ++_stage;
            _fs.open(destiantionPath(), _size_buffer.read());
            return _data.handle(true, value);
        }

        if (_stage == FileCommandStage::Data) {
            const auto response = _data.handle(false, value);
            if (!response.next) {
                ++_stage;
                if (response.value == OK) {
                    _flag.execute = true;
                }
            }
            return response;
        }

        return Response{
            .value = BadCall,
            .next = false
        };
    }
public:
    SceneStageCommand(const ID id, Contract::FS *fs, const char *basepath)
    : Command(id),  _size(0, &_size_buffer), _fs(fs, basepath), _data(0, &_fs) {
        _size.checksum(false);
        _size_buffer.validator([](const uint32_t size) -> bool {
            return size > 0;
        });
        _data.defer(true);
    }

    void loop() {
        _fs.loop();
        if (_flag.execute) {
            _flag.execute = false;
            _fs.trigger();
        }
    }
};
}

Communication::Interface::Interface(Filesystem::Filesystem *fs) {
    // Firmware
    addCommand(TypedOutputCommand<Version>(0x01));
    addCommand(_update = new UpdateCommand(0x0A, fs));
    addCommand(_reboot = new Lib::SPI::Slave::ExecCommand(0x0F, [] { rp2040.reboot(); }, true));

    // Imgaes
    addCommand(_remove_image = VariableInputCommand(0x10, true, [fs](const Data data, Size _) {
        fs->remove((String(Filesystem::Filesystem::_IMAGES_) + reinterpret_cast<char*>(data)).c_str());
    }, true));

    addCommand(_upload_image = new CRCFileUploadCommand(0x11, fs, Filesystem::Filesystem::_IMAGES_)); // todo validate .png
    addCommand(new DirectoryManifest(0x12, fs, Filesystem::Filesystem::_IMAGES));

    // Scenes
    addCommand(_remove_scene = VariableInputCommand(0x20, true, [fs](const Data data, Size _) {
        fs->remove((String(Filesystem::Filesystem::_SCENES_) + reinterpret_cast<char*>(data)).c_str());
    }, true));

    addCommand(_upload_scene = new CRCFileUploadCommand(0x21, fs, Filesystem::Filesystem::_SCENES_)); // todo validate .json
    addCommand(new DirectoryManifest(0x22, fs, Filesystem::Filesystem::_SCENES));

    // Action
    addCommand(_scene_init = VariableInputCommand(0x50, [](const Data data, Size _) {
        const auto scene_name = reinterpret_cast<char*>(data);

        // stage scene in manager
    }, true, [fs](const Data data, Size _) -> bool {
        const auto scene_name = reinterpret_cast<char*>(data);

        return false; // if scene name is known
    }, true));

    addCommand(_scene_set = VariableInputCommand(0x51, true, [fs](const Data data, Size _) {
        const auto scene_name = reinterpret_cast<char*>(data);

    }, true));

    addCommand(_scene_commit = TypedInputCommand<SceneStageCommand::StageId>(0x5E, [](SceneStageCommand::StageId value) {
        rp2040.reboot();
    }, true, [](SceneStageCommand::StageId value) -> bool {
        return false; // if the value is currently staged scene
    }, false));

    addCommand(_scene_abort = TypedInputCommand<SceneStageCommand::StageId>(0x5F, [](SceneStageCommand::StageId value) {
        rp2040.reboot(); scene_manager.abort(value);
    }, true, [](SceneStageCommand::StageId value) -> bool {
        return false; // if the value is currently staged scene
    }, false));
}

void Communication::Interface::begin() {
    setup();
}

void Communication::Interface::loop() {
    _update->loop();
    _remove_image->loop();
    _upload_image->loop();
    _remove_scene->loop();
    _upload_scene->loop();
    _scene_init->loop();
    _scene_commit->loop();
    _scene_abort->loop();
    _reboot->loop();
}
