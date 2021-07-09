#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


typedef struct history* history_link;
typedef struct history {
	char command[200];
	int count;
	history_link link;
}History;
history_link head=NULL;//history list�� ���
history_link tail=NULL;//history list�� ����

typedef struct opcode_entry* opcode_link;
typedef struct opcode_entry {
	char mnemonic[10];
	int opcode;
	opcode_link link;
}Opcode_Entry;
Opcode_Entry* opcode_table[20];//opcode hash table


char command[200];//��ü shell command ����
char order[200];//��ü shell command�� ��ɾ� �κи� ����
char last_part[200];//��ü shell command�� ��ɾ� ������ �κ� ����.
char argument[4][50];//��ü shell command�� start �κ� ����, argument[3]���� �����Ⱚ
char mnemonic[10];
int start, end;//dump�� ���� start, end�� ����
int address, value;//edit�� ���� address�� value ����
int base_address = 0;//������ address + 1������ ��� �ִ� �ּ� �� ����� ������ �ּ�.
unsigned char memory_space[1048576];//������ �޸� ����


void get_normalize_command();
void get_order_of_command();
void get_normalize_last_part();

int check_is_right_hexargument(char* argument);
int check_argument_has_noblank(char* argument);
int string_to_int(char* str);

void store_history(int count);

void init_opcode_table();
int hash_function(char *mnemonic);

void help();
void dir();
void quit();
void history();
void dump();
void dump_start();
void dump_start_end(int count);
void edit_address_value();
void fill_start_end_value();
void reset();
void opcode_mnemonic();
void opcodelist();

