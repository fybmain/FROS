IMAGE_IMG_NAME = fros.img

IMAGE_IMG_PATH = $(IMAGE_BUILD_DIR)/$(IMAGE_IMG_NAME)
IMAGE_FS_PATH = $(IMAGE_BUILD_DIR)/fs

IMAGE_FS_FILES =

define image_add_file_script =
IMAGE_FS_FILES += $$(IMAGE_FS_PATH)/$2
$$(dir $$(IMAGE_FS_PATH)/$2):
	mkdir -p $$@
$$(IMAGE_FS_PATH)/$2: $1 | $$(dir $$(IMAGE_FS_PATH)/$2)
	cp -r $$< $$@
endef
image_add_file = $(eval $(call image_add_file_script,$1,$2))

$(call image_add_file,$(LOADER_EFI_PATH),efi/boot/bootx64.efi)

$(IMAGE_BUILD_DIR):
	mkdir $@

$(IMAGE_FS_PATH): | $(IMAGE_BUILD_DIR)
	mkdir $@

$(IMAGE_IMG_PATH): | $(IMAGE_BUILD_DIR)
$(IMAGE_IMG_PATH): $(IMAGE_FS_FILES) 
	mkfs.fat -F 32 -C $@.$$$$ 40960 ; \
	mcopy -s -v -i $@.$$$$ $(IMAGE_FS_PATH)/* :: ; \
	mv $@.$$$$ $@

$(IMAGE_BUILD_DIR)/default: $(IMAGE_BUILD_DIR)/all
$(IMAGE_BUILD_DIR)/all: $(IMAGE_IMG_PATH)
$(IMAGE_BUILD_DIR)/everything: $(IMAGE_BUILD_DIR)/all $(IMAGE_BUILD_DIR)/clean
$(IMAGE_BUILD_DIR)/clean:
	rm -rf $(IMAGE_FS_PATH)
	rm -f $(IMAGE_IMG_PATH)*

