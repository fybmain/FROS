MAKEFLAGS += -r -R

COMMON_C_COMPILER = clang -c
COMMON_C_FLAGS =
COMMON_C_FLAGS += -Wall -Wextra
COMMON_C_FLAGS += -nostdinc

COMMON_LINK_LINKER = ld.lld
COMMON_LINK_FLAGS =
COMMON_LINK_FLAGS += -nostdlib
COMMON_LINK_FLAGS += --warn-common

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

define include_subproject_script =

$1_SRC_DIR=$$(PROJECT_SRC_DIR)/$2
$1_BUILD_DIR=$$(PROJECT_BUILD_DIR)/$2

.PHONY: $$($1_BUILD_DIR)/default
.PHONY: $$($1_BUILD_DIR)/all
.PHONY: $$($1_BUILD_DIR)/clean
.PHONY: $$($1_BUILD_DIR)/everything

all: $$($1_BUILD_DIR)/all
clean: $$($1_BUILD_DIR)/clean

include $$($1_SRC_DIR)/build.mk

endef
include_subproject = $(eval $(call include_subproject_script,$1,$2))

$(call include_subproject,LOADER,loader)
$(call include_subproject,KERNEL,kernel)
$(call include_subproject,IMAGE,image)

.PHONY: default all clean everything
default: all
.DEFAULT_GOAL = default

everything: all run clean

ifdef CONFIG_TARGET_SWITCH_RUN
run: $(IMAGE_IMG_PATH)
	qemu-system-x86_64 -hda $(IMAGE_IMG_PATH) -bios $(UEFI_FIRMWARE_IMAGE_PATH)
endif

