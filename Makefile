PROGRAM := gendoc
SRC_DIR := src
BUILD_DIR := $(SRC_DIR)/build
DEBUG_DIR := $(BUILD_DIR)/debug
DIST_DIR := dist
SCRIPTS_DIR := scripts
DOCS_DIR := docs
TEMPLATES_DIR := templates
SRC := $(wildcard $(SRC_DIR)/*.c)

$(DEBUG_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(DEBUG_DIR)
	@gcc -m64 -fsanitize=address -g -o $@ $^ -DLOG_LEVEL=4

$(BUILD_DIR)/$(PROGRAM): $(SRC)
	@mkdir -p $(BUILD_DIR)
	@gcc -m64 -o $@ $^ -lc

build: $(BUILD_DIR)/$(PROGRAM)
debug: $(DEBUG_DIR)/$(PROGRAM)

watch:
	@find src docs gendoc.config | entr -rs "make build && ./src/build/gendoc serve"

clean:
	@rm -rf $(BUILD_DIR) $(DIST_DIR) ./.site ./*.tar.gz

package: build
	@mkdir -p $(DIST_DIR)
	@cp -r $(BUILD_DIR)/$(PROGRAM) $(DOCS_DIR) $(TEMPLATES_DIR) $(SCRIPTS_DIR)/install.sh $(DIST_DIR)
	@cp gendoc.config.example $(DIST_DIR)/gendoc.config
	@tar -czvf $(PROGRAM).tar.gz -C $(DIST_DIR) $(PROGRAM)

install: package
	@sudo cp $(DIST_DIR)/$(PROGRAM) /usr/local/bin/
	@sudo rm -rf /etc/gendoc
	@sudo cp -r $(DIST_DIR) /etc/gendoc
	@sudo chmod +x /usr/local/bin/$(PROGRAM)
	@echo "Installed gendoc"
	@echo "/etc/gendoc:"
	@ls /etc/gendoc