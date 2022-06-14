#ifndef _CMD_H_
#define _CMD_H_


#define MAX_PARAM_COUNT	12
#define STRINGBUFFERSIZE 512

typedef int (do_dbg_func)(unsigned char **, unsigned int);

typedef struct cmd_struct {
	unsigned char      * name;
	do_dbg_func        * execute;
} DBG_CMD_STRUCT;

int do_dbg_help(unsigned char * cmd[], unsigned int param_count);
int do_dbg_quit(unsigned char * cmd[], unsigned int param_count);
int do_dbg_init(unsigned char * cmd[], unsigned int param_count);
int do_dbg_mmio(unsigned char * cmd[], unsigned int param_count);
int do_dbg_ts(unsigned char * cmd[], unsigned int param_count);
int do_dbg_voltage(unsigned char * cmd[], unsigned int param_count);
int do_dbg_print_info(unsigned char * cmd[], unsigned int param_count);
int do_dbg_flash(unsigned char * cmd[], unsigned int param_count);
int do_dbg_program_fw(unsigned char * cmd[], unsigned int param_count);
int do_dbg_clk(unsigned char * cmd[], unsigned int param_count);
int do_dbg_mem_test(unsigned char * cmd[], unsigned int param_count);


void CToolParserCmd(void);

#endif