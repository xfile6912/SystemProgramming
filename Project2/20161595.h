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
history_link head = NULL;//history list�� ���
history_link tail = NULL;//history list�� ����

typedef struct opcode_entry* opcode_link;
typedef struct opcode_entry {
	char mnemonic[10];
	char format[10];
	int opcode;
	opcode_link link;
}Opcode_Entry;
Opcode_Entry* opcode_table[20];//opcode hash table

typedef struct symbol* symbol_link;
typedef struct symbol {
	char label[31];//30��+\0;
	int location_counter;//location counter;
	symbol_link link;
}Symbol;
symbol_link symbol_head = NULL;//symbol table list
symbol_link symbol_tail = NULL;
symbol_link real_symbol_head = NULL;//���� ���߿� ������ symbol list(��½� ���)
symbol_link real_symbol_tail = NULL;


typedef struct object_code* object_code_link;
typedef struct object_code {
	char object_code[11];
	int location_counter;//location counter;
	int is_last;//record�� ������������ ��Ÿ���� flag//1�̸� �������̶� ���ĺ��ʹ� �����ٿ� ��µǾ����.
	object_code_link link;
}Object_Code;
object_code_link object_code_head = NULL; //object code���� list�ν� ����(object code ����� ���)
object_code_link object_code_tail = NULL;

typedef struct modi_code* modi_link;
typedef struct modi_code {
	int address;//4������ address
	modi_link link;
}Modi_Code;
modi_link modi_head = NULL;//object Code�� relocation�� ���� modify �� ���� ���� �����ϴ� list
modi_link modi_tail = NULL;

char command[200];//��ü shell command ����
char order[200];//��ü shell command�� ��ɾ� �κи� ����
char last_part[200];//��ü shell command�� ��ɾ� ������ �κ� ����.
char argument[4][50];//��ü shell command�� start �κ� ����, argument[3]���� �����Ⱚ
char mnemonic[10];
int start, end;//dump�� ���� start, end�� ����
int address, value;//edit�� ���� address�� value ����
int base_address = 0;//������ address + 1������ ��� �ִ� �ּ� �� ����� ������ �ּ�.
char filename[50];//filename�� ����
unsigned char memory_space[1048576];//������ �޸� ����
char program_name[50];//program �̸� ����.
int start_address, program_length;//���α׷� �����ּ�, ����


void get_normalize_command();
void get_order_of_command();
void get_normalize_last_part();


int check_is_right_hexargument(char* argument);
int check_argument_has_noblank(char* argument);
int string_to_int(char* str);


void store_history(int count);

void init_opcode_table();
int hash_function(char* mnemonic);


void free_list();
void print_error(int line_num, char* description);
void parse_instruction(char* instruction, char* label, char* opcode, char* operand);
int find_format(char* mnemonic);
int find_opcode(char *mnemonic);
int add_symbol_to_symbol_list(char* label, int line_num, int location_counter);
void print_record(FILE* obj_fp);
int get_register_value(char* reg);
void add_object_code(char* object_code, int location_counter);
void add_modi_code(int address);
int pass1();
int pass2();


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
void type();
void assemble();
void symbol();



////////////////////////////////////////////////
//�Լ���: init_opcode_table                   //
//���: opcode.txt�κ��� opcode������ �о�ͼ�//
//opcode table(hash_table)�� �����ϴ� ����.   //
//����: ����                                  // 
////////////////////////////////////////////////
void init_opcode_table()
{
	FILE* fp = fopen("opcode.txt", "r");//file�� ������
	int res, opcode;
	char mnemonic[10];
	char format[10];
	int hash_key;
	while (1)
	{
		res = fscanf(fp, "%x %s %s", &opcode, mnemonic, format);//���Ϸκ��� opcode, mnemonic, format �о��
		if (res == EOF)//������ ���̸� �о���°� ����
		{
			break;
		}
		hash_key = hash_function(mnemonic);//hash_key�� ����� ��.

		Opcode_Entry* opcode_entry = (Opcode_Entry*)malloc(sizeof(Opcode_Entry));
		opcode_entry->opcode = opcode;
		strcpy(opcode_entry->mnemonic, mnemonic);
		strcpy(opcode_entry->format, format);
		opcode_entry->link = NULL;
		//�о�� ������ ���ο� opcode entry�� ����.


		if (opcode_table[hash_key] == NULL)//���� �ƹ��͵� ����� ���� ���ٸ� �ٷ� ����
		{
			opcode_table[hash_key] = opcode_entry;
		}
		else//����� ���� �̹� �ִٸ� �ش� ����Ʈ�� ���� ����
		{
			Opcode_Entry* ptr = opcode_table[hash_key];
			for (; ptr->link != NULL; ptr = ptr->link);
			ptr->link = opcode_entry;
		}
	}
	fclose(fp);
}

/////////////////////////////////////////
//�Լ���: hash_function                //
//���: mnemonic�� �޾� �̸� ��������  //
//������ �ؽ�Ű��(0~19)�� �����ϴ� ����//
//����: �ؽ�Ű��                       //
/////////////////////////////////////////
int hash_function(char* mnemonic)
{
	int hash_key;
	int i;
	int temp_value = 0;
	for (i = 0; i < strlen(mnemonic); i++)
	{
		temp_value += mnemonic[i] / 2;
	}

	double a = 0.618934;
	a = temp_value * a - (int)(temp_value * a);
	a *= 512;
	hash_key = (int)a % 20;//������ hash key���� ������ ����
	return hash_key;
}

////////////////////////////////////////////////// 
//�Լ���: string_to_int                         //    
//���: 16���� string�� �޾� �̸�               //
//10������ �ٲپ� �ִ� ����.                    //
//����: 10���� ����(�ٲܼ� ���ٸ�  -1)          //
//////////////////////////////////////////////////
int string_to_int(char* str)
{
	int decimal = 0;
	int i;
	int exp = 1;
	if (strlen(str) == 0)
		return -1;//���̰� 0�̸� hexdecimal ���°� �ƴ�.
	for (i = strlen(str) - 1; i >= 0; i--) {

		if ('a' <= str[i] && str[i] <= 'f') {//�ҹ����ΰ��
			decimal += exp * (str[i] + 10 - 'a');
			exp *= 16;
		}
		else if ('A' <= str[i] && str[i] <= 'F') {//�빮���� ���
			decimal += exp * (str[i] + 10 - 'A');
			exp *= 16;
		}

		else if ('0' <= str[i] && str[i] <= '9') {
			decimal += exp * (str[i] - '0');
			exp *= 16;
		}
		else {	//hex decimal ���°� �ƴϹǷ� -1 ����
			return -1;
		}
	}

	return decimal;
}

//////////////////////////////////////////////
//�Լ���: check_is_right_hexargument        //
//���: argument�� �޾� �ش� argument��     //
//������ 16���� argument���� �Ǵ��ϴ� ����  //
//����: 1(������ 16���� ����),              //
//      0(�������� ���� 16���� ����)        //
//////////////////////////////////////////////
int check_is_right_hexargument(char* argument)
{
	int flag = check_argument_has_noblank(argument);

	if (string_to_int(argument) == -1)//hex�� �ٲܼ� ���� argument��� �߸��� argument
		flag = 0;
	return flag;
}

