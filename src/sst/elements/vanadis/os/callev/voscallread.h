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

#ifndef _H_VANADIS_SYSCALL_READ
#define _H_VANADIS_SYSCALL_READ

#include "os/voscallev.h"

namespace SST {
namespace Vanadis {

class VanadisSyscallReadEvent : public VanadisSyscallEvent {
public:
    VanadisSyscallReadEvent() : VanadisSyscallEvent() {}
    VanadisSyscallReadEvent(uint32_t core, uint32_t thr, VanadisOSBitType bittype, int64_t fd, uint64_t buff_ptr, int64_t count)
        : VanadisSyscallEvent(core, thr, bittype), read_fd(fd), read_buff_addr(buff_ptr), read_count(count) {}

    VanadisSyscallOp getOperation() override { return SYSCALL_OP_READ; }

    int64_t getFileDescriptor() const { return read_fd; }
    uint64_t getBufferAddress() const { return read_buff_addr; }
    int64_t getBufferCount() const { return read_count; }

private:
    void serialize_order(SST::Core::Serialization::serializer& ser) override {
        VanadisSyscallEvent::serialize_order(ser);
        SST_SER(read_fd);
        SST_SER(read_buff_addr);
        SST_SER(read_count);
    }
    ImplementSerializable(SST::Vanadis::VanadisSyscallReadEvent);

    int64_t read_fd;
    uint64_t read_buff_addr;
    int64_t read_count;
};

} // namespace Vanadis
} // namespace SST

#endif
