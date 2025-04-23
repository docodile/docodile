TEST_FILES := $(wildcard src/*.test.c)
ALL_TESTS := $(TEST_FILES:test/%.c=test_%)
WATCH ?= false
PROGRAM := gendoc
SRC_DIR := src
BUILD_DIR := $(SRC_DIR)/build
DEBUG_DIR := $(BUILD_DIR)/debug
SRC := $(SRC_DIR)/main.c

$(DEBUG_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(DEBUG_DIR)
	@gcc -g -o $@ $< $(LFLAGS) -DLOG_LEVEL=4

$(BUILD_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(BUILD_DIR)
	@gcc -o $@ $< -lc

build: $(BUILD_DIR)/$(PROGRAM)
debug: $(DEBUG_DIR)/$(PROGRAM)

clean:
	@rm -rf $(BUILD_DIR)

test: $(ALL_TESTS)

%.test: src/%.c
	@gcc -o $@ $<

test_%: %.test
ifeq ($(WATCH), true)
	@find src | entr -rs "make $< && ./$(<F)"
else
	@./$(<F)
endif

help:
	@echo -e "Usage: make [target] [options]"
	@echo -e ""
	@echo -e "Targets:"
	@echo -e "\t\e[34mscaffold\e[0m\t\tInitialise a new C project."
	@echo -e "\t\e[34mtest_<test_suite>\e[0m\tRun only this test suite."
	@echo -e "\t\e[34mtest\e[0m\t\t\tRun all tests."
	@echo -e "\t\e[34mhelp\e[0m\t\t\tPrint this message."
	@echo -e ""
	@echo -e "Options:"
	@echo -e "\t\e[35mWATCH\e[0m\t\t\tWhen set to true the test will be ran in watch mode. Only works with individual suites."
	@echo -e ""
	@echo -e "Test suites:"
	@echo -e "\e[32m$(ALL_TESTS)\e[0m" | column -t

scaffold:
	@echo -e "\e[34mScaffolding new C project...\e[0m"
	@echo -e "\e[90mCreating \e[0msrc\e[90m directory..."
	@mkdir -p src
	@echo -e "Creating \e[0mtest\e[90m directory..."
	@mkdir -p test
	@echo -e "Creating \e[0m.gitignore\e[90m..."
	@touch .gitignore
	@echo -e "build/\n" > .gitignore
	@echo -e "Creating \e[0m\"Hello, World\"\e[90m..."
	@echo -e "\e[32mDone.\e[0m"