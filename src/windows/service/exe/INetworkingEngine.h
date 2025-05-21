// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once
#include "WslCoreNetworkEndpointSettings.h"

namespace lsw::core {

class INetworkingEngine
{
public:
    virtual ~INetworkingEngine() = default;
    virtual void Initialize() = 0;
    virtual void TraceLoggingRundown() noexcept = 0;
    virtual void FillInitialConfiguration(LX_MINI_INIT_NETWORKING_CONFIGURATION& message) = 0;
    virtual void StartPortTracker(wil::unique_socket&& socket) = 0;
};
} // namespace lsw::core
