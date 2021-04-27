#include<unistd.h>
#include<stdio.h>
#include<string.h>
#define BUFFERSIZE 1024

void print_split_command(char* set[])
{
	int i=0;
	while(set[i])
	{
		printf("%s\n",set[i]);
		i++;
	}
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
}

//TODO
void pipe_shell()
{
	//char* xset[] = {"ls",NULL};
	//char* yset[] = {"grep","hello.c",NULL};
	char* set[] = {"ls", "|","grep","hello.c",NULL};
	char *xset[5];
	char *yset[5];
	xset[0] = "hello!";
	//strcpy("hello", xset[0]);
	//printf("%s\n",xset[0]);
	pipe_split_command(set,xset,yset);
	printf("%s\n",xset[0]);
	printf("%s\n",yset[0]);
	//print_split_command(xset);
	//print_split_command(yset);
	
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

int main()
{

	pipe_shell();
	return 0;
}
