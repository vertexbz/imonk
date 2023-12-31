#pragma once

#if defined(TARGET_RP2040) || defined(ARDUINO_ARCH_RP2040)
#include <functional>
#define _SPIS_CB_(ret, name, ...) typedef std::function<ret(__VA_ARGS__)> name
#define _SPIS_CB_CTX(name)
#define _SPIS_CB_CTX_DECL(callback_name, context_name, callback_type) void callback_name(const callback_type &fn)
#define _SPIS_CB_CTX_DEF(cls, callback_name, context_name, callback_type) \
    void cls::callback_name(const callback_type &fn) {\
        _##callback_name = fn;\
    }
#define _SPIS_CB_CTX_DEF_TMPL(tmpl, cls, callback_name, context_name, callback_type) \
    template<tmpl>\
    void cls::callback_name(const callback_type &fn) {\
        _##callback_name = fn;\
    }

#define _SPIS_CB_CALL(callback_name, context_name, ...) _##callback_name(__VA_ARGS__)
#define _SPIS_CB_IF_CALL(callback_name, context_name, ...) if (_##callback_name) _##callback_name(__VA_ARGS__)
#else
#define _SPIS_CB_(ret, name, ...) typedef ret (*name)(__VA_OPT__(__VA_ARGS__,) void *context);
#define _SPIS_CB_CTX(name) void *_##name = nullptr;
#define _SPIS_CB_CTX_DECL(callback_name, context_name, callback_type) \
    void context_name(void *ctx); \
    void callback_name(const callback_type &fn); \
    void callback_name(const callback_type &fn, void *ctx);
#define _SPIS_CB_CTX_DEF(cls, callback_name, context_name, callback_type) \
    void cls::callback_name(const callback_type &fn) {\
        _##callback_name = fn;\
    }\
    void cls::context_name(void *ctx) {\
        _##context_name = ctx;\
    }\
    void cls::callback_name(const callback_type &fn, void *ctx) {\
        callback_name(fn);\
        context_name(ctx);\
    }
#define _SPIS_CB_CTX_DEF_TMPL(tmpl, cls, callback_name, context_name, callback_type) \
    template<tmpl>\
    void cls::callback_name(const callback_type &fn) {\
        _##callback_name = fn;\
    }\
    template<tmpl>\
    void cls::context_name(void *ctx) {\
        _##context_name = ctx;\
    }\
    template<tmpl>\
    void cls::callback_name(const callback_type &fn, void *ctx) {\
        callback_name(fn);\
        context_name(ctx);\
    }
#define _SPIS_CB_CALL(callback_name, context_name, ...) _##callback_name(__VA_OPT__(__VA_ARGS__,) _##context_name);
#define _SPIS_CB_IF_CALL(callback_name, context_name, ...) if (_##callback_name) _##callback_name(__VA_OPT__(__VA_ARGS__,) _##context_name);
#endif
