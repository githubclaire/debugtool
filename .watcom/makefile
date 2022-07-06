
OUTPUTPATH=$(CODEPATH)\.watcom

CC=wcc386
LINK=link
WLINK=wlink

OBJ =clk.obj cmd.obj common.obj flash.obj i2c.obj ini.obj main.obj
OBJ += md5.obj memtest.obj pcie.obj share.obj spi_flash.obj ts.obj

EXECUTABLE = glendbg.exe

CC_OPT=-w4 -e25 -zq -od -d2 -6r -bt=dos -mf -fo=$(OUTPUTPATH)\$*.obj

LINK_OPT=-nologo -out:$@
WLINK_OPT=d all sys dos32a op m op maxe=25 op q op symf @glendbg.lk1 

ALL:    $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
    @echo "Linking all obj..."
    @%write glendbg.lk1
    @%append glendbg.lk1
    @$(WLINK) name glendbg $(WLINK_OPT) File $(OUTPUTPATH)\*.obj
    @del /q $(OUTPUTPATH)\*.obj > nul
    @del /q $(OUTPUTPATH)\*.err > nul
CLEAN:
    @if exist "$(OUTPUTPATH)\glendbg.*" (del /q $(OUTPUTPATH)\glendbg.* > nul)

$(OBJ)
    @$(CC) $(CC_OPT)  $(CODEPATH)\$*.c
    @echo $*.c
