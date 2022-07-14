
OUTPUTPATH=$(CODEPATH)\.watcom

CC=wcc386
LINK=link
WLINK=wlink

OBJ = clk.obj cmd.obj common.obj flash.obj i2c.obj ini.obj main.obj
OBJ += md5.obj memtest.obj pcie.obj share.obj spi_flash.obj ts.obj efuse.obj cts.obj

PROJ = glendbg
EXECUTABLE = $(PROJ).exe

CC_OPT=-w4 -e25 -zq -od -d2 -6r -bt=dos -mf -fo=$(OUTPUTPATH)\$*.obj

LINK_OPT=-nologo -out:$@
WLINK_OPT=d all sys dos32a op m op maxe=25 op q op symf @$(PROJ).lk1

ALL: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	@echo "Linking all obj..."
	@%write $(PROJ).lk1
	@%append $(PROJ).lk1
	@$(WLINK) name $(PROJ) $(WLINK_OPT) File $(OUTPUTPATH)\*.obj
	@if exist "$(OUTPUTPATH)\*.obj" del /q $(OUTPUTPATH)\*.obj > nul
	@if exist "$(OUTPUTPATH)\*.err" del /q $(OUTPUTPATH)\*.err > nul
CLEAN:
	@if exist "$(OUTPUTPATH)\$(PROJ).*" (del /q $(OUTPUTPATH)\$(PROJ).* > nul)

$(OBJ)
	@$(CC) $(CC_OPT)  $(CODEPATH)\$*.c
	@echo $*.c
