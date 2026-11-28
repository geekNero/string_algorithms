# Uses Clang's C++20 module support (including Apple Clang).
CXX := clang++
CXXFLAGS ?= -O0 -g -Wall -Wextra
MODULE_FLAGS := -std=c++20 -fcxx-modules
TEST_FLAGS := -isystem build/deps -fprebuilt-module-path="$(CURDIR)/build"
SANITIZER_FLAGS := -isystem build/deps -fprebuilt-module-path="$(CURDIR)/build/san" -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
COMPILE_COMMAND = $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(MODULE_FLAGS)
DOCTEST_VERSION := 2.4.12
DOCTEST_HEADER := build/deps/doctest/doctest.h
MODULES := build/fingerprint.pcm build/oracle.pcm
SAN_MODULES := build/san/fingerprint.pcm build/san/oracle.pcm
OBJECTS := build/fingerprint.o build/oracle.o build/test_fingerprint.o
SAN_OBJECTS := build/san/fingerprint.o build/san/oracle.o build/san/test_fingerprint.o

.PHONY: all test clean FORCE
.DELETE_ON_ERROR:
all: build/test_fingerprint compile_commands.json

test: all
	./build/test_fingerprint --abort-after=1

sanitize: build/san/test_fingerprint 
	./build/san/test_fingerprint

build:
	mkdir -p $@/san

$(DOCTEST_HEADER):
	mkdir -p $(@D)
	curl --fail --location --retry 3 https://raw.githubusercontent.com/doctest/doctest/v$(DOCTEST_VERSION)/doctest/doctest.h -o $@.tmp
	mv $@.tmp $@

build/fingerprint.pcm: src/in_place_fingerprinting.cppm Makefile | build compile_commands.json
	$(COMPILE_COMMAND) --precompile $< -o $@

build/oracle.pcm: tests/oracle.cppm Makefile | build compile_commands.json
	$(COMPILE_COMMAND) --precompile $< -o $@

build/san/fingerprint.pcm: src/in_place_fingerprinting.cppm Makefile | build 
	$(COMPILE_COMMAND) $(SANITIZER_FLAGS) --precompile $< -o $@

build/san/oracle.pcm: tests/oracle.cppm Makefile | build 
	$(COMPILE_COMMAND) $(SANITIZER_FLAGS) --precompile $< -o $@

build/san/%.o: build/san/%.pcm
	$(CXX) $(CXXFLAGS) $(MODULE_FLAGS) $(SANITIZER_FLAGS) -c $< -o $@

build/%.o: build/%.pcm
	$(CXX) $(CXXFLAGS) $(MODULE_FLAGS) -c $< -o $@

build/test_fingerprint.o: tests/test_fingerprint.cpp $(MODULES) $(DOCTEST_HEADER) Makefile | compile_commands.json
	# clangd may resolve module search paths relative to its launch directory.
	$(COMPILE_COMMAND) $(TEST_FLAGS) -c $< -o $@

build/san/test_fingerprint.o: tests/test_fingerprint.cpp $(SAN_MODULES) $(DOCTEST_HEADER) Makefile 
	# clangd may resolve module search paths relative to its launch directory.
	$(COMPILE_COMMAND) $(SANITIZER_FLAGS) -c $< -o $@

build/test_fingerprint: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

build/san/test_fingerprint: $(SAN_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SANITIZER_FLAGS) $(SAN_OBJECTS) $(LDLIBS) -o $@

# Generate before compiling, even when source files currently contain errors.
compile_commands.json: tools/compile_commands.py Makefile FORCE
	python3 tools/compile_commands.py $(COMPILE_COMMAND) -- $(TEST_FLAGS)

FORCE:

clean:
	rm -rf build compile_commands.json
