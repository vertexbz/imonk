#pragma once

#include "../Display/types.hpp"
#include "../Display/Widget.hpp"
#include <memory>

namespace Widget {
class BaseWidget : public Display::Widget {
public:
    enum class TypeId : uint8_t {
        STRING = 0x57,
        BOOL = 0x01,
        INTEGER = 0x69,
        FLOAT = 0x42,
        UNKNOWN = 0,
    };

    virtual void init() {
    };

    virtual std::unique_ptr<BaseWidget> clone() = 0;

    virtual void setValue(TypeId type, void *data) = 0;

    virtual TypeId typeId() = 0;
};
}
