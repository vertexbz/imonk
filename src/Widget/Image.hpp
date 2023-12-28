#pragma once

#include "../Filesystem/File.hpp"
#include "_widget_.hpp"

namespace Widget {
    class Image : public BaseWidget {
    protected:
        Display::Unit _x;
        Display::Unit _y;

        std::shared_ptr<Filesystem::File> _file;
        bool _rendered;
    public:
        Image(std::shared_ptr<Filesystem::File> file, Display::Unit x, Display::Unit y);

        void render(Display::Painter *) override;

        std::unique_ptr<BaseWidget> clone() override;

        void setValue(TypeId type, void *data) override;

        TypeId typeId() override;
    };
}
