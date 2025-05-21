/*++

Copyright (c) Microsoft. All rights reserved.

Module Name:

    timezone.h

Abstract:

    This file contains timezone function declarations.

--*/

#pragma once

#include "WslDistributionConfig.h"

void UpdateTimezone(std::string_view Timezone, const lsw::linux::WslDistributionConfig& Config);

void UpdateTimezone(gsl::span<gsl::byte> Buffer, const lsw::linux::WslDistributionConfig& Config);
