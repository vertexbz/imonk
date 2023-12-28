#pragma once

#include "Display/Display.hpp"
#include "View/_view_.hpp"
#include "Job/Chronic.hpp"
#include "View/Custom.hpp"
#include <map>

namespace Filesystem {
class Filesystem;
}

class StageManager : protected Job::Chronic {
    struct cmp_str {
        bool operator()(char const *a, char const *b) const {
            return strcmp(a, b) < 0;
        }
    };
public:
    typedef uint8_t StageId;
protected:
    bool _newView = false;
    Display::Display *_display;
    Filesystem::Filesystem *_filesystem;

    std::unique_ptr<View::BaseView> _view = nullptr;

    StageId _current_id = 0;
    std::unique_ptr<View::BaseView> _current = nullptr;

    StageId _staged_id = 0;
    std::unique_ptr<View::BaseView> _staged = nullptr;

    std::map<char*, std::unique_ptr<View::Custom>, cmp_str> _custom_views{};
public:
    StageManager(Hz hz, Display::Display *display, Filesystem::Filesystem *fs);

    void enter(std::unique_ptr<View::BaseView> view);

    [[nodiscard]] StageId stagedId() const;

    [[nodiscard]] StageId currentId() const;

    void loadView(const char * name);

    bool hasView(const char *name) const; // used in comms, should be fast

    void forgetView(const char * name);

    bool stage(StageId id, const char * name);

    void commit();

    void abort();

    View::BaseView *viewByStageId(const StageId id) {
        if (id == _current_id) {
            return _current.get();
        }
        if (id == _staged_id) {
            return _staged.get();
        }

        return nullptr;
    }

protected:
    void run() override;
};
