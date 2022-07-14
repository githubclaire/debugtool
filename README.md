# debugtool

# Build dos tool in windows
open cmd and change path to .watcom, run batch file build.bat
````
build all
````
# Build dos tool with docker in windows
````
docker pull dockerhy/watcom-docker
````
````
docker run --rm -v %cd%:/usr/src/myapp -w /usr/src/myapp dockerhy/watcom-docker wmake -f makefile_watcom
````
# Build linux tool with docker in windows
 ````
docker pull dockerhy/linux-dbg
````
````
docker run --rm -v %cd%:/usr/src/myapp -w /usr/src/myapp dockerhy/linux-dbg make
````

# Tool Usage
print help info
````
help/?
````

Quit
````
quit
````

Print temperature
````
ts
````

Print Vcore
````
vcore
````

Read/Write clock
````
clock d/e/v
````

MIU test connection
````
memtest
````

Program fw to spi flash
````
prog xxx.rom/xxx.bin
````

Dump data from spi flash
````
dump xxx.rom/xxx.bin
````

Read/Write register
````
mmio xxxx
````

I2c read/write data
````
i2c index addr subaddr data
````

Print all info(pcie info/mem info/vcore/clk/temp)
````
print


````
efuse hdcp 1.4/2.2 key -dos
````
efuse 
