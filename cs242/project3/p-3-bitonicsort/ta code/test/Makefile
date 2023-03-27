_PROJECT_ := bitonicsort
CXX_CMPLR := g++
CXX_FLAGS := -std=c++11 -faligned-new -Wall -Werror -Wextra -pedantic -lbenchmark -lbenchmark_main -lpthread -fopenmp -O2
INC_PATHS := -I../src -I../
SRC_FILES := ../src/bitonicsort.cc ../src/merge_segments.cc
BUILD_DIR := /home/$(USER)/mybuilds/$(_PROJECT_)

.PHONY: $(BUILD_DIR)\
	benchmarks\
	$(BUILD_DIR)/benchmarks\
	all\
	clean

all: benchmarks

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/benchmarks: benchmarks.cc | $(BUILD_DIR)
	$(CXX_CMPLR) $(INC_PATHS) $(SRC_FILES) $< $(CXX_FLAGS) $(MAKE_ARGS) -o $@
benchmarks: $(BUILD_DIR)/benchmarks
	$< --benchmark_enable_random_interleaving=false --benchmark_counters_tabular=true $(PROGRAM_ARGS)

clean:
	rm -r $(BUILD_DIR)