//////////////////////////////////////////////
//�Լ���: check_argument_has_noblank        //
//���: argument�� �޾� �ش� argument��.    //
//��ĭ�� ���� argument���� �Ǵ��ϴ� ����    //
//����: 1(��ĭ�� ����, ������ ����),        //
//      0(��ĭ�� ����, �������� ���� ����   //
//////////////////////////////////////////////
int check_argument_has_noblank(char* argument)
{
	int flag = 1;//������ ���� argument�̸� 1, �ƴϸ� 0;
	char temp[200];//argument�� �ӽ÷� ������ ����


	strcpy(temp, argument);

	int start = 0;	//argument���� ��ĭ�� �ƴ� ���� index
	int end = strlen(argument) - 1;//argument���� ��ĭ�� �ƴ� ������ index
	while (temp[start] == ' ' || temp[start] == '\t')
	{
		start++;
	}
	while (temp[end] == ' ' || temp[end] == '\t')
	{
		end--;
	}
	if (start > end)//start�� end���� ũ�� �������θ� �̷���� argument�̹Ƿ� �߸��� argument
		return 0;
	strncpy(argument, temp + start, end - start + 1);//argumnet�� �յ� ���� ����
	argument[end - start + 1] = '\0';

	for (int i = 0; i < strlen(argument); i++)
	{
		if (argument[i] == ' ' || argument[i] == '\t')//�յ� ������ �����ߴµ� �ȿ� ������ �ִٸ� �߸��� argument��
		{
			flag = 0;
			break;
		}
	}
	return flag;
}

//////////////////////////////////////////////
//�Լ���: get_normalize_command             //
//���: command�� �յ� ������ �����ϴ� ���� //
//����: ����.                               //
//////////////////////////////////////////////
void get_normalize_command()
{
	printf("sicsim> ");
	fgets(command, 200, stdin);//command�� ��ü ��ɾ� �޾ƿ�
	command[strlen(command) - 1] = '\0';//fgets�� ������ \n�� ���Ƿ� \n�� \0���� �ٲپ���

	char temp[200];//command�� �ӽ÷� ������ ����
	strcpy(temp, command);

	int start = 0;	//command���� ��ĭ�� �ƴ� ���� index
	int end = strlen(command) - 1;//command���� ��ĭ�� �ƴ� ������ index
	if (start > end)
	{
		command[0] = '\0';
		return;
	}
	while (temp[start] == ' ' || temp[start] == '\t')
	{
		start++;
	}
	while (temp[end] == ' ' || temp[end] == '\t')
	{
		end--;
	}

	if (start > end)
	{
		command[0] = '\0';
		return;
	}


	strncpy(command, temp + start, end - start + 1);//�յ� ���� ����
	command[end - start + 1] = '\0';
}

//////////////////////////////////////////////
//�Լ���: get_order_of_command              //
//���: command�κ��� ���� ��ɾ�� ����  //
//      �� order�� �����ϴ� ����            //
//����: ����.                               //
//////////////////////////////////////////////
void get_order_of_command()
{
	int i;
	for (i = 0; command[i] != ' ' && command[i] != '\t' && command[i] != '\0'; i++);
	strncpy(order, command, i);
	order[i] = '\0';
}

//////////////////////////////////////////////
//�Լ���: get_normalize_last_part           //
//���: command���� ���� ��ɾ ������ �� //
//���� ��Ʈ�� �յ� ������ �����ϰ� last_part//
//�� �����ϴ� ����                          //
//����: ����.                               //
//////////////////////////////////////////////
void get_normalize_last_part()
{
	int i;
	for (i = 0; command[i] != ' ' && command[i] != '\t' && command[i] != '\0'; i++); //command���� order�κ� ����
	int start = i;	//������ part���� ��ĭ�� �ƴ� ���� index
	int end = strlen(command) - 1;//������ part���� ��ĭ�� �ƴ� ������ index
	if (start > end)
	{
		last_part[0] = '\0';
		return;
	}
	while (command[start] == ' ' || command[start] == '\t')
	{
		start++;
	}//������ �κ��� �պκ��� ���� ����
	while (command[end] == ' ' || command[end] == '\t')
	{
		end--;
	}//������ �κ��� �޺κ��� ���� ����
	if (start > end)
	{
		last_part[0] = '\0';
		return;
	}
	strncpy(last_part, command + start, end - start + 1);//�յ� ���� ����
	last_part[end - start + 1] = '\0';
}

//////////////////////////////////////////////
//�Լ���: store_history                     //
//���: ����� ��ɾ list�� �����ϴ� ����//
//����: ����.                               //
//////////////////////////////////////////////
void store_history(int count) {
	History* data = (History*)malloc(sizeof(History));
	int space_index;//��ĭ�� ������ �ε���
	int i;

	strcpy(data->command, order);
	space_index = strlen(data->command);
	if (count != 0)//���� ����
	{
		data->command[space_index] = ' ';
		for (i = 0; i < count; i++)
		{
			strcpy((data->command) + space_index + 1, argument[i]);
			space_index = strlen(data->command) + 1;
			if (i != count - 1)//�������� �ƴѰ�쿡�� ', ' �־������.
			{
				data->command[space_index - 1] = ',';
				data->command[space_index] = ' ';
			}

		}

	}
	data->link = NULL;
	if (head == NULL)//head�� null�̸� head�� ��������.
	{
		data->count = 1;
		head = data;
		tail = data;
	}
	else //head�� null�� �ƴϸ� tail�� ��������.
	{
		data->count = tail->count + 1;
		tail->link = data;
		tail = data;
	}
}

//////////////////////////////////////////////
//�Լ���: help                              //
//���: shell���� ���� ������ ��ɾ�� ��� //
//����: ����.                               //
//////////////////////////////////////////////
void help()
{
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	//��ɾ�� ���
	store_history(0);
}

//////////////////////////////////////////////
//�Լ���: dir                               //
//���: ���� ���丮�� �ִ� ���ϵ� ���.   //
//����: ����.                               //
//////////////////////////////////////////////
void dir()
{
	DIR* dir = opendir(".");//���� ���丮
	struct dirent* file;
	struct stat file_stat;
	int count = 0;
	while ((file = readdir(dir)))//���ϵ� ��ȸ
	{
		count++;
		stat(file->d_name, &file_stat);//���� ����
		if (S_ISDIR(file_stat.st_mode))//directory���ڿ� / �ٿ���
		{
			printf("\t%s/", file->d_name);
		}
		else if (S_IXUSR & file_stat.st_mode)//excutable file�̶�� �ڿ� * �ٿ���
		{
			printf("\t%s*", file->d_name);
		}
		else//etc
		{
			printf("\t%s", file->d_name);
		}
		if (count % 4 == 0)
			printf("\n");
	}
	printf("\n");
	closedir(dir);

	store_history(0);
}

//////////////////////////////////////////////
//�Լ���: quit                              //
//���: �����Ҵ�� �޸𸮵��� deallocate    //
//����: ����.                               //
//////////////////////////////////////////////
void quit()
{
	//free opcode table
	int i;
	for (i = 0; i < 20; i++)//opcode table ��ȸ
	{
		Opcode_Entry* ptr = opcode_table[i];
		opcode_table[i] = NULL;
		while (ptr != NULL)//�����Ҵ�� ���� deallocate
		{
			Opcode_Entry* temp = ptr->link;
			free(ptr);
			ptr = temp;
		}
	}
	//free history list
	History* ptr = head;
	head = NULL;
	tail = NULL;
	while (ptr != NULL)//history ��ȸ�ϸ鼭 �����Ҵ�� ���� deallocate
	{
		History* temp = ptr->link;
		free(ptr);
		ptr = temp;
	}

	store_history(0);

}

