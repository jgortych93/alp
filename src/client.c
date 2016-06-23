/*
    ** client.c -- klient używający gniazd strumieniowych
    */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>




#define MAXDATASIZE 100 // maksymalna ilość dancyh, jaką możemy otrzymać na raz
int sockfd;

int write_string(int new_fd,const char *string);
int read_string(int fd);
void showMenu();
char* choose_if();
char* setMac();

int main(int argc, char *argv[])
{
    
    int PORT=5557;
    struct hostent *he;
    struct sockaddr_in their_addr; // informacja o adresie osoby łączącej się
	
	char *string,*bufor;

    if (argc==1){
        if ((he=gethostbyname("127.0.0.1")) == NULL) {  // pobierz informacje o hoście
            perror("gethostbyname");
            exit(1);
        }
    }

    if (argc > 1) {
        if ((he=gethostbyname(argv[1])) == NULL) {  // pobierz informacje o hoście
            perror("gethostbyname");
            exit(1);
        }
    }

    if (argc>2){
        PORT=atoi(argv[2]);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;    // host byte order
    their_addr.sin_port = htons(PORT);  // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);  // wyzeruj resztę struktury

    if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    int option=3;

    while(1){
        switch(option){
            case 1:
                if((read_string(sockfd))==-1)
						perror("read");
					
				
                option=2;
                break;
            case 2:
                showMenu();
                scanf("%d",&option);
                break;
            case 3:
				string="1.";
                if ((write_string(sockfd,string))==-1)
						perror("send");
                option=1;
                break;
			case 4:
				string="2.";
				if ((write_string(sockfd,string))==-1)
						perror("send");
				if((read_string(sockfd))==-1)
						perror("read");
				
				string=choose_if();
				if (string==NULL)
				{
					perror("Powrót z wybierania interfejsu niewłaściwy\n");
					break;
				}
				if ((write_string(sockfd,string))==-1)
						perror("send");
                option=1;
                break;
			case 5:
				string="3.";
				if ((write_string(sockfd,string))==-1)
						perror("send");
				if((read_string(sockfd))==-1)
						perror("read");
				
				string=setMac();
				if (string==NULL)
				{
					perror("Powrót z wprowadzania MACu niewlasciwy\n");
					break;
				}
				if ((write_string(sockfd,string))==-1)
						perror("send");
                option=1;
				break;
			default:
				printf("Wybrales nieistniejaca opcje\n");
				break;
        }

    }
    return 0;
}

void showMenu(){
    printf("1:Odbierz\n2:Menu\n3:Pobierz listę interfejsów\n4:Informacje o interfejsie\n5:Ustaw MAC");
}

int write_string(int new_fd,const char *string)
{
    	size_t len=strlen(string);
		if( write( new_fd, &len, sizeof(len) ) == - 1 )
		{
        	perror( "send" );
			return -1;
		}
		
    	if( write( new_fd, string, len) == - 1 )
		{
        	perror( "send" );
			return -1;
		}
}
   

int read_string(int fd)
{
    size_t t_len;
	char* bufor;
	int numbytes;
	
    if ((read(fd, &t_len, sizeof(t_len)))==-1)
	{
		perror("read");
		return -1;
	}
    bufor=(char*)malloc(sizeof(t_len));
    if((numbytes=read(fd, bufor, t_len))==-1)
	{
		perror("read");
		return -1;
	}
	
	bufor[numbytes]='\0';
	printf("Received: %s\n",bufor);
	//free(bufor);
	 
}
char* choose_if()
{
	int opcja;
	char* bufor;
	printf("1.Wszyskie interfejsy.\n2.eth0");
	scanf("%d",&opcja);
	switch(opcja)
	{
		case 1:
			bufor="ALL";
			break;
		case 2:
			bufor="eth0";
			break;
		default:
			printf("Wybrales nieistniejaca opcje\n");
			return NULL;
			
	}
	return bufor;
}
char* setMac()
{
	char* bufor;
	scanf("%s",bufor);
	return bufor;
}
