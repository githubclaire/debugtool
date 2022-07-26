########
# 
# -----------------------------------------------
# REFERENCE:
# 1. dependency generation
#    http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
########

F ?= 0
CHOOSE_SYSTEM = $(shell echo $$(($(F))))
OBJTREE := .objs/
DEPDIR := .deps

CC = gcc
LD = gcc
CFLAGS = -Wall
LDFLAGS = -lz -ludev -lpci
ifneq ($(CHOOSE_SYSTEM),0)
LDFLAGS = /home/system/Desktop/pciutils-3.7.0/lib/libpci.a -lz -ludev
endif


COBJS = main.o cmd.o pcie.o share.o common.o clk.o ini.o md5.o  spi_flash.o flash.o ts.o memtest.o 
COBJS += i2c.o efuse.o cts.o
OBJS = $(addprefix $(OBJTREE),$(COBJS))

EXECUTABLE := glenfly_tool_debug

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

.PHONY: all clean


all:  $(OBJTREE)
	$(MAKE) $(EXECUTABLE)

$(OBJTREE):
	@mkdir -p $@

$(EXECUTABLE) : $(OBJS)
	@echo "[LD] $@"
#	$(LD) $(CFLAGS) $(CFILE) $(AFILE) -o $@ $(LDFLAGS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

$(OBJTREE)%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	@echo "[CC] $@"
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@
	$(POSTCOMPILE)

$(DEPDIR):
	@echo "dep"
	@mkdir -p $@

clean:
	@echo "cleaning."
	@rm -f $(EXECUTABLE) $(DEPDIR)/*.d $(OBJTREE)*.o
	@rmdir $(DEPDIR) $(OBJTREE)

DEPFILES := $(COBJS:%.o=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))

