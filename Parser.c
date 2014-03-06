

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>


//Structure for my input
 struct command{
	char **current_cmnd_args;
	char current_operation;
	char next_operation;
	char *cmnd;
	char *infilename;
	char *outfilename;
	char *appenoutFile;
	struct command *next;
};
 struct command *head=NULL;

 //Function to get space for my input
 struct command * getspace()
 {
 	struct command * current=NULL;
 	current=(struct command *)malloc(sizeof(struct command));
 	return current;
 }

 // Function to copy String
 char* Strcpy(const char* str)
 {
 	char* newstr = NULL;
 	int size = strlen(str);
 	newstr =(char *)malloc(size+1);
 	strcpy(newstr,str);
 	newstr[size] = '\0';
 	return newstr;
 }

//Funtion to check for syntax error
int checkSyntax(char *string)
{
	char *pt=string;
	int syntaxCheck=0;
	if(strstr(string,"&"))
	{
		pt=strstr(string,"&");
		while(*pt!='\0')
		{
			pt=pt+1;
			if(*pt==';' || *pt=='&' || *pt=='|')
			{
				syntaxCheck=1;
				break;
			}
		}
	}
return syntaxCheck;
}

/**
 * Function to run my commands. Checks input and output operations
 * and accordingly handles pipes, opens files and handles standard input and output
 */