////////////////////////////////////////////////
//�Լ���: history                             //
//���: ������� ����� ��ɾ�� ������� ���//
//����: ����.                                 //
////////////////////////////////////////////////
void history()
{
	store_history(0);
	History* ptr;
	//history list ��ȸ�ϸ鼭 ���� ��ɾ�� ���
	for (ptr = head; ptr != NULL; ptr = ptr->link)
	{
		printf("%d\t %s\n", ptr->count, ptr->command);
	}
}
//////////////////////////////////////////////
//�Լ���: dump                              //
//���: �Ҵ�Ǿ� �ִ� �޸𸮸� 10���� ���. //
//����: ����.                               //
//////////////////////////////////////////////
void dump()
{
	start = base_address;
	end = start + 159;//160��

	if (end > 0xfffff)//�޸� ������ end�� �Ѿ�ٸ�
	{
		end = 0xfffff;
	}
	dump_start_end(0);
}
//////////////////////////////////////////////
//�Լ���: dump_start                        //
//���: �Ҵ�Ǿ� �ִ� �޸𸮸� start �ּҺ� //
//      �� 10���� ���.                     //
//����: ����.                               //
//////////////////////////////////////////////
void dump_start()
{
	end = (start + 159);//10�����ΰ���

	if (end > 0xfffff)//�޸� ������ end�� �Ѿ�ٸ�
	{
		end = 0xfffff;
	}
	dump_start_end(1);
}
//////////////////////////////////////////////
//�Լ���: dump_start_end                    //
//���: �Ҵ�Ǿ� �ִ� �޸𸮸� start �ּҺ� //
//      �� end�ּұ��� ���                 //
//����: ����.                               //
//////////////////////////////////////////////
void dump_start_end(int count)//count�� ������ ����
{
	if (start < 0 || end < 0 || start > end || start > 0xfffff || end > 0xfffff) {//�߸��� �޸� ������ ���
		printf("wrong memory access!\n");
		return;
	}
	base_address = end + 1;//base address�� ������ �޸� �ּ�+1 �־���
	if (base_address > 0xfffff)
		base_address = 0;

	int line_addr;
	int addr;
	for (line_addr = start / 16 * 16; line_addr <= end / 16 * 16; line_addr = line_addr + 16)
	{
		printf("%05X", line_addr);//5�ڸ� memory line address ���
		for (addr = line_addr; addr < line_addr + 16; addr++)//�� line�� memory ���� ���� ���(16��)
		{
			if (addr<start || addr>end)//start�� end ���̿� ���� ���
			{
				printf("   ");
			}
			else//start�� end ���̿� �ִ°��
			{
				printf(" %02X", memory_space[addr]);
			}
		}
		printf(" ; ");
		for (addr = line_addr; addr < line_addr + 16; addr++)//�� line�� memory ���� ���� ascii code���(16��).
		{
			if (addr<start || addr>end)//start�� end ���̿� ���� ���
			{
				printf(".");
			}
			else if (memory_space[addr] < 0x20 || memory_space[addr]>0x7e)//ascii code �÷����� ��Ÿ���� �ʴ� �κ�
			{
				printf(".");
			}
			else//ascii code �÷����� ��Ÿ���� �κ�
			{
				printf("%c", memory_space[addr]);
			}
		}
		printf("\n");
	}
	store_history(count);
}

//////////////////////////////////////////////
//�Լ���: edit_address_value                //
//���: �޸��� address������ ���� value�� //
//      ������ ������ ����                  //
//����: ����.                               //
//////////////////////////////////////////////
void edit_address_value()
{
	if (address < 0x00000 || address>0xfffff)//�߸��� address access
	{
		printf("Wrong memory space address!\n");
		return;
	}
	if (value < 0x00 || value>0xff)//�߸��� value
	{
		printf("Wrong argument!\n");
		return;
	}
	memory_space[address] = value;//�޸� ������ �� ����
	store_history(2);
}
//////////////////////////////////////////////
//�Լ���: fill_start_end_value              //
//���: �޸��� start�������� end��������  //
//      ���� value�� ������ ������ ����     //
//����: ����.                               //
//////////////////////////////////////////////
void fill_start_end_value()
{
	if (start < 0 || end < 0 || start > end || start > 0xfffff || end > 0xfffff) {//�߸��� �޸� ������ ���
		printf("wrong memory access!\n");
		return;
	}
	if (value < 0x00 || value>0xff)//�߸��� value
	{
		printf("Wrong argument!\n");
		return;
	}

	int addr;
	for (addr = start; addr <= end; addr++)//start���� end������ memory������ ��� value�� set
	{
		memory_space[addr] = value;
	}
	store_history(3);
}
//////////////////////////////////////////////
//�Լ���: reset                             //
//���: �޸� ��ü�� ���� 0���� ����.      //
//                                          //
//����: ����.                               //
//////////////////////////////////////////////
void reset()
{
	int addr;
	for (addr = 0; addr < 1048576; addr++)//�޸� ��ü�� ���� 0���� ����
		memory_space[addr] = 0;
	store_history(0);
}

//////////////////////////////////////////////
//�Լ���: opcode_mnemonic                   //
//���: ��ɾ �ش��ϴ� opcode�� ���.    //
//                                          //
//����: ����.                               //
//////////////////////////////////////////////
void opcode_mnemonic()
{
	int hash_key = hash_function(mnemonic);//mnemonic�� ���� hash_keyã�Ƴ�
	int flag = 0;//ã������ 1, ������ 0;
	int opcode;

	Opcode_Entry* ptr = opcode_table[hash_key];
	for (; ptr != NULL; ptr = ptr->link)//opcode_table[hash_key]�� list ��ȸ�ϸ鼭 �ش� mnemonic�ִ��� ã��
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic�� �ִٸ� flag�� 1�� �ٲپ��ְ� break;
		{
			opcode = ptr->opcode;
			flag = 1;
			break;
		}
	}
	if (flag == 1)//�ִ� mnemonic�̶��
	{
		printf("opcode is %02X\n", opcode);
		store_history(1);
	}
	else//���� mnemonic�̶��
	{
		printf("Wrong Mnemonic!\n");
		return;
	}

}

//////////////////////////////////////////////
//�Լ���: opcodelist                        //
//���: opcode hash table�� ������ ���.    //
//                                          //
//����: ����.                               //
//////////////////////////////////////////////
void opcodelist()
{
	int i;
	for (i = 0; i < 20; i++)//opcode table ��ȸ
	{
		Opcode_Entry* ptr = opcode_table[i];
		printf("%3d: ", i);
		for (; ptr != NULL; ptr = ptr->link)//opcode_table[i]�� list ��ȸ�ϸ鼭 ���
		{
			printf("[%s, %02X]", ptr->mnemonic, ptr->opcode);
			if (ptr->link != NULL)//������ ����Ű�� ���� �ִٸ�
				printf(" -> ");
		}
		printf("\n");
	}
	store_history(0);
}

//////////////////////////////////////////////
//�Լ���: type                              //
//���: file name�� �ش��ϴ� ������ �о  //
//      ȭ�鿡 ���                         //
//����: ����.                               //
//////////////////////////////////////////////
void type()
{
	FILE* fp = fopen(filename, "r");//open file
	if (fp == NULL)//if file not exists in directory.
	{
		printf("%s not exists.\n", filename);
		return;
	}
	int res;
	char c;
	while (1)
	{
		res = fscanf(fp, "%c", &c);//read char.
		if (res == EOF)//if end of file.
			break;
		printf("%c", c);//print char
	}
	store_history(1);
}

