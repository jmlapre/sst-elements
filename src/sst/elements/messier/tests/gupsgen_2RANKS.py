import sst

# Define SST core options
sst.setProgramOption("timebase", "1ps")

# Tell SST what statistics handling we want
sst.setStatisticLoadLevel(4)

memory_mb = 128

# Define the simulation components
comp_cpu = sst.Component("cpu", "miranda.BaseCPU")
comp_cpu.addParams({
	"verbose" : 0,
})
cpugen = comp_cpu.setSubComponent("generator", "miranda.GUPSGenerator")
cpugen.addParams({
	"verbose" : 0,
	"count" : 10000,
	"max_address" : ((memory_mb) // 2) * 1024 * 1024,
})

# Enable statistics outputs
comp_cpu.enableAllStatistics({"type":"sst.AccumulatorStatistic"})

comp_l1cache = sst.Component("l1cache", "memHierarchy.Cache")
comp_l1cache.addParams({
      "access_latency_cycles" : "2",
      "cache_frequency" : "2 Ghz",
      "replacement_policy" : "lru",
      "coherence_protocol" : "MESI",
      "associativity" : "4",
      "cache_line_size" : "64",
      "prefetcher" : "cassini.StridePrefetcher",
      "L1" : "1",
      "cache_size" : "8KB",
      "backing" : "none",
})

# Enable statistics outputs
comp_l1cache.enableAllStatistics({"type":"sst.AccumulatorStatistic"})

nvm_memory = sst.Component("memory", "memHierarchy.MemController")
nvm_mem_backend = nvm_memory.setSubComponent("backend", "memHierarchy.Messier")
nvm_memctrl_params = {
    "clock" : "1024 MHz",
    "backing" : "none",
    "addr_range_start" : 0,
}

nvm_mem_backend_params = {
    "mem_size" : "1024MB",
   # "max_requests_per_cycle" : 1,
   # "backend.device_count" : 1,
   # "backend.link_count" : 4,
   # "backend.vault_count" : 16,
   # "backend.queue_depth" : 64,
   # "backend.bank_count" : 16,
   # "backend.dram_count" : 20,
   # "backend.capacity_per_device" : 4, # Min is now 4 but we'll just use 1 of it
   # "backend.xbar_depth" : 128,
   # "backend.max_req_size" : 64,
   # "backend.tag_count" : 512,
}

nvm_memory.addParams(nvm_memctrl_params)
nvm_mem_backend.addParams(nvm_mem_backend_params)

messier_inst = sst.Component("NVMmemory", "Messier")

messier_params = {
	"clock" : "1 GHz",

}
messier_inst.addParams(messier_params)

messier_inst.addParams({
      "tCL" : "30",
      "tRCD" : "300",
      "clock" : "1GHz",
      "tCL_W" : "1000",
      "write_buffer_size" : "32",
      "flush_th" : "90",
      "num_banks" : "32",
      "num_ranks" : 2,
      "max_outstanding" : "32",
      "max_current_weight" : "160",
      "read_weight" : "5",
      "write_weight" : "50",
      "max_writes" : 4
})


#nvm_memory.addParams({
 #     "coherence_protocol" : "MESI",
#      "backend.access_time" : "1000 ns",
 #     "backend.mem_size" : str(memory_mb * 1024 * 1024) + "B",
#      "clock" : "1GHz"
#})

#nvm_memory.addParams(nvm_mem_params)

link_nvm_bus_link = sst.Link("link_nvm_bus_link")
link_nvm_bus_link.connect( (messier_inst, "bus", "50ps"), (nvm_mem_backend, "nvm_link", "50ps") )

# Define the simulation links
link_cpu_cache_link = sst.Link("link_cpu_cache_link")
link_cpu_cache_link.connect( (comp_cpu, "cache_link", "1000ps"), (comp_l1cache, "highlink", "1000ps") )
link_cpu_cache_link.setNoCut()

link_mem_bus_link = sst.Link("link_mem_bus_link")
link_mem_bus_link.connect( (comp_l1cache, "lowlink", "50ps"), (nvm_memory, "highlink", "50ps") )