void runCommand(int bckFlag)
{
	int pid=0;
	int in=0,out=0,status=0;
	int pipes=0;
	int pipeindex=0;
	int *pipefd=(int *)malloc(sizeof(int)*40);
	for(pipes=0;pipes<20;pipes++)
	{
		pipe(&pipefd[pipes*2]);
	}
	pipeindex=0;
	while(head!=NULL)
	{
		if((pid=fork())==-1)
		{
			perror("Fork Error");
		}
		if(pid==0)
		{
			if(head->current_operation=='\0' && head->next_operation=='<')
			{
				in=open(head->infilename,O_RDONLY);
				if(dup2(in,0)==-1)
					perror("Error while doing dup2 ");
				if(head->outfilename!=NULL)
				{
					out=open(head->outfilename,O_WRONLY|O_CREAT|O_TRUNC,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				else if(head->appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");

				}
			}
			if(head->current_operation=='|' && head->next_operation=='<')
			{
				in=open(head->infilename,O_RDONLY);
				if(dup2(in,0)==-1)
					perror("Error while doing dup2 ");
				if(head->outfilename!=NULL)
				{
					out=open(head->outfilename,O_CREAT|O_WRONLY,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				else if(head->appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
			}
			if(head->current_operation=='\0' && head->next_operation=='>')
			{
				if(head->outfilename!=NULL)
				{
					out=open(head->outfilename,O_CREAT|O_WRONLY,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				else if(head->appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
			}

			if(head->current_operation=='|' && head->next_operation=='>')
			{
				dup2(pipefd[pipeindex],0);
				if(head->outfilename!=NULL)
				{
					out=open(head->outfilename,O_CREAT|O_WRONLY,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				else if(head->appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
			}

			if(head->current_operation=='\0' && head->next_operation=='|')
			{
				if(head->infilename!=NULL)
				{
					in=open(head->infilename,O_RDONLY);
					if(dup2(in,0)==-1)
						perror("Error while doing dup2 ");
				}
				if(head -> outfilename!=NULL)
				{
					out=open(head->outfilename,O_CREAT|O_WRONLY,0644);
					dup2(out,1);

				}
				if(head -> appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				if(head->infilename!=NULL && head->outfilename!=NULL)
				{
				}
				else if(head->outfilename!=NULL || head->appenoutFile!=NULL )
				{
				}
				else
				{
					if(dup2(pipefd[pipeindex+1],1)==-1)
						perror("Error while doing dup2 ");

				}
			}
			else if(head->current_operation=='|' && head->next_operation=='|')
			{
				if(head->infilename!=NULL)
				{
					in=open(head->infilename,O_RDONLY,0644);
					if(dup2(in,0)==-1)
						perror("Error while doing dup2 ");
				}
				if(head -> outfilename!=NULL)
				{
					out=open(head->outfilename,O_CREAT|O_WRONLY,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");

				}
				if(head -> appenoutFile!=NULL)
				{
					out=open(head->appenoutFile,O_CREAT|O_WRONLY|O_APPEND,0644);
					if(dup2(out,1)==-1)
						perror("Error while doing dup2 ");
				}
				if((head->infilename!=NULL) && (head->outfilename!=NULL))
				{

				}
				else if(head->outfilename!=NULL || head->appenoutFile!=NULL )
				{
				}
				else if(head->infilename!=NULL)
				{	pipeindex=pipeindex+2;
					if(dup2(pipefd[pipeindex+1],1)==-1)
						perror("Error while doing dup2 ");
				}
				else
				{
					dup2(pipefd[pipeindex],0);
					pipeindex+=2;
					dup2(pipefd[pipeindex+1],1);
				}
			}
			else if(head->current_operation=='|' && head->next_operation=='\0')
			{

				if(dup2(pipefd[pipeindex],0)==-1)
					perror("Error while doing dup2 ");

			}

			if(execvp(head->cmnd,head->current_cmnd_args)==-1)
			{
				perror("Error while executing execvp function");
				exit(0);
			}
		}
		else
		{
			if(bckFlag==1)
			{
				wait4(pid,&status,WNOHANG,NULL);
			}
			else
			{
				waitpid(pid,&status,0);
			}
			if(head->current_operation=='\0' && head->next_operation=='|')
			{
				if(head->outfilename!=NULL || head->appenoutFile!=NULL)
				{
				}
				else
				{
					close(pipefd[pipeindex+1]);
				}
			}
			if(head->current_operation=='|' && head->next_operation=='|')
			{
				if(head->infilename!=NULL)
				{ close(pipefd[pipeindex]);
					pipeindex=pipeindex+2;
					close(pipefd[pipeindex+1]);
				}
				else
				{
					close(pipefd[pipeindex]);
					pipeindex+=2;
					close(pipefd[pipeindex+1]);
				}
			}
			if(head->current_operation=='|' && head->next_operation=='\0')
			{
				close(pipefd[pipeindex]);
			}
			if(head->current_operation=='|' && head->next_operation=='>')
			{
				close(pipefd[pipeindex]);
			}
		}
		head=head->next;
	}
	for(pipes=0;pipes<20;pipes++)
	{
		close(pipefd[pipes*2]);
		close(pipefd[(pipes*2)+1]);
	}
}


//Function to initialise my command Structure
char * initialiseStructure(struct command *temp)
{
	temp->current_cmnd_args=NULL;
	temp->current_operation='\0';
	temp->next_operation='\0';
	temp->cmnd=NULL;
	temp->infilename=NULL;
	temp->outfilename=NULL;
	temp->appenoutFile=NULL;
	temp->next=NULL;
	return temp;

}

//Function to truncate end spaces
char * truncateEndSpaces(char * string)
{
	char *pt=NULL;
	int length=strlen(string);
	int count=0;
	pt=&string[length-1];
	if(*pt==' ')
	{
		while(*pt==' ')
		{
			pt--;
			count++;
		}

	}
string[length-count]='\0';
return string;


}

//Function to add spaces. Adds a space before and after '|','>>','>','<' to seperate the strings by space
char * addSpaces(char *myString)
{
	char *pt=NULL,*pt1=NULL,*changedString=NULL;
	int currentSize = strlen(myString)+1;
	changedString = (char*)malloc(200*sizeof(char));
	memset(changedString, '\0', currentSize);
	pt=myString;
	pt1=changedString;
	while(*pt!='\0')
	       {
	    	   if(*pt=='|')
	    	   {
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;
	    		   *pt1++=*pt++;
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;

	    	   }
	    	   if(*pt=='>')
	    	   {
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;
	    		   *pt1++=*pt++;
	    		   if(*pt=='>')
	    		   {
	    			   *pt1++=*pt++;
	    		   }
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;
	    	   }
	    	   if(*pt=='<')
	    	   {
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;
	    		   *pt1++=*pt++;
	    		   currentSize=currentSize+1;
	    		   changedString[currentSize] = '\0';
	    		   *pt1=' ';
	    		   pt1=pt1+1;
	    	   }
	    	   *pt1++=*pt++;

	       }
	       *pt1='\0';
	       return changedString;
}


//Function to remove extra spaces from the input
char *removeExtraSpaces(char *myString, char *copy)
{
	char *pt=NULL,*pt1=NULL;
	//int currentSize=strlen(myString);
	pt=myString;
	pt1=copy;
	while(*pt!='\0')
	{
		if(*pt==' ')
		{
			*pt1++=*pt++;

			if(*pt==' ')
			{
				while(*pt==' ')
				{
					pt=pt+1;
				}
			}
		}
		*pt1++=*pt++;
	}
	*pt1='\0';
	return copy;
}

//Function to truncate beginning spaces in the string
char * truncateBeginningSpaces(char * string)
{
		char *pt=NULL,*pt1=NULL,*copy=NULL;
		int currentSize = strlen(string)+1;
		copy = (char*)malloc(currentSize);
		memset(copy,'\0',currentSize);
		pt=string;
		pt1=copy;
		if(*pt==' ')
		{
			pt++;
		}
		while(*pt!='\0')
		{
			*pt1++=*pt++;
		}
		*pt1='\0';
		if(copy[strlen(copy)-1]==' ')
		{
			copy[strlen(copy)-1]='\0';
		}
		return copy;
}


/**
 * Function to parse input string.
 * Removes beginning spaces adds spaces
 * by call to addSpaces and then seperates the string by space
 */
char** parseString(char *const parse,int *numberArg)
{
	char *token = NULL,*newString=NULL;
	int size = strlen(parse) + 1;
	char *string = (char*)malloc(size);
	memset(string, '\0', size);
	memcpy(string, parse, size);
	char *copy=NULL,*copy1,*copy2;
	int no=0;
	copy=addSpaces(string);
	copy1=(char *)malloc(strlen(copy)+1);
	memset(copy1,'\0',strlen(copy)+1);
	copy1=strdup(copy);
	memset(copy,'\0',strlen(copy));
	copy=removeExtraSpaces(copy1,copy);
	copy2=truncateBeginningSpaces(copy);
	newString=(char *)malloc(sizeof(char)*(strlen(copy2)+1));
	memset(newString,'\0',strlen(copy2));
	newString = strdup(copy2);
	char **temp=(char **)malloc(sizeof(char*)*256);
	memset(temp,'\0',sizeof(char*)*256);
	while((token =strsep(&newString," "))!=NULL)
	{
		temp[no]=token;
		no++;

	}
	*numberArg=no;
	return temp;

}

// Functio to traverse the command structure to the end
struct command *putAtEnd()
{
	struct command *temp;
	temp=head;
	while(temp->next!=NULL)
	{
		temp=temp->next;
	}
	return temp;

}

//Function for setting the command structure according to the input string
void setStructure(char **temp,int noOfArgumentsInTemp)
{
	int tempArrayIndex=0,argumentsInTemp,j=0,noOfCommands=0,commandArgument=0,flagCheckCommand=0,flagCheckRedirection=0;
	char *commandName=NULL;
	argumentsInTemp=noOfArgumentsInTemp;
	struct command *c=NULL,*end=NULL,*commandNext=NULL;
	head=NULL;
	while(noOfArgumentsInTemp!=0)
	{
		if(tempArrayIndex<argumentsInTemp && strcmp(temp[tempArrayIndex],"<")!=0 && strcmp(temp[tempArrayIndex],">")!=0 && strcmp(temp[tempArrayIndex],">>")!=0 && strcmp(temp[tempArrayIndex],"|")!=0)
		{
			if(j==0)
			{
				c=getspace();
				c=initialiseStructure(c);
				head=c;
				c->current_cmnd_args=(char **)malloc(sizeof(char *)*5);
				memset(c->current_cmnd_args,0,sizeof(char *)*5);
				commandName=(char*)malloc( (strlen(temp[tempArrayIndex])+1)  *  sizeof(char));
				strcpy(commandName,temp[tempArrayIndex]);
				c->cmnd=commandName;
				c->current_operation='\0';
				c->next=NULL;
				c->current_cmnd_args[commandArgument]=c->cmnd;
				tempArrayIndex++;
				noOfArgumentsInTemp--;
				j++;
				noOfCommands++;
				commandArgument++;
				c->current_cmnd_args[commandArgument]=NULL;
			}
			else
			{
				commandName=(char*)malloc( (strlen(temp[tempArrayIndex])+1)  *  sizeof(char));
				strcpy(commandName,temp[tempArrayIndex]);
				c->current_cmnd_args[commandArgument]=commandName;
				tempArrayIndex++;
				noOfArgumentsInTemp--;
				commandArgument++;
				c->current_cmnd_args[commandArgument]=NULL;

			}
		}
		if(tempArrayIndex<argumentsInTemp && strcmp(temp [tempArrayIndex],"<")==0 )
		{
			flagCheckCommand=1;
			c->next_operation='<';
			flagCheckRedirection=1;
			tempArrayIndex++;
			commandName=(char*)malloc(strlen(temp[tempArrayIndex])+1);
			memset(commandName, '\0', strlen(temp[tempArrayIndex])+1);
			strcpy(commandName,temp[tempArrayIndex]);
			c->infilename=commandName;
			tempArrayIndex++;
			noOfArgumentsInTemp=noOfArgumentsInTemp-2;
		}
		if(tempArrayIndex<argumentsInTemp && strcmp(temp[tempArrayIndex],">")==0 )
		{
			flagCheckCommand=1;
				if(flagCheckRedirection==0)
				{
					c->next_operation='>';
				}

					tempArrayIndex++;
					commandName=(char*)malloc(strlen(temp[tempArrayIndex])+1);
					memset(commandName, '\0', strlen(temp[tempArrayIndex])+1);
					strcpy(commandName,temp[tempArrayIndex]);
					c->outfilename=commandName;
					tempArrayIndex++;
					noOfArgumentsInTemp=noOfArgumentsInTemp-2;
		}
		if(tempArrayIndex<argumentsInTemp && strcmp(temp[tempArrayIndex],">>")==0 )
		{
					flagCheckCommand=1;
						if(flagCheckRedirection==0)
						{
							c->next_operation='>';
						}
							tempArrayIndex++;
							commandName=(char*)malloc(strlen(temp[tempArrayIndex])+1);
							memset(commandName, '\0', strlen(temp[tempArrayIndex])+1);
							strcpy(commandName,temp[tempArrayIndex]);
							c->appenoutFile=commandName;
							tempArrayIndex++;
							noOfArgumentsInTemp=noOfArgumentsInTemp-2;
		}
		if(tempArrayIndex<argumentsInTemp && strcmp(temp[tempArrayIndex],"|")==0)
		{
							flagCheckCommand=1;
							commandArgument=0;
							c->next_operation='|';
							commandNext=getspace();
							commandNext=initialiseStructure(commandNext);
							end=putAtEnd();
							end->next=commandNext;
							c=commandNext;
							c->next=NULL;
							c->current_operation='|';
							c->current_cmnd_args=(char **)malloc(sizeof(char *)*5);
							tempArrayIndex++;
							commandName=(char*)malloc(strlen(temp[tempArrayIndex])+1);
							memset(commandName, '\0', strlen(temp[tempArrayIndex])+1);
							strcpy(commandName,temp[tempArrayIndex]);
							c->cmnd=commandName;
							c->current_cmnd_args[commandArgument]=c->cmnd;
							tempArrayIndex++;
							noOfArgumentsInTemp=noOfArgumentsInTemp-2;
							commandArgument++;
							noOfCommands++;
							c->current_cmnd_args[commandArgument]=NULL;
		}
	}
	if(flagCheckCommand==0)
	{
		c->next_operation='\0';
	}
}


// My main function
int main(int argc, char *argv[])
{
	int syntaxCheck=0,status=0;
	char *inputString=NULL;
	size_t len = 512;
	char **arg_temp=NULL;
	char **arg=NULL;

	char *path=NULL, *pathname=NULL,*cdpath=NULL;
	int size,i=0,arg_count=0;

	char *strparse=NULL;
	char **temp_arg=NULL;
	int numberArg=0;
	char *strwithBackground=NULL;
	char *bck=NULL,*strptr=NULL;
	int bckFlag=0;
	inputString = (char *) malloc (512);
	memset(inputString,'\0',len);

	while(1)
	{
		bckFlag=0;
		if(argc ==2)
		{
			printf("%s:",argv[1]);
		}
		else if(argc==1)
			printf("myshell:");
		fflush(stdout);

		getline(&inputString, &len, stdin);
		if(feof(stdin) != 0)
		{
			exit(0);
		}
		size=strlen(inputString);
		inputString[size-1]='\0';
		arg = (char **)malloc(10 * sizeof(char *));
		memset(arg,0,10 * sizeof(char *));
		syntaxCheck=checkSyntax(inputString);
		if(syntaxCheck==1)
		{
			printf("Syntax Error\n");
			continue;
		}
		arg_temp=&inputString;
		arg[i] = strsep(arg_temp, ";");
		while(arg[i] != NULL)
		{
			bckFlag=0;
			strparse = Strcpy(arg[i]);
			strparse=truncateEndSpaces(strparse);

			if(strcmp(strparse, "exit") == 0)
				exit(0);
			if(strstr(strparse, "cd"))
			{
				pathname=strstr(strparse, "cd");
				while(*pathname!=' ')
				{
					pathname=pathname+1;

				}
				if(*pathname==' ')
				{
					while(*pathname==' ')
					{
						pathname=pathname+1;
					}
				}
				cdpath=(char *)malloc(strlen(strparse));
				memset(cdpath,'\0',strlen(strparse));
				path=cdpath;
				while(*pathname!='\0')
				{
					*path++=*pathname++;
				}
				*path='\0';
				if(chdir(cdpath)==-1)
				{
					perror("Change Directory Failed");
				}
				i++;
				arg[i] = strsep(arg_temp, ";");
				continue;
			}

			strptr=strparse;
			strwithBackground=(char *)malloc(sizeof(char)*200);
			memset(strwithBackground,'\0',sizeof(char)*200);
			bck=strwithBackground;
			if(strstr(strparse,"&"))
			{
				bckFlag=1;
				while(*strptr!='&')
				{
					*bck++=*strptr++;
				}
				*bck='\0';
				strwithBackground=truncateEndSpaces(strwithBackground);
				memset(strparse,'\0',strlen(strwithBackground)+1);
				strparse = Strcpy(strwithBackground);
			}
			arg_count++;
			temp_arg=parseString(strparse,&numberArg);
			setStructure(temp_arg,numberArg);
			runCommand(bckFlag);
			i++;
			arg[i] = strsep(arg_temp, ";");
		}
		wait4(-1,&status,WNOHANG,NULL);
	}

	return 0;
}

