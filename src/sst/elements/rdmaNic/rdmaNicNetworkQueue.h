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

class NetworkQueue {
    struct Entry {
        Entry( RdmaNicNetworkEvent* ev, int destNode ) : ev(ev), destNode(destNode ) {}
        RdmaNicNetworkEvent* ev;
        int destNode;
    };
  public:
    NetworkQueue( RdmaNic& nic, int numVC, int maxSize ) : nic(nic), maxSize(maxSize) {
        queues.resize( numVC );
    }
    void process();
    void add( int vc, int destNode, RdmaNicNetworkEvent* ev ) {
        queues[vc].push( Entry( ev, destNode) );
    }
    bool full( int vc ) { return queues[vc].size() == maxSize; }

  private:
    RdmaNic& nic;
    std::vector< std::queue< Entry > > queues;
    int maxSize;
};
