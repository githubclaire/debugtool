#ifndef _CMD_H_
#define _CMD_H_


#define MAX_PARAM_COUNT	12
#define STRINGBUFFERSIZE 512

#define UPKEY		(0x80|0x48)
#define DOWNKEY		(0x80|0x50)
#define LEFTKEY		(0x80|0x4B)
#define RIGHTKEY	(0x80|0x4D)
#define DEBUGKEY	(0x4)
#define F3KEY		(0x80|0x3D)

typedef int (do_dbg_func)(char **, unsigned int);

typedef struct cmd_struct {
	char      * name;
	do_dbg_func        * execute;
} DBG_CMD_STRUCT;

int do_dbg_help(char * cmd[], unsigned int param_count);
int do_dbg_quit(char * cmd[], unsigned int param_count);
int do_dbg_init(char * cmd[], unsigned int param_count);
int do_dbg_mmio(char * cmd[], unsigned int param_count);
int do_dbg_ts(char * cmd[], unsigned int param_count);
int do_dbg_voltage(char * cmd[], unsigned int param_count);
int do_dbg_print_info(char * cmd[], unsigned int param_count);
int do_dbg_flash(char * cmd[], unsigned int param_count);
int do_dbg_program_fw(char * cmd[], unsigned int param_count);
int do_dbg_dump_fw(char * cmd[], unsigned int param_count);
int do_dbg_clk(char * cmd[], unsigned int param_count);
int do_dbg_mem_test(char * cmd[], unsigned int param_count);
int do_dbg_i2c_test(char * cmd[], unsigned int param_count);
int do_dbg_dp_test(char * cmd[], unsigned int param_count);
int do_dbg_efuse_test(char * cmd[], unsigned int param_count);

void CToolParserCmd(void);
int ProcessString(char* buffer);
void helpinfo(void);
char GetKey(void);
#endif
