#include "util.hpp"


bool sflags(const char* mode, fs::OpenMode& om, fs::AccessMode& am) {
    switch (mode[0]) {
        case 'r':
            am = fs::AM_READ;
        om = fs::OM_DEFAULT;
        break;
        case 'w':
            am = fs::AM_WRITE;
        om = static_cast<fs::OpenMode>(fs::OM_CREATE | fs::OM_TRUNCATE);
        break;
        case 'a':
            am = fs::AM_WRITE;
        om = static_cast<fs::OpenMode>(fs::OM_CREATE | fs::OM_APPEND);
        break;
        default:
            return false;
    }
    switch (mode[1]) {
        case '+':
            am = static_cast<fs::AccessMode>(fs::AM_WRITE | fs::AM_READ);
        break;
        case 0:
            break;
        default:
            return false;
    }
    return true;
}
