// Copyright 2009-2024 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2024, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include <mercury/components/operating_system_base.h>

namespace SST {
namespace Hg {

OperatingSystemBase::OperatingSystemBase(ComponentId_t id, SST::Params &params)
    : SubComponent(id) {
    }

} // namespace Hg
} // namespace SST