#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include<limits.h>
#include <time.h>

#define BUFSIZE 8096

struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"png", "image/png" },
	{"bmp", "image/x-ms-bmp" },
	{"doc", "application/msword" },
    {"pdf", "application/pdf" },
	{"js", "text/javascript" },
	{"mp3", "audio/mpeg" },
	{"mp4", "video/mp4" },
	{"flv", "video/x-flv" },
	{"swf", "application/x-shockwave-flash" },
	{"swfl", "application/x-shockwave-flash" },
    {"htm", "text/html" },
    {"html","text/html" },
	{"txt","text/plain" },
	{"css","text/css" },
    {"exe","text/plain" },
    {0,0} };



void handle_socket(int fd)
{

	int j, file_fd, buflen, elen;
	long i, ret;
	char * fstr;
	static char buffer[BUFSIZE+1];
	time_t tm;
	struct tm *tms;
	char *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul","Aug","Sep","Oct","Nov","Dec"};
	struct stat st;
    FILE *fptr;
    DIR *dptr;
    int fNLen,flag = 0,bFlag = -1;
    char fNameBuf[1000] = {'\0'};
    char fNameBufAppend[1000] = {'\0'};

	time(&tm);
	tms = gmtime(&tm);

	fNLen = strlen(fNameBuf);
	ret = read(fd,buffer,BUFSIZE);  
    if (ret==0||ret==-1) {
       	exit(3);
    }

        
	if (ret>0&&ret<BUFSIZE)
		buffer[ret] = 0;
	else
		buffer[0] = 0;
		
	 for(i=4;i<BUFSIZE;i++) {
        if(buffer[i] == ' ') {
            buffer[i] = 0;
            break;
        }
        }
	strcpy(fNameBuf,&buffer[5]);
		
		
	if(fNameBuf[0]=='\0')//root
	{
		fptr = fopen("index.html","r");
				 
		if(!fptr)
		{
			sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found root %s</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fNameBuf);
			write(fd,buffer,strlen(buffer));
			exit(1);
		}
			
		sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
		write(fd,buffer,strlen(buffer));
		while ((ret=fread(&buffer, 1,BUFSIZE ,fptr))>0) {
		write(fd,buffer,ret);
		}
		fclose(fptr);
		exit(1);
			
	}
	
	else if(fNameBuf[fNLen-1]=='/')
	{
		strncpy(fNameBufAppend,fNameBuf,1000);
		strcat(fNameBufAppend,"index.html");
		fptr = fopen(fNameBufAppend,"r");
		
		if(!fptr)
		{
			sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found root</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
			write(fd,buffer,strlen(buffer));
			exit(1);
		}
			
		sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
		write(fd,buffer,strlen(buffer));
		while ((ret=fread(&buffer, 1,BUFSIZE ,fptr))>0) {
		write(fd,buffer,ret);
		}
		fclose(fptr);
		exit(1);
	}
	/*else
	{
		sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n%s\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fNameBuf);
		write(fd,buffer,strlen(buffer));
		exit(1);
	}*/
	else
	{
	//  printf("Filename : %s \n",fname);
		if(!stat(fNameBuf, &st))
		{
			 
			if (S_ISDIR(st.st_mode))
			{
				//printf("This is a directory.\n");
				sprintf(buffer,"HTTP/1.0 301 Moved Permanently\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nLocation: %s/\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fNameBuf);
				//sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found dir</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
					write(fd,buffer,strlen(buffer));
				exit(1);
			}
		
		
			else if(S_ISREG(st.st_mode)) //this name is a file
			{
				for(h=0;extensions[j].ext != 0;j++)
				{
					 
					elen = strlen(extensions[j].ext);
					if(!strncmp(&fNameBuf[buflen-elen], extensions[j].ext, elen)) {
						fstr = extensions[j].filetype;
						break;
					}
				}

				if(fstr == 0) //not supported
					fstr = extensions[j-1].filetype; //assign to text/plain
				
				
				}
				/*//printf("a file\n");
				fptr = fopen(fNameBuf,"r");
				 
				if(!fptr)
				{
					perror(fNameBuf);
					sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found file</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
					write(fd,buffer,strlen(buffer));
					exit(1);
				}
			
				sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n%s\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
				write(fd,buffer,strlen(buffer));
				fclose(fptr);
				exit(1);*/
				
				fptr = fopen(fNameBuf,"r");
				
				if(!fptr)
				{
					sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found root</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec);
					write(fd,buffer,strlen(buffer));
					exit(1);
				}
					
				sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: %s\r\n\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fstr);
				write(fd,buffer,strlen(buffer));
				while ((ret=fread(&buffer, 1,BUFSIZE ,fptr))>0) {
				write(fd,buffer,ret);
				}
				fclose(fptr);
				exit(1);
				
				/*sprintf(buffer,"HTTP/1.0 200 OK\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n%s\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fNameBuf);
				write(fd,buffer,strlen(buffer));
				exit(1);*/
			 }
		}
		else
		{
			sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>404 Not Found : Can't get file %s</b>\r\n" ,week[tms->tm_wday],tms->tm_mday,month[tms->tm_mon],(1900+tms->tm_year),tms->tm_hour,tms->tm_min,tms->tm_sec,fNameBuf);
			write(fd,buffer,strlen(buffer));
			exit(1);
		}
		 
	}
	/*sprintf(buffer,"HTTP/1.0 404 Not Found\r\nDate: %s, %d %s %d %d:%d:%d GMT\r\nServer: Apache/2.2.12 (Ubuntu)\r\nConnection: Close\r\nContent-Type: text/html\r\n\r\n<b>nothing final</b>\r\n" );
	write(fd,buffer,strlen(buffer));*/
	
	
}
int main(int argc, char **argv)

{

    int i, listenfd, socketfd;
    size_t length;
    static struct sockaddr_in cli_addr;
    static struct sockaddr_in serv_addr;
	pid_t pid, sid;

    /* 背景繼續執行 */
    if(fork() != 0)
        return 0;

    /* 讓父行程不必等待子行程結束 */
    signal(SIGCLD, SIG_IGN);

	sid = setsid();
    if (sid < 0) {
            /* Log any failure */
            exit(EXIT_FAILURE);
        }

	 printf("SID:%d\n",sid);
	/* 使用 /tmp 當網站根目錄 */
    if(chdir("/home/hao/htdoc") == -1){ 
        printf("ERROR: Can't Change to directory tmp\n");
        exit(4);
    }

	umask(0);

	close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* 開啟網路 Socket */
    if ((listenfd=socket(AF_INET, SOCK_STREAM,0))<0)
        exit(3);

    /* 網路連線設定 */
    serv_addr.sin_family = AF_INET;
    /* 使用任何在本機的對外 IP */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* 使用 80 Port */
    serv_addr.sin_port = htons(8080);

    /* 開啟網路監聽器 */
    if (bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
        exit(3);

    /* 開始監聽網路 */
    if (listen(listenfd,64)<0)
        exit(3);

    while(1) {
        length = sizeof(cli_addr);
        /* 等待客戶端連線 */
        if ((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length))<0)
            exit(3);

        /* 分出子行程處理要求 */
        if ((pid = fork()) < 0) {
            exit(3);
        } else {
            if (pid == 0) {  /* 子行程 */
                close(listenfd);
                handle_socket(socketfd);
            } else { /* 父行程 */
                close(socketfd);
            }
        }
    }
}
