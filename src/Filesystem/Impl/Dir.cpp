//
// Created by Adam Makswiej on 10/12/2023.
//

#include "Dir.hpp"

bool Filesystem::Impl::Dir::fileTag(Tag tag, void *data, size_t size) {
    return _getAttr(tag, size, data);
}