//////////////////////////////////////////////
//�Լ���: assemble                          //
//���: file name�� �ش��ϴ� ������ �о  //
//      object file�� ������ ������ ����    //
//����: ����.                               //
//////////////////////////////////////////////
void assemble()
{
	char obj_filename[50];
	char lst_filename[50];
	int pass1_flag = 0;
	int pass2_flag = 0;


	//���� �̸��� ���˿� ���� �ʴ´ٸ� ���� ���
	if (strlen(filename) < 5 || strcmp(filename + strlen(filename) - 4, ".asm"))
	{
		printf("file name should form like *.asm.\n");
		return;
	}


	//obj_filename�� lst_filename �ʱ�ȭ
	strcpy(obj_filename, filename);
	strcpy(lst_filename, filename);
	strcpy(obj_filename + strlen(obj_filename) - 3, "obj");
	strcpy(lst_filename + strlen(obj_filename) - 3, "lst");
	

	//pass1�� ����
	pass1_flag = pass1();
	
	//���� pass1�� �����Ѵٸ� symbol table�� modify list��, object_code list, intermediate file�� ����
	if (pass1_flag == 0)
	{
		free_list();//free symbol table, modi list, object_code list, intermediate file
		return;
	}

	//pass2�� ����
	pass2_flag = pass2();

	//���� pass2�� �����Ѵٸ� symbol table�� modify list��, object_code list, intermediate file�� ����
	//object file�� lst file�� ����
	if (pass2_flag == 0)
	{
		free_list();//free symbol table, modi list, object_code list, intermediate file
		remove(obj_filename);
		remove(lst_filename);
		return;
	}

	//������

	Symbol* symbol_ptr = real_symbol_head;//���� real symbol���� ������.
	real_symbol_head = NULL;
	real_symbol_tail = NULL;
	while (symbol_ptr != NULL)//symbol list ��ȸ�ϸ鼭 �����Ҵ�� ���� deallocate
	{
		Symbol* temp = symbol_ptr->link;
		free(symbol_ptr);
		symbol_ptr = temp;
	}

	real_symbol_head = symbol_head;//��µǴ� symbol table �� ��Ÿ���� head ������Ʈ
	real_symbol_tail = symbol_tail;
	symbol_head = NULL;//�� free_list�� ���� �������� �ʵ��� ������ head�� null�� �ٲپ���.
	symbol_tail = NULL;

	free_list();//free symbol table, modi list, object_code list
	printf("[%s], [%s]\n", lst_filename, obj_filename);
	store_history(1);
}

//////////////////////////////////////////////
//�Լ���: symbol                            //
//���: ���� �ֱٿ� assemble�� symbol table //
//      �� ȭ�鿡 ���                      //
//����: ����.                               //
//////////////////////////////////////////////
void symbol()
{
	Symbol* ptr;
	//real symbol list ��ȸ�ϸ鼭 ���� ��ɾ�� ���

	if (real_symbol_head == NULL)//symbol table�� ���� �������� ����.
	{
		printf("Symbol Table not exists.\n");
		return;
	
	}

	for (ptr = real_symbol_head; ptr != NULL; ptr = ptr->link)
	{
		printf("\t%s\t %04X\n", ptr->label, ptr->location_counter);//�ɺ����̺� ��ȸ�ϸ鼭 ���.
	}
	store_history(0);
}

//////////////////////////////////////////////
//�Լ���: free_list                         //
//���: symbol table, intermediate file,    //
//      obcode list, modi list�� free����   //
//����: ����.                               //
//////////////////////////////////////////////
void free_list()
{
	//free symbol list
	Symbol* symbol_ptr = symbol_head;
	symbol_head = NULL;
	symbol_tail = NULL;
	while (symbol_ptr != NULL)//symbol list ��ȸ�ϸ鼭 �����Ҵ�� ���� deallocate
	{
		Symbol* temp = symbol_ptr->link;
		free(symbol_ptr);
		symbol_ptr = temp;
	}

	//free object code list
	Object_Code* object_ptr = object_code_head;
	object_code_head = NULL;
	object_code_tail = NULL;
	while (object_ptr != NULL)//object code list ��ȸ�ϸ鼭 �����Ҵ�� ���� deallocate
	{
		Object_Code* temp = object_ptr->link;
		free(object_ptr);
		object_ptr = temp;
	}

	//free modi code list
	Modi_Code* modi_ptr = modi_head;
	modi_head = NULL;
	modi_tail = NULL;
	while (modi_ptr != NULL)//modify list ��ȸ�ϸ鼭 �����Ҵ�� ���� deallocate
	{
		Modi_Code* temp = modi_ptr->link;
		free(modi_ptr);
		modi_ptr = temp;
	}

	remove("intermediate");
}
//////////////////////////////////////////////
//�Լ���: print_error                       //
//���: error line ��ȣ�� description�� �޾�//
//      error�� ȭ�鿡 ���                 //
//����: ����.                               //
//////////////////////////////////////////////
void print_error(int line_num, char* description)
{
	printf("[LINE:%d]%s.\n", line_num, description);
}

//////////////////////////////////////////////
//�Լ���: parse_instruction                 //
//���: assembly instruction�� �޾Ƽ� label,//
//      opcode(mnemonic), operand�� ������  //
//����: ����.                               //
//////////////////////////////////////////////
void parse_instruction(char* instruction, char* label, char* opcode, char* operand)
{
	int label_it = 0, opcode_it = 0, operand_it = 0;//label, opcode, operand iterator;
	int comma_flag = 0;//instruction�� comma�� ������ 1, ������ 0

	int i;

	if (!strcmp(instruction, "\n"))//�����̸� return;
		return;

	//comma�� �ִ��� ������ üũ
	for (i = 0; i < strlen(instruction); i++)
	{
		if (instruction[i] == ',')
		{
			comma_flag = 1;
			break;
		}
	}

	//label parsing
	for (i = 0; instruction[i] != ' ' && instruction[i] != '\t'; i++)
	{
		label[label_it++] = instruction[i];
	}
	label[label_it] = '\0';

	//��ĭ�� ������
	while (instruction[i] == ' ' || instruction[i] == '\t')
	{
		i++;
	}


	//opcode parsing
	for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
	{
		opcode[opcode_it++] = instruction[i];
	}
	opcode[opcode_it] = '\0';

	//��ĭ�� ������
	while (instruction[i] == ' ' || instruction[i] == '\t')
	{
		i++;
	}

	//operand parsing
	for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
	{
		operand[operand_it++] = instruction[i];
	}
	//comma�� ������ ,���� operand�� ������
	if (comma_flag == 1)
	{
		operand[operand_it++] = instruction[i++];
		for (; instruction[i] != ' ' && instruction[i] != '\t' && instruction[i] != '\n'; i++)
		{
			operand[operand_it++] = instruction[i];
		}
	}

	operand[operand_it] = '\0';

}


