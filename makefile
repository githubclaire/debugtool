glenfly_tool_debug:main.c
	gcc -Wall -o glenfly_tool_debug main.c cmd.c pcie.c ini.c share.c common.c md5.c clk.c memtest.c spi_flash.c ts.c flash.c /usr/lib/x86_64-linux-gnu/libpci.a -lz -ludev 
clean:
	rm -fr *.o *~ glenfly_tool_debug
