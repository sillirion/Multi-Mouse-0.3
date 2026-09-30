#ifndef MULTI_MOUSE_BACKEND_H
#define MULTI_MOUSE_BACKEND_H

#include <cstdint>
#include <memory>

namespace godot {

class MultiMouseServer;

class MultiMouseBackend {
public:
    explicit MultiMouseBackend(MultiMouseServer *p_server) : server(p_server) {}
    virtual ~MultiMouseBackend() = default;

    virtual void start() {}
    virtual void stop() {}
    virtual void poll() {}
    virtual uint64_t get_raw_input_count() const { return 0; }
    virtual bool refresh_raw_input_registration() { return false; }

    virtual void set_target_window(void * /*hwnd*/) {}

protected:
    MultiMouseServer *server = nullptr;
};

std::unique_ptr<MultiMouseBackend> create_multi_mouse_backend(MultiMouseServer *server);

} // namespace godot

#endif // MULTI_MOUSE_BACKEND_H