/////////////////////////////////////////////////////////
//�Լ���: find_format                                  //
//���: mnemonic�� �޾� �ش� opcode�� type ��ȯ        //
//����: 1(1����), 2(2����), 3(3/4����), 0(���� opcode) //
/////////////////////////////////////////////////////////
int find_format(char* mnemonic)
{
	int hash_key = hash_function(mnemonic);//mnemonic�� ���� hash_keyã�Ƴ�
	int format = 0;//1(1����), 2(2����), 3(3/4����), 0(���� mnemonic)

	Opcode_Entry* ptr = opcode_table[hash_key];
	//opcode_table[hash_key]�� list ��ȸ�ϸ鼭 �ش� mnemonic�ִ��� ã��
	for (; ptr != NULL; ptr = ptr->link)
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic�� �ִٸ� format�� ����
		{
			if (!strcmp(ptr->format, "1"))
				format = 1;
			else if (!strcmp(ptr->format, "2"))
				format = 2;
			else
				format = 3;
			break;
		}
	}
	return format;

}
/////////////////////////////////////////////////////////
//�Լ���: add_symbol_to_symbol_list                    //
//���: symbol�� symbol table�� ����                   //
//����: ������ 1, ���н� 0                             //
/////////////////////////////////////////////////////////
int add_symbol_to_symbol_list(char* label, int line_num, int location_counter)
{
	Symbol* ptr;
	//symbol list ��ȸ�ϸ鼭 �ߺ��Ǵ� label�� �ִ��� üũ
	for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
	{
		if (!strcmp(ptr->label, label))//�ߺ��Ǵ� label ������ ������.
		{
			return 0;//���н�
		}
	}

	//�ߺ��Ǵ� label�� ���°��
	Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
	strcpy(new_symbol->label, label);
	new_symbol->location_counter = location_counter;
	new_symbol->link = NULL;

	if (symbol_head == NULL)//symbol list�� �ƹ��͵� ������
	{
		symbol_head = new_symbol;
		symbol_tail = new_symbol;
	}
	else//symbol list�� ������� ���� ���
	{

		//���ο� �ɺ��� ���� �տ� ���� ���(��������)
		if (strcmp(new_symbol->label, symbol_head->label) < 0)
		{
			new_symbol->link = symbol_head;
			symbol_head = new_symbol;
		}
		//���ο� �ɺ��� ���� �ڿ� ���� ���(��������)
		else if (strcmp(new_symbol->label, symbol_tail->label) > 0)
		{
			symbol_tail->link = new_symbol;
			symbol_tail = new_symbol;
		}
		//���ο� �ɺ��� �߰��� ���� ���(��������)
		else
		{
			Symbol* prev = symbol_head;
			Symbol* ptr;
			for (ptr = symbol_head->link; ptr != NULL; ptr = ptr->link)
			{
				//new_symbol�� ptr�����ź��� ũ�� ptr���� ������ �� ���̿� new_symbol�� ������.
				if (strcmp(new_symbol->label, prev->label) > 0 && strcmp(new_symbol->label, ptr->label) < 0)
				{
					
					prev->link = new_symbol;
					new_symbol->link = ptr;
					break;
				}
				prev = ptr;
			}
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////
//�Լ���: pass1                                        //
//���: assemble�������� pass1�� ����                  //
//����: ������ 1, ���н� 0                             //
/////////////////////////////////////////////////////////
int pass1()
{
	FILE* asm_fp = fopen(filename, "r");
	FILE* inter_fp = fopen("intermediate", "w");
	char asm_inst[100];//asm instruction one line ����
	char label[31];
	char opcode[50];
	char operand[50];
	int line_num = 0;
	int location_counter = 0;
	int start_flag = 0;//start�� ������ 1�� �ٲ�
	int end_flag = 0;//end�� ������ 1�� �ٲ�

	//file�� ���� ���丮�� �������� �ʴ´ٸ�
	if (asm_fp == NULL)
	{
		printf("%s not exists.\n", filename);
		return 0;
	}
	//intermediate file�� ���� �� ���ٸ�
	if (inter_fp == NULL)
	{
		printf("Cannot make intermediate file.\n");
		return 0;
	}
	strcpy(program_name, "");
	do {
		//�� ������ �ʱ�ȭ
		line_num += 5;
		memset(asm_inst, '\0', sizeof(asm_inst));
		memset(label, '\0', sizeof(label));
		memset(opcode, '\0', sizeof(opcode));
		memset(operand, '\0', sizeof(operand));

		//���� end ��ȣ�� ������ �ʾҴµ� ������ ����������� ����
		if (fgets(asm_inst, 100, asm_fp) == NULL)
		{
			print_error(line_num, "asm should finish with end signature.");
			fclose(inter_fp);
			fclose(asm_fp);
			return 0;
		}
		if (!strcmp(asm_inst, "\n"))//�����̸� continue;
			continue;
		//comment�� ���
		if (asm_inst[0] == '.')
		{
			fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			continue;
		}
		//assembly instruction�� parsing����
		parse_instruction(asm_inst, label, opcode, operand);

		//���� label�� ���ڷ� �����Ѵٸ� �߸��� ����
		if (label[0] >= '0' && label[0] <= '9')
		{
			print_error(line_num, "number cannot be the first part of the label.");
			fclose(inter_fp);
			fclose(asm_fp);
			return 0;
		}
		//opcode�� start�� ��쿡�� locounter �ʱ�ȭ ����.
		if (start_flag == 0 && !strcmp(opcode, "START"))
		{
			start_flag = 1;
			strcpy(program_name, label);//program �̸� ����.
			start_address = string_to_int(operand);//start address ����.
			location_counter = start_address;
			if (start_address == -1)//hex�� �ٲܼ� ���ٸ�
			{
				print_error(line_num, "invalid start address");
				fclose(inter_fp);
				fclose(asm_fp);
				return 0;
			}

			fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);

		}
		//opcode�� END�� ���
		else if (end_flag == 0 && !strcmp(opcode, "END"))
		{
			//intermediate file�� ����.
			end_flag = 1;
			fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			break;//while�� ��������.
		}
		else
		{
			//start�� �������� ���� ���.
			if (start_flag == 0 && strcmp(opcode, "START"))
			{
				start_flag=1;
				start_address=0;
				location_counter=start_address;
			}

			if (label[0] != '\0')//label�� �ִ� ���
			{
				//LABEL�� �ִ� ��� SYMBOL TABLE�� �־���.
				if (!add_symbol_to_symbol_list(label, line_num, location_counter))
				{
					//symbol �ߺ��� ���
					print_error(line_num, "symbol duplicated");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}

			}

			//BASE�� ��� location counter ǥ�þ���
			if (!strcmp(opcode, "BASE"))
			{
				fprintf(inter_fp, "%d\t____\t%s", line_num, asm_inst);
			}
			else if (!strcmp(opcode, "RSUB"))//RSUB�� ���
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3;
			}
			else if (!strcmp(opcode, "BYTE"))//BYTE�� ���
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				if (operand[0] == 'C')
				{
					location_counter += (strlen(operand) - 3);//C''�� 3���� char ����
				}
				else if (operand[0] == 'X')
				{
					location_counter += (strlen(operand) - 3) / 2;//X''�� 3���� ���� ���� ���� �� 2���� ���ڰ� �ϳ��� byte�̹Ƿ� 2�� ��������.
				}
			}
			else if (!strcmp(opcode, "RESB"))//RESB�� ���
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += atoi(operand);
			}
			else if (!strcmp(opcode, "WORD"))//WORD�� ���
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3;
			}
			else if (!strcmp(opcode, "RESW"))//RESW�� ���
			{
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 3 * atoi(operand);
			}
			else if (opcode[0] == '+')//4������ ���
			{
				int format = find_format(opcode + 1);
				if (format == 0)//0�� ��쿡�� �ش� opcode�� �߸��� ����
				{
					print_error(line_num, "non-exist opcode.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				if (format != 3)//3,4���� �ȵǴ� ������
				{
					print_error(line_num, "cannot be a format4.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += 4;
			}
			else//1, 2, 3������ ���
			{
				int format = find_format(opcode);//������ ã�ƿ�
				if (format == 0)//0�� ��쿡�� �ش� opcode�� �߸��� ����
				{
					print_error(line_num, "non-exist opcode.");
					fclose(inter_fp);
					fclose(asm_fp);
					return 0;
				}
				fprintf(inter_fp, "%d\t%04X\t%s", line_num, location_counter, asm_inst);
				location_counter += format;
			}
		}


	} while (1);
	program_length = location_counter - start_address;//���α׷� ���� ����.
	fclose(inter_fp);
	fclose(asm_fp);
	return 1;
}


/////////////////////////////////////////////////////////
//�Լ���: find_opcode                                  //
//���: opcode table���� mnemonic�� �ش��ϴ� opcode��ȯ//
//����: ������ opcode, ���н� -1                       //
/////////////////////////////////////////////////////////
int find_opcode(char *mnemonic)
{
	int hash_key = hash_function(mnemonic);//mnemonic�� ���� hash_keyã�Ƴ�
	int flag = 0;//ã������ 1, ������ 0;
	int opcode;

	Opcode_Entry* ptr = opcode_table[hash_key];
	for (; ptr != NULL; ptr = ptr->link)//opcode_table[hash_key]�� list ��ȸ�ϸ鼭 �ش� mnemonic�ִ��� ã��
	{
		if (!strcmp(ptr->mnemonic, mnemonic))//mnemonic�� �ִٸ� flag�� 1�� �ٲپ��ְ� break;
		{
			opcode = ptr->opcode;
			flag = 1;
			break;
		}
	}
	if (flag == 1)//�ִ� mnemonic�̶��
	{
		return opcode;
	}
	else//���� mnemonic�̶��
	{
		return -1;
	}

}
/////////////////////////////////////////////////////////
//�Լ���: get_register_value                           //
//���: register�� �ش��ϴ� �� ��ȯ                    //
//����: ������ register value, ���н� -1               //
/////////////////////////////////////////////////////////
int get_register_value(char* reg)
{
	if (!strcmp(reg, "A"))
		return 0;
	if (!strcmp(reg, "X"))
		return 1;
	if (!strcmp(reg, "L"))
		return 2;
	if (!strcmp(reg, "PC"))
		return 8;
	if (!strcmp(reg, "SW"))
		return 9;
	if (!strcmp(reg, "B"))
		return 3;
	if (!strcmp(reg, "S"))
		return 4;
	if (!strcmp(reg, "T"))
		return 5;
	if (!strcmp(reg, "F"))
		return 6;
	return -1;
}

/////////////////////////////////////////////////////////
//�Լ���: add_object_code                              //
//���: object_code�� object code list�� �߰�          //
//����: ����                                           //
/////////////////////////////////////////////////////////
void add_object_code(char* object_code, int location_counter)
{
	//���ο� object_code node�� ������ְ� �����Է�
	Object_Code* new_object_code = (Object_Code*)malloc(sizeof(Object_Code));
	strcpy(new_object_code->object_code, object_code);
	new_object_code->location_counter = location_counter;
	new_object_code->link = NULL;

	if (object_code_head == NULL)//list�� ����ִ� ���
	{
		object_code_head = new_object_code;
		object_code_tail = new_object_code;
	}
	else//list�� ������� �������
	{
		object_code_tail->link = new_object_code;
		object_code_tail = new_object_code;
	}
}

/////////////////////////////////////////////////////////
//�Լ���: add_modi_code                                //
//���: modi�� �����ϴ� address�� list�� ����      //
//����: ����                                           //
/////////////////////////////////////////////////////////
void add_modi_code(int address)
{
	//���ο� modi_code node�� ������ְ� �����Է�
	Modi_Code* new_modi = (Modi_Code*)malloc(sizeof(Modi_Code));
	new_modi->address = address;
	new_modi->link = NULL;

	if (modi_head == NULL)//list�� ����ִ� ���
	{
		modi_head = new_modi;
		modi_tail = new_modi;
	}
	else//list�� ������� �������
	{
		modi_tail->link = new_modi;
		modi_tail = new_modi;
	}
}

/////////////////////////////////////////////////////////
//�Լ���: print_record                                 //
//���: ������ object file�� record������ ���         //
//����: ����                                           //
/////////////////////////////////////////////////////////
void print_record(FILE* obj_fp)
{
	if (strlen(program_name) > 6)//6�ڸ� ����ؾ��ϹǷ�
		program_name[6] = '\0';
	fprintf(obj_fp, "H%-6s%06X%06X\n", program_name, start_address, program_length);//head record ���
	Object_Code* ptr;
	Modi_Code* modi_ptr;
	int i;
	char record[61] = { '\0' };//record�� �����ϴ� �迭 (object �ڵ� ����)
	int record_index = 0;
	int record_address=0;
	memset(record, '\0', sizeof(record));

	for (ptr = object_code_head; ptr != NULL; ptr = ptr->link)//text record ���
	{
		if (record_index == 0)//record�� �������� record_address�� ����.
			record_address = ptr->location_counter;


		for (i = 0; i < strlen(ptr->object_code); i++) //record�� object �ڵ�� �־���.
		{
			record[record_index++] = (ptr->object_code)[i];
		}

		//ptr�� object�ڵ尡 record�� ������ object �ڵ��̰ų�
		//ptr�� ������ũ�� ���̰ų�
		//���� object�ڵ带 ������ �� 30byte�� �Ѿ��
		if (ptr->is_last == 1 || ptr->link == NULL || record_index + strlen(ptr->link->object_code) > 60)
		{
			record[record_index] = '\0';

			//record ������ ���ְ�
			fprintf(obj_fp, "T%06X%02X%s\n", record_address,record_index/2, record);
			memset(record, '\0', sizeof(record));//record�迭 �ʱ�ȭ
			record_index = 0;//record index�� �ʱ�ȭ ����.
		}
	}

	//modify code�� ���� ������ ���
	for (modi_ptr = modi_head; modi_ptr != NULL; modi_ptr = modi_ptr->link)
	{
		fprintf(obj_fp, "M%06X05\n", modi_ptr->address);
	}


	fprintf(obj_fp, "E%06X\n", start_address);//end record ���
}
/////////////////////////////////////////////////////////
//�Լ���: pass2                                        //
//���: assemble�������� pass2�� ����                  //
//����: ������ 1, ���н� 0                             //
/////////////////////////////////////////////////////////
int pass2()
{
	FILE* obj_fp, * lst_fp, * inter_fp;
	char obj_filename[50];
	char lst_filename[50];
	char inter_line[200];//intermediate file one line
	int line_num;
	char loc_count[40], label[31], mnemonic[50], operand[50];
	char real_operand[50], real_mnemonic[50];//���� Ư�����ڸ� ������ operand, mnemonic;
	char after_comma[50];//comma ������ ������ ���� �����
	int N, I, X, B, P, E, PC;//��������
	int format = 0;
	int BASE = 0;
	char object_code[40];
	int i;

	//obj_filename, lst filename ����
	strcpy(obj_filename, filename);
	strcpy(lst_filename, filename);
	strcpy(obj_filename + strlen(obj_filename) - 3, "obj");
	strcpy(lst_filename + strlen(obj_filename) - 3, "lst");

	obj_fp = fopen(obj_filename, "w");
	lst_fp = fopen(lst_filename, "w");
	inter_fp = fopen("intermediate", "r");

	//������ ���� ���ٸ�
	if (obj_fp == NULL || lst_fp == NULL || inter_fp == NULL)
	{
		printf("cannot open file\n");
		return 0;
	}

	PC = start_address;// pc�ʱ�ȭ

	do
	{
		//������ �ʱ�ȭ
		memset(inter_line, '\0', sizeof(inter_line));
		line_num = 0;
		memset(loc_count, '\0', sizeof(loc_count));
		memset(label, '\0', sizeof(label));
		memset(mnemonic, '\0', sizeof(mnemonic));
		memset(operand, '\0', sizeof(operand));
		memset(after_comma, '\0', sizeof(after_comma));
		memset(object_code, '\0', sizeof(object_code));

		//end�� ���������� ���α׷��� �����ٸ�
		if (fgets(inter_line, 200, inter_fp) == NULL)
		{
			return 0;
		}
		//intermediate file�� �Ѱ��� ������ parsing
		sscanf(inter_line, "%d %s %s %s %s %s", &line_num, loc_count, label, mnemonic, operand, after_comma);

		//���� \n���ڸ� \0�� �ٲپ���.
		if (strlen(inter_line) > 0)
			inter_line[strlen(inter_line) - 1] = '\0';

		int comma_flag = 0;//comma�� �ִ��� ������ check
		for (i = 0; i < strlen(inter_line); i++)
		{
			if (inter_line[i] == ',')
			{
				comma_flag = 1;
				break;
			}

		}

		//interline���� ���� label�� �����ϴ� index�� ���
		int tab_count = 0;//interline�� �� ������ ��
		int label_index = 0;//label�� ���� �� �ε��� ����.
		for (label_index = 0;label_index < strlen(inter_line); label_index++)
		{
			if (inter_line[label_index]=='\t')
			{
				tab_count++;
			}
			if (tab_count == 2)//tab 2�� ���Ŀ��� label�� ���;���.
				break;
		}


		//label�� �ִ��� ������ üũ
		//label�� ���� ��� label���� �ϳ��� �̷Ｍ ��������� ����� �����.
		if (inter_line[label_index + 1] == ' ' || inter_line[label_index + 1] == '\t')
		{
			strcpy(after_comma, operand);
			strcpy(operand, mnemonic);
			strcpy(mnemonic, label);
			strcpy(label, "");
		}


		//real mnemonic�� Ư������ �����ϰ� ����.
		if (mnemonic[0] == '+')
			strcpy(real_mnemonic, mnemonic + 1);
		else
			strcpy(real_mnemonic, mnemonic);

		//real operand�� Ư������ �����ϰ� ����.
		if (operand[0] == '#' || operand[0] == '@')
			strcpy(real_operand, operand + 1);
		else
			strcpy(real_operand, operand);


		if (!strcmp(loc_count, "____"))//location counter�� ���� ��� �ش� �κ� ' '�� �ٲپ���.
		{
			int loc_index;//location counter�� ���� ��� underbar ���ֱ� ������ �ش�κ� �ε��� �޾ƿ�
			for (loc_index = 0; inter_line[loc_index] != '\t'; loc_index++);
			for (i = loc_index + 1; inter_line[i] == '_'; i++)
				inter_line[i] = ' ';
		}		//location counter�� underbar�� ' '�� �ٲپ���.

		/// object code ���� ���
		if (label[0] == '.')//comment�� ���
		{
			//comment�� object code�� ������ ����
			//PC �ǵ��� ����
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code�� ����.
			continue;
		}
		else if (!strcmp(mnemonic, "START"))//start�� ���
		{
			//object code�� ������ ����
			//PC �ǵ��� ����
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code ����.
		}
		else if (!strcmp(mnemonic, "END"))//end �� ���
		{
			//object code�� ������ ����
			//PC �ǵ��� ����
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code ����.
			break;
		}
		else if (!strcmp(mnemonic, "BASE"))
		{
			//OBJECT �ڵ� ������ ����
			//PC �ǵ��� ����
			int symbol_exists = 0;//operand�� symbol list�� ������ 1, ������ 0;
			Symbol* ptr;
			for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
			{
				if (!strcmp(ptr->label, operand))
				{
					//symbol list�� operand�� ����
					symbol_exists = 1;
					break;
				}
			}
			if (symbol_exists == 0)
			{
				print_error(line_num, "symbol not in symbol table.");
				fclose(lst_fp);
				fclose(obj_fp);
				fclose(inter_fp);
				return 0;
			}
			BASE = ptr->location_counter;
			fprintf(lst_fp, "%-60s\n", inter_line);//lst code ����.
		}
		else if (!strcmp(mnemonic, "RESB"))
		{
			//OBJECT �ڵ� ������ ����
			PC += atoi(real_operand);
			fprintf(lst_fp, "%-60s\n", inter_line);

			//resb�� resw�� ��� text record�� ���� �ٷ� ���������. 
			//���� ���� taild�� object code�� ���� ������ record��� �� �÷��׷� ��������.
			object_code_tail->is_last = 1;
		}
		else if (!strcmp(mnemonic, "RESW"))
		{
			//OBJECT �ڵ� ������ ����
			PC += 3 * atoi(real_operand);
			fprintf(lst_fp, "%-60s\n", inter_line);

			//resb�� resw�� ��� text record�� ���� �ٷ� ���������.
			//���� ���� taild�� object code�� ���� ������ record��� �� �÷��׷� ��������.
			object_code_tail->is_last = 1;
		}
		else//object code �� �ִ� ���. �������� ������ ��������.
		{
			if (operand[0] == '#')//immediate operand
			{
				//set register
				N = 0; I = 1; B = 0;
				if ('0' <= operand[1] && operand[1] <= '9')//DIRECT ADDRESSING
					P = 0;
				else
					P = 1;
				X = 0; E = 0;
			}
			else if (operand[0] == '@')//indirect addressing
			{
				N = 1; I = 0; B = 0;
				P = 1; X = 0; E = 0;
			}
			else//Simple addressing
			{
				N = 1; I = 1; B = 0;
				P = 1; X = 0; E = 0;
			}

			if (!strcmp(mnemonic, "BYTE"))
			{
				format = 5;
				if (real_operand[0] == 'C')
				{
					PC += (strlen(real_operand) - 3);//C''�� 3���� char ����
				}
				else if (real_operand[0] == 'X')
				{
					PC += (strlen(real_operand) - 3) / 2;//X''�� 3���� ���� ���� ���� �� 2���� ���ڰ� �ϳ��� byte�̹Ƿ� 2�� ��������.
				}

			}
			else if (!strcmp(mnemonic, "WORD"))
			{
				format = 6;
				PC += 3;
			}
			else if (mnemonic[0] == '+')//4����
			{
				B = 0; P = 0; E = 1;
				format = 4;//������ intermediate ���� ���鶧 4������ �ɼ� �ִ��� �̸� üũ �� �߱� ������
				//���⼭�� ���ʿ� ����.
				PC += format;
			}
			else
			{
				format = find_format(real_mnemonic);
				//format��������. ���� mnemonic�� ���� �� ���� ������ intermediate ���� ���鶧 üũ����.
				PC += format;
			}

			//object code ���
			if (format == 1)
			{
				//format1�ε� operand�� ������ �ȵ�.
				if (real_operand[0]!='\0')
				{
					print_error(line_num, "format1 should not have a operand!");
					fclose(lst_fp);
					fclose(obj_fp);
					fclose(inter_fp);
					return 0;
				}
				int opcode=find_opcode(real_mnemonic);
				sprintf(object_code, "%02X", opcode);//16������ �ٲپ���
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
			}
			if (format == 2)
			{
				int opcode = find_opcode(real_mnemonic);
				if (comma_flag == 1)//comma�� �ִ°��
				{
					real_operand[strlen(real_operand) - 1] = '\0';
					int first_reg_val = get_register_value(real_operand);
					int second_reg_val = get_register_value(after_comma);
					if (first_reg_val == -1 || second_reg_val == -1)
					{
						print_error(line_num, "Invalid register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					sprintf(object_code, "%04X", opcode*16*16+first_reg_val*16+second_reg_val);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.			

				}
				else//comma�� ���� ���
				{
					int reg_val = get_register_value(real_operand);
					if (reg_val == -1)
					{
						print_error(line_num, "Invalid register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					sprintf(object_code, "%04X", opcode * 16 * 16 + reg_val * 16);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.		
				}

			}
			if (format == 3)
			{
				char object1[10] = { '\0' };//first part object �ڵ�� �����Ѱ� ����
				char object2[10] = { '\0' };//second part object �ڵ�� �����Ѱ� ����
				char object3[10] = { '\0' };//third part object �ڵ�� �����Ѱ� ����
				int first8;//opcode+ni
				int second4;//xbpe
				int third12 = 0;//address

				int opcode = find_opcode(real_mnemonic);
				if (real_operand[0] == '\0')//operand�� ���� ���
				{
					P = 0;//pc relative�� �ƴ�.

					first8 = opcode + 2 * N + I;//opcode+ni
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					third12=0;//address

					//������ 16������ �ٲپ���
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16������ �ٲ� �͵��� �ٿ� �־���
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
				}
				else if(N==1 && P==1)//SIMPLE ADDRESSSING�� ���
				{
					first8 = opcode + 2 * N + I;//opcode+ni

					third12 = 0;//disp

					if (comma_flag == 1 && !strcmp(after_comma, "X"))//comma�� �ְ� comma�ڿ� x�� �ִ� ���
					{
						real_operand[strlen(real_operand) - 1] = '\0';//comma ������ ��.
						X = 1;//X��Ʈ ��
					}
					else if (comma_flag == 1 && strcmp(after_comma, "X"))//comma�� �ִµ� �ڿ��� X�� �ƴѰ��
					{
						print_error(line_num, "Unvalid Register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					Symbol* ptr;
					//symbol list ��ȸ�ϸ鼭 operand�� label���� üũ
					for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
					{
						if (!strcmp(ptr->label, real_operand))
						{
							break;
						}
					}

					if (ptr != NULL)//operand�� label�̶��
					{
						third12 = ptr->location_counter - PC;
					}
					else
					{
						print_error(line_num, "cannot find operand in symtab!");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					if (third12 < -2048 || third12>2047)//pc relative�� �� �� ���ٸ�
					{
						P = 0; B = 1;//PC RELATIVE���� BASE RELATIVE�� �ٲ�.
						third12 = ptr->location_counter - BASE;
						if (third12 <0 ||third12>4095)//base relative�� �� �� ���ٸ�
						{
							print_error(line_num, "cannot be a format3");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					if (third12 < 0)//HEX 3���� ǥ��ǰ� �ϱ� ���Ͽ�
						third12 &= 0xfff;
					//������ 16������ �ٲپ���
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16������ �ٲ� �͵��� �ٿ� �־���
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.

				}
				else//Indirect, Immediate
				{
					first8 = opcode + 2 * N + I;//opcode+ni
					second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					third12 = 0;//disp
					if (P == 1)//PC RELATIVE(Indirect or Immediate)
					{
						Symbol* ptr;
						//symbol list ��ȸ�ϸ鼭 operand�� label���� üũ
						for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
						{
							if (!strcmp(ptr->label, real_operand))
							{
								break;
							}
						}

						if (ptr != NULL)//operand�� label�̶��
						{
							third12 = ptr->location_counter - PC;
						}
						else
						{
							print_error(line_num, "cannot find operand in symtab!");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}
					else//DIRECT(immediate)
					{

						int num_flag = 1;//operand�� �������� check�ϴ� flag;//1�̸� ����
						for (i = 0; i < strlen(real_operand); i++)
						{
							if (real_operand[i] < '0' || real_operand[i]>'9')
							{
								num_flag = 0;
								break;
							}

						}
						if (num_flag == 0)
						{
							print_error(line_num, "invalid operand");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}

						third12 = atoi(real_operand);

						if (third12 < 0 || third12>4095)//base relative�� �� �� ���ٸ�
						{
							print_error(line_num, "cannot be a format3");
							fclose(lst_fp);
							fclose(obj_fp);
							fclose(inter_fp);
							return 0;
						}
					}

					third12 &= 0xfff;//HEX 3���� ǥ��ǰ� �ϱ� ���Ͽ�

					//������ 16������ �ٲپ���
					sprintf(object1, "%02X", first8);
					sprintf(object2, "%01X", second4);
					sprintf(object3, "%03X", third12);

					//16������ �ٲ� �͵��� �ٿ� �־���
					strcat(object_code, object1);
					strcat(object_code, object2);
					strcat(object_code, object3);
					add_object_code(object_code, string_to_int(loc_count));
					fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
				}

			}
			if (format == 4)
			{

				int opcode = find_opcode(real_mnemonic);
				char object1[10] = { '\0' };//first part object �ڵ�� �����Ѱ� ����
				char object2[10] = { '\0' };//second part object �ڵ�� �����Ѱ� ����
				char object3[10] = { '\0' };//third part object �ڵ�� �����Ѱ� ����

				int first8 = opcode + 2 * N + I;//opcode+ni
				int second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
				int third20;//address
				if (comma_flag == 1)//comma�� �ִٸ�
				{
					real_operand[strlen(real_operand) - 1] = '\0';//comma ������ ��.
				}
				Symbol* ptr;
				//symbol list ��ȸ�ϸ鼭 operand�� label���� üũ
				for (ptr = symbol_head; ptr != NULL; ptr = ptr->link)
				{
					if (!strcmp(ptr->label, real_operand))
					{
						break;
					}
				}

				if (ptr != NULL)//operand�� label�̶��
				{
					if (comma_flag == 1 && !strcmp(after_comma, "X"))//comma�� �ְ� comma�ڿ� x�� �ִ� ���
					{
						X = 1;//X��Ʈ ��
						second4 = X * 8 + B * 4 + P * 2 + E * 1;//xbpe
					}
					else if (comma_flag == 1 && strcmp(after_comma, "X"))//comma�� �ִµ� �ڿ��� X�� �ƴѰ��
					{
						print_error(line_num, "Unvalid Register.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}
					third20 = ptr->location_counter;
				}
				else//operand�� label�� �ƴ϶�� operand�� ������.
				{
					int num_flag=1;//operand�� �������� check�ϴ� flag;//1�̸� ����
					for (i = 0; i < strlen(real_operand); i++)
					{
						if (operand[0]!='#' || real_operand[i] < '0' || real_operand[i]>'9')
						{
							num_flag = 0;
							break;
						}

					}
					if (num_flag == 0)
					{
						print_error(line_num, "invalid operand");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}

					third20 = atoi(real_operand);
				}

				//������ 16������ �ٲپ���
				sprintf(object1, "%02X", first8);
				sprintf(object2, "%01X", second4);
				sprintf(object3, "%05X", third20);

				//16������ �ٲ� �͵��� �ٿ� �־���
				strcat(object_code, object1);
				strcat(object_code, object2);
				strcat(object_code, object3);
				add_object_code(object_code, string_to_int(loc_count));//object code text record ����� ���ؼ�.
				if (operand[0] != '#')//���� immediate�� �ƴ� 4�����̶��
				{
					add_modi_code(string_to_int(loc_count) + 1);//modi���� �߰�����.
					// ���߿� object �ڵ� M����� ���ؼ�.
				}
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
			}
			if (format == 5)//byte
			{
				int integer_object = 0; // object code�� integer ����
				int hex = 1;//16�� x��������
				if (real_operand[0] == 'C')
				{
					int char_length = (strlen(real_operand) - 3);//char�� ����
					for (i = char_length+1; i >=2; i--)//��¥ char ���鼭
					{
						integer_object += real_operand[i] * hex;
						hex *= 256;
					}
					sprintf(object_code, "%X", integer_object);
				}
				else if(real_operand[0] == 'X')
				{
					int char_length = (strlen(real_operand) - 3);//������ ����
					if (char_length % 2 == 1)//Ȧ���� ��� �տ� 0�� �־���
					{
						object_code[0] = '0';
						int j = 1;
						for (i = 2; real_operand[i] != '\''; i++)//�׸��� �״�� ��������.
						{
							object_code[j++] = real_operand[i];
						}
						object_code[j] = '\0';
					}
					else//¦���� ��쿡�� �׳� �״�� �������ָ� ������Ʈ�ڵ���.
					{
						int j = 0;
						for (i = 2; real_operand[i] != '\''; i++)
						{
							object_code[j++] = real_operand[i];
						}
						object_code[j] = '\0';
					}
					if (string_to_int(object_code) == -1)//16���� ���°� �ƴ϶��
					{
						print_error(line_num, "operand is not the type of hex.");
						fclose(lst_fp);
						fclose(obj_fp);
						fclose(inter_fp);
						return 0;
					}
				}
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
			}
			if (format==6)//word
			{
				int number=atoi(real_operand);
				number &= 0xffffff;//3byte
				sprintf(object_code, "%06X", number);
				add_object_code(object_code, string_to_int(loc_count));
				fprintf(lst_fp, "%-60s\t\t%s\n", inter_line, object_code);//lst code ����.
			}

		}

	} while (1);

	print_record(obj_fp);//object file �������.

	fclose(lst_fp);
	fclose(obj_fp);
	fclose(inter_fp);
	return 1;
}
