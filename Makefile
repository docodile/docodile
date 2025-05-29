PROGRAM := gendoc
SRC_DIR := src
BUILD_DIR := $(SRC_DIR)/build
DEBUG_DIR := $(BUILD_DIR)/debug
DIST_DIR := dist
SRC := $(wildcard $(SRC_DIR)/*.c)

$(DEBUG_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(DEBUG_DIR)
	@gcc -m64 -g -o $@ $^ -DLOG_LEVEL=4

$(BUILD_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(BUILD_DIR)
	@gcc -m64 -o $@ $^ -lc

build: $(BUILD_DIR)/$(PROGRAM)
debug: $(DEBUG_DIR)/$(PROGRAM)

watch:
	@find src docs gendoc.config | entr -rs "make build && ./src/build/gendoc"

clean:
	@rm -rf $(BUILD_DIR) $(DIST_DIR) ./.site ./*.tar.gz

package: build
	@mkdir -p dist
	@cp $(BUILD_DIR)/$(PROGRAM) $(DIST_DIR)
	@tar -czvf $(PROGRAM).tar.gz -C $(DIST_DIR) $(PROGRAM)

install: package
	@sudo cp $(DIST_DIR)/$(PROGRAM) /usr/local/bin/
	@sudo chmod +x /usr/local/bin/$(PROGRAM)
	@echo "Installed gendoc"