CPPFLAGS += -D__ECO32__ -static

board-$(CONFIG_GENERIC) := generic

KALLSYMS += --symbol-prefix=_

KBUILD_DEFCONFIG := eco32_defconfig

archprepare: maketools

PHONY += maketools

ifneq ($(board-y),)
BOARD  := arch/eco32/boards/$(board-y)/
else
BOARD  :=
endif

common-y += $(BOARD)
common-y += arch/eco32/lib/
common-y += arch/eco32/cpu/

lds-y += arch/eco32/cpu/barebox.lds

common-$(CONFIG_BUILTIN_DTB) += arch/eco32/dts/

dts := arch/eco32/dts

TEXT_BASE = $(CONFIG_ARCH_TEXT_BASE)
CPPFLAGS += -DTEXT_BASE=$(TEXT_BASE)

%.dtb: scripts
	$(Q)$(MAKE) $(build)=$(dts) $(dts)/$@

loader.bin: barebox.bin
	$(Q)echo "  AS      loader.o"
	$(eval BAREBOX_SIZE_IN_SECTORS := \
	$(shell echo "a=$(shell stat -c%s $?); b=512; if (a%b) a/b+1 else 1/b" | bc))
	$(Q)$(CC) -o arch/eco32/loader/loader arch/eco32/loader/loader.S \
	-T arch/eco32/loader/loader.lds -nostdlib \
	-DCONFIG_ARCH_TEXT_BASE=$(TEXT_BASE) \
	-DBAREBOX_SIZE_IN_SECTORS=$(BAREBOX_SIZE_IN_SECTORS)
	$(Q)echo "  OBJCOPY loader"
	$(Q)$(OBJCOPY) -O binary arch/eco32/loader/loader arch/eco32/loader/loader.bin

barebox.pbootblk: barebox.bin loader.bin
	$(Q)echo "  BUILD   barebox.pbootblk"
	$(Q)cat arch/eco32/loader/loader.bin barebox.bin > $@

barebox.mbootblk: barebox.bin loader.bin
	$(Q)dd if=/dev/zero of=/tmp/zeros bs=512 count=1 2>/dev/null
	$(Q)echo "  BUILD   barebox.mbootblk"
	$(Q)cat arch/eco32/loader/loader.bin /tmp/zeros barebox.bin > $@

archclean:
	$(Q)echo "  CLEAN   arch/eco32/loader"
	$(Q)rm -f arch/eco32/loader/loader
	$(Q)rm -f arch/eco32/loader/loader.o
	$(Q)rm -f arch/eco32/loader/loader.bin
	$(Q)rm -f barebox.pbootblk
	$(Q)rm -f barebox.mbootblk
