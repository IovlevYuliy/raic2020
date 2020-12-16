#ifndef _DEBUG_INTERFACE_HPP_
#define _DEBUG_INTERFACE_HPP_

#include "../stream/Stream.hpp"
#include "DebugCommand.hpp"
#include "DebugState.hpp"
#include <memory>

class DebugInterface {
public:
    DebugInterface(const std::shared_ptr<InputStream>& inputStream, const std::shared_ptr<OutputStream>& outputStream);
    void send(const DebugCommand& command);
    DebugState getState();

private:
    std::shared_ptr<InputStream> inputStream;
    std::shared_ptr<OutputStream> outputStream;
};

#endif