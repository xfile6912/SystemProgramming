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
					if (check_is_right_hexargument(argument[0]) && comma_count == 0)//argument�� �´� ����̶��
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

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count == 1)//argument�� �´� ����̶��
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

					if (check_is_right_hexargument(argument[0]) && check_is_right_hexargument(argument[1]) && comma_count == 1)//argument�� �´� ����̶��
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
						&& comma_count == 2)//argument�� �´� ����̶��
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

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument�� �´� ����̶��
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
		else if (!strcmp(order, "type"))
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
				else//���ڰ� 1���� ��� type filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument�� �´� ����̶��
					{
						strcpy(filename, argument[0]);
						type();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "type"))
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
				else//���ڰ� 1���� ��� type filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument�� �´� ����̶��
					{
						strcpy(filename, argument[0]);
						type();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "assemble"))
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
				else//���ڰ� 1���� ��� assemble filename
				{

					if (check_argument_has_noblank(argument[0]) && comma_count == 0)//argument�� �´� ����̶��
					{
						strcpy(filename, argument[0]);
						assemble();
					}
					else {//�߸��� argument�� �ִٸ�
						printf("Wrong argument!\n");
						continue;
					}
				}

			}
		}
		else if (!strcmp(order, "symbol"))
		{
			if (!strcmp(last_part, ""))//last_part�� ��������� �ùٸ� command
			{
				symbol();
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

