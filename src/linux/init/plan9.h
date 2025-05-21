// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once

#include <optional>
#include <lxwil.h>
#include "SocketChannel.h"
#include "WslDistributionConfig.h"

std::pair<unsigned int, lsw::shared::SocketChannel> StartPlan9Server(const char* socketWindowsPath, const lsw::linux::WslDistributionConfig& Config);

void RunPlan9Server(const char* socketPath, const char* logFile, int logLevel, bool truncateLog, int controlSocket, int serverFd, wil::unique_fd& pipeFd);

bool StopPlan9Server(bool force, lsw::linux::WslDistributionConfig& Config);