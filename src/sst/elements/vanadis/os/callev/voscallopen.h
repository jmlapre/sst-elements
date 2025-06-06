// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef _H_VANADIS_SYSCALL_OPEN
#define _H_VANADIS_SYSCALL_OPEN

#include "os/voscallev.h"

namespace SST {
namespace Vanadis {

class VanadisSyscallOpenEvent : public VanadisSyscallEvent {
public:
    VanadisSyscallOpenEvent() : VanadisSyscallEvent() {}
    VanadisSyscallOpenEvent(uint32_t core, uint32_t thr, VanadisOSBitType bittype, uint64_t path_ptr, uint64_t flags, uint64_t mode)
        : VanadisSyscallEvent(core, thr, bittype), open_path_ptr(path_ptr), open_flags(flags), open_mode(mode) {}

    VanadisSyscallOp getOperation() override { return SYSCALL_OP_OPEN; }

    uint64_t getPathPointer() const { return open_path_ptr; }
    uint64_t getFlags() const { return open_flags; }
    uint64_t getMode() const { return open_mode; }

private:
    void serialize_order(SST::Core::Serialization::serializer& ser) override {
        VanadisSyscallEvent::serialize_order(ser);
        SST_SER(open_path_ptr);
        SST_SER(open_flags);
        SST_SER(open_mode);
    }
    ImplementSerializable(SST::Vanadis::VanadisSyscallOpenEvent);

    uint64_t open_path_ptr;
    uint64_t open_flags;
    uint64_t open_mode;
};

} // namespace Vanadis
} // namespace SST

#endif
