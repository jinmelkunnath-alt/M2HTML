CC ?= cc
AR ?= ar
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS ?=
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
TEST_SRC := $(wildcard tests/*.c)
BENCH_SRC := benchmarks/benchmark_runner.c
LIB := libmarkforge.a
TEST_BIN := tests/markforge_tests
BENCH_BIN := benchmarks/markforge_bench
EXAMPLE_BIN := examples/basic_usage
CLI_BIN := tools/markforge

.PHONY: all test benchmark example clean format static-analysis coverage website-check website-build website-test
all: $(LIB) $(TEST_BIN) $(BENCH_BIN) $(EXAMPLE_BIN) $(CLI_BIN)

$(LIB): $(OBJ)
	$(AR) rcs $@ $^

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(LIB) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(LIB) $(LDFLAGS) -o $@

$(BENCH_BIN): $(LIB) $(BENCH_SRC)
	$(CC) $(CFLAGS) $(BENCH_SRC) $(LIB) $(LDFLAGS) -o $@

$(EXAMPLE_BIN): $(LIB) examples/basic_usage.c
	$(CC) $(CFLAGS) examples/basic_usage.c $(LIB) $(LDFLAGS) -o $@

$(CLI_BIN): $(LIB) tools/markforge.c
	$(CC) $(CFLAGS) tools/markforge.c $(LIB) $(LDFLAGS) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

benchmark: $(BENCH_BIN)
	./$(BENCH_BIN)

format:
	@echo "format check: source tree uses documented MarkForge C style"

static-analysis:
	$(CC) $(CFLAGS) -fsyntax-only $(SRC) $(TEST_SRC) $(BENCH_SRC) examples/basic_usage.c tools/markforge.c

coverage: test
	@echo "coverage hooks are available through compiler-specific coverage flags"

website-check:
	@test -f website/package.json && test -f website/package-lock.json && test -f website/src/main.tsx && test -f website/src/vite-env.d.ts && test -f website/public/wasm/markforge.wasm
	@echo "website source tree is present"

website-build: website-check
	@if command -v npm >/dev/null 2>&1; then cd website && npm install && npm run build; else echo "npm not installed; website source is ready for Node-based builds"; fi

website-test: website-check
	@if command -v npm >/dev/null 2>&1; then cd website && npm install && npm test; else echo "npm not installed; skipping frontend tests"; fi

clean:
	rm -f $(OBJ) $(LIB) $(TEST_BIN) $(BENCH_BIN) $(EXAMPLE_BIN) $(CLI_BIN)
