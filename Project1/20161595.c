#include "20161595.h"

int main(void) {
	init_opcode_table();//opcode_hash_table ����
	while (1)
	{
		get_normalize_command();//command�޾Ƽ� �յ� ���� ����
		get_order_of_command();//command���� order ������
		get_normalize_last_part();//order�� ������ ������ part �յ� ���� ������ last_part�� ����

		int comma_count = 0;
		int i;
		for (i = 0; i < strlen(last_part); i++)
		{
			if (last_part[i] == ',')
				comma_count++;
		}

		//printf("%s", last_part);
		if (!strcmp(order, "h") || !strcmp(order, "help"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				help();
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "d") || !strcmp(order, "dir"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				dir();
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "q") || !strcmp(order, "quit"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				quit();//free ���ִ� �۾�
				break;
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "hi") || !strcmp(order, "history"))
		{

			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				history();
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "du") || !strcmp(order, "dump"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//������ ���� comma�� ������ �߸��� command
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//�ƹ��� ���ڵ� ������ ���� ��� [dump]
			{
				dump();
			}
			else {//���ڰ� �ִ� ���

				int count = 0;
				char* ptr = strtok(last_part, ",");//,�� �������� parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count >= 3)//���ڰ� 3���̻��� ��� �߸��� argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else if (count == 1)//���ڰ� �Ѱ��� ��� [dump start]
				{
					if (check_is_right_hexargument(argument[0]) && comma_count==0)//argument�� �´� ����̶��
					{
						start = string_to_int(argument[0]);
						dump_start();
					}
					else {//argument�� �߸��Ǿ��ٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}
				else if (count == 2)//���ڰ� �ΰ��� ��� [dump start, end]
				{

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count==1)//argument�� �´� ����̶��
					{
						start = string_to_int(argument[0]);
						end = string_to_int(argument[1]);
						dump_start_end(2);
					}
					else {//argument�� �߸��Ǿ��ٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "e") || !strcmp(order, "edit"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//������ ���� comma�� ������ �߸��� argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//�ƹ��� ���ڵ� ������ ���� ��� �߸��� argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			else {//���ڰ� �ִ� ���
				int count = 0;
				char* ptr = strtok(last_part, ",");//,�� �������� parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count != 2)//���ڰ� 2���� �ƴ� ��� �߸��� argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//���ڰ� �ΰ��� ��� edit address, value
				{

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count==1)//argument�� �´� ����̶��
					{
						address = string_to_int(argument[0]);
						value = string_to_int(argument[1]);
						edit_address_value();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "f") || !strcmp(order, "fill"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//������ ���� comma�� ������ �߸��� argument
			{

				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//�ƹ��� ���ڵ� ������ ���� ��� �߸��� argument
			{

				printf("Wrong argument!\n");
				continue;
			}
			else {//���ڰ� �ִ� ���
				int count = 0;
				char* ptr = strtok(last_part, ",");//,�� �������� parsing
				while (ptr != NULL)
				{

					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
				}

				if (count != 3)//���ڰ� 3���� �ƴ� ��� �߸��� argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//���ڰ� 3���� ��� fill start, end, value
				{


					if (check_is_right_hexargument(argument[0])
						&& check_is_right_hexargument(argument[1])
						&& check_is_right_hexargument(argument[2])
						&& comma_count==2)//argument�� �´� ����̶��
					{
						start = string_to_int(argument[0]);
						end = string_to_int(argument[1]);
						value = string_to_int(argument[2]);
						fill_start_end_value();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "reset"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				reset();
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else if (!strcmp(order, "opcode"))
		{
			if (last_part[strlen(last_part) - 1] == ',')//������ ���� comma�� ������ �߸��� argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			if (!strcmp(last_part, ""))//�ƹ��� ���ڵ� ������ ���� ��� �߸��� argument
			{
				printf("Wrong argument!\n");
				continue;
			}
			else {//���ڰ� �ִ� ���
				int count = 0;
				char* ptr = strtok(last_part, ",");//,�� �������� parsing
				while (ptr != NULL)
				{
					strcpy(argument[count], ptr);
					ptr = strtok(NULL, ",");
					count++;
					if (count > 3)
						break;
				}

				if (count != 1)//���ڰ� 1���� �ƴ� ��� �߸��� argument
				{
					printf("Wrong argument!\n");
					continue;
				}
				else//���ڰ� 1���� ��� opcode mnemonic
				{

					if (check_argument_has_noblank(argument[0]) && comma_count==0)//argument�� �´� ����̶��
					{
						strcpy(mnemonic, argument[0]);
						opcode_mnemonic();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "opcodelist"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				opcodelist();
			}
			else//last_part�� ������� ������ �ùٸ��� ���� command
			{
				printf("Wrong argument!\n");
			}
		}
		else//�׿� ��� Ŀ�ǵ���� �߸��� Ŀ�ǵ�
		{
			printf("Wrong Command!\n");
		}
	}
}


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
	char type[10];
	int hash_key;
	while (1)
	{
		res = fscanf(fp, "%x %s %s", &opcode, mnemonic, type);//���Ϸκ��� opcode, mnemonic, type �о��
		if (res == EOF)//������ ���̸� �о���°� ����
		{
			break;
		}
		hash_key = hash_function(mnemonic);//hash_key�� ����� ��.

		Opcode_Entry* opcode_entry = (Opcode_Entry*)malloc(sizeof(Opcode_Entry));
		opcode_entry->opcode = opcode;
		strcpy(opcode_entry->mnemonic, mnemonic);
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
