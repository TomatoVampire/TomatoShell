#include<unistd.h>
#include<stdio.h>
#include<string.h>
#define BUFFERSIZE 1024
#define MAXPARA 10

void test_shell();

enum CommandType
{
	HELP,
	EXIT,
	TEST,
	RUN,
	LS,
	CP,
	PIPE,
	NONE
};

void print_split_command(char* set[])
{
	int i=0;
	while(set[i])
	{
		printf("%s\n",set[i]);
		i++;
	}
}

//根据字符串分析命令
enum CommandType get_command(char* set[])
{
	//printf()
	//PIPE
	int i=0;
	while(set[i])
	{
		if(strcmp(set[i], "|") == 0)
		{
		//printf("type:pipe\n");
		return PIPE;}
		i++;
	}

	if(strcmp(set[0], "help") == 0)
	{
		return HELP;
	}
	else if(strcmp(set[0], "exit") == 0)
	{

		return EXIT;
	}
	else if(strcmp(set[0], "test") == 0)
	{
		return TEST;
	}
	else if(strcmp(set[0], "run") == 0)
	{
		return RUN;
	}
	else if(strcmp(set[0], "ls") == 0)
	{
		return LS;
	}
	else if(strcmp(set[0], "cp") == 0)
	{
		return CP;
	}
	else return NONE;
}

int empty_check(char* s)
{
	//printf("empty check\n");
	if(s[0] == '\0') return 1;
	//bool flag = false;
	for(int i=0;s[i] != '\0';i++)
	{
		if(s[i] != ' ') 
		{
		//printf("not empty\n");
		return 0;
		}
	}
	return 1;
}

//将输入的命令以空格分隔，保存至set。参数后加入一个NULL!!!
void div_command(char* str, char* cmdset[])
{
	int pos = 0;
	char temp[BUFFERSIZE];

	cmdset[0] = strtok(str, " ");
	if(empty_check(str))
	{
		cmdset[0] = "";
		cmdset[1] = NULL;
		return;
	}
	while(cmdset[pos]!=NULL)
	{
		pos++;
		cmdset[pos] = strtok(NULL, " ");
	}
	//空输入检查
	cmdset[pos] = NULL;
	//for(int i=0;i<pos;i++)
	//{printf("%s\n", cmdset[i]);}
}

//为了管道而将|前后的命令分开
void pipe_split_command(char* oset[], char* xset[], char* yset[])
{
	int x=0, y=0;
	int o=0;
	//xset
	//print_split_command(oset);
	while(oset[o])
	{
		//printf("%s\n",oset[o]);
		if(strcmp(oset[o], "|") == 0)
		{
		//printf("met |\n");
		break;
		}
		//strcpy(oset[o], xset[x]);
		xset[x] = oset[o];
		//printf("%s\n",xset[x]);
		x++;
		o++;
	}
	//printf("----------");
	//最后补NULL
	xset[x] = NULL;
	//取|后一个参数
	o++;
	while(oset[o])
	{
		//懒得复制，直接引用
		yset[y] = oset[o];
		//printf("%s\n",yset[y]);
		y++;
		o++;
	}
	yset[y] = NULL;
	//print_split_command(xset);
	//print_split_command(yset);
}


//管道
void pipe_shell(char* set[])
{
	char *xset[MAXPARA];
	char *yset[MAXPARA];
	//char *sset[] = {"ls","|","grep","hello.c",NULL};
	pipe_split_command(set, xset, yset);
	//print_split_command(xset);
	//print_split_command(yset);
	int pp;
	pp =  fork();
	if(pp == 0)
	{
	int fd[2];
	pipe(fd);
	int p;
	p = fork();

		if(p == 0)
		{
		//写
		dup2(fd[1],1);
		//printf("%s\n",xset[0]);
		execvp(xset[0],xset);
		}
		else
		{
		//读
		dup2(fd[0],0);
		close(fd[1]);
		//printf("%s\n",yset[0]);
		execvp(yset[0],yset);
		}
	}
	else
	{
	wait();
	}
}

//帮助信息
void help()
{
	printf("Welcome to MyShell2018172020!\n");
	printf("Type help to see help info.\n");
	printf("Type run + programname + parameters to run a custom program.\n");
	//printf("Welcome to MyShell2018172020!\n");
}

//退出程序
void exit_shell()
{
	printf("exiting program...\n");
	exit(1);
}



//运行自定义程序
void run_program(char* set[])
{
	//printf("test run...\n");
	char temp[BUFFERSIZE] = "./";
	int i=1;
	char sp[] = " ";
	while(set[i])
	{
		//printf("%s\n",temp);
		strcat(temp, set[i]);
		strcat(temp, sp);
		i++;
	}
	//printf("%s\n",temp);
	system(temp);
	//由于在子进程里，需要手动exit
	exit(1);
}

int invalid_command(char* set[])
{
	enum CommandType c;
	c = get_command(set);
	switch(c)
	{
		case NONE:
		printf("No such command!\n");
		return 1;
		break;
		default:return 0;
		break;
	}
	
}

//内建命令
void built_in_command(char* set[])
{
	enum CommandType c;
	c = get_command(set);
	switch(c)
	{
		case HELP:help();break;
		case EXIT:exit_shell();break;
		case TEST:test_shell();break;
		default:break;
	}
}



//外部命令
void out_command(char* set[])
{
	enum CommandType c = get_command(set);
	int p = -2;
	switch(c)
	{
		case RUN://printf("run!\n");
		case CP:
		case LS:p = fork();break;
		case PIPE:pipe_shell(set);break;
		default:break;
	}
	if(p == -2) return;
	//出错
	else if(p == -1)
	{
		printf("fork error!\n");
		exit(1);
	}
	//子进程
	else if(p == 0)
	{
		switch(c)
		{
			case LS:
			execvp("ls", set);
			break;
			case CP:
			execvp("cp", set);
			break;
			case RUN:
			run_program(set);
			break;
			default:break;		
		}
	}
	//父进程：
	else
	{
		wait();
		printf("child end.\n");
	}
}

//test
void test_shell()
{
	//printf("this is test program.\n");
	char *set[MAXPARA] = {"ls","|","grep","hello.c",NULL};
	pipe_shell(set);
}
int main()
{
	char buffer[BUFFERSIZE];
	memset(buffer, '\0', BUFFERSIZE);
	while(1)
	{
		//输出提示符
		printf("$ ");
		char set[MAXPARA+1][BUFFERSIZE];
		gets(buffer);
		//分割好的命令
		//printf("%s\n", buffer);
		div_command(buffer, set);
		
		//无效命令
		if(invalid_command(set)) continue;
		//内建命令
		built_in_command(set);
		out_command(set);
		//外部命令
	}
}
