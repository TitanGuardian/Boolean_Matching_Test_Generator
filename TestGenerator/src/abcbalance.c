#include <stdio.h>
#include <time.h>
#include <string.h>

#define ARRCLEAN for (i = 0 ; i <1000 ; ++i) { \
                     Command[i]='\0'; }
void Abc_Start();
void Abc_Stop();

typedef struct Abc_Frame_t_ Abc_Frame_t;

Abc_Frame_t * Abc_FrameGetGlobalFrame();
int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );
 
int balanceDD  () {     
    Abc_Frame_t * pAbc;
    char * pFileName1= "./out/TestA.v",
         * pFileName2= "./out/TestB.v";
    char Command[1000];
    int i;
    Abc_Start();
    pAbc = Abc_FrameGetGlobalFrame();
    
    sprintf(Command, "read %s", pFileName1);
    if(Cmd_CommandExecute(pAbc, Command)){
        fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
	ARRCLEAN;
    sprintf(Command, "balance");
    if(Cmd_CommandExecute(pAbc, Command)){
	fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
    ARRCLEAN;
    sprintf(Command, "write_verilog %s", pFileName1);
    if(Cmd_CommandExecute(pAbc, Command)){
	fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
    ARRCLEAN;
    sprintf(Command, "read %s", pFileName2);
    if(Cmd_CommandExecute(pAbc, Command)){
	fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
    ARRCLEAN;
    sprintf(Command, "balance");
    if(Cmd_CommandExecute(pAbc, Command)){
	fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
    ARRCLEAN;
    sprintf(Command, "write_verilog %s", pFileName2);
    if(Cmd_CommandExecute(pAbc, Command)){
	fprintf(stdout , "Cannot execute command \"%s\".\n ",Command);
	return 1;
    }
    Abc_Stop();
    return 0;
 }
