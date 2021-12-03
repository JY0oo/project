#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>

#define BUF_SIZE 100
#define NORMAL_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

void menu();
void changeName();
void menuOptions(int sock);
void dutchpay();
void minigame();
void filetransfer(int sock);
void filedownload(int sock);
void filenamewrite(int sock);

char filename[30];
char name[NORMAL_SIZE]="[DEFALT]";     // name
char msg_form[NORMAL_SIZE];            // msg form
char serv_time[NORMAL_SIZE];        // server time
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // server port number
char clnt_ip[NORMAL_SIZE];            // client ip address
volatile int flagz=0;
volatile int flagzz=0;
volatile int gameflag=0;
char team[2];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;

    int result,total,person;

    if (argc!=4)
    {
        printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

    /** local time **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);
    sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour,
            t->tm_min);

    sprintf(name, "  [%s] -", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" conncet() error");

    // call menu
    menu();
    //send, recvS separate
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void* send_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    char myInfo[BUF_SIZE];
    char* who = NULL;
    char temp[BUF_SIZE];
    int price;
    char totalprice[100];
    int howmany;
    char howm[100];

    int gamenum;//
    char gamec[100];

    char under[]="under\n";
    char up[] ="up\n";
    char please[] = "please number\n";
    int k=0;

    /** send join messge **/
    printf(" >> join the chat !! \n");
    sprintf(myInfo,  "%s's join. IP_%s\n",name , clnt_ip);
    write(sock, myInfo, strlen(myInfo));

    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);

        // menu_mode command -> !menu
        if (!strcmp(msg, "!menu\n"))
        {//input menu -> can select
            menuOptions(sock);

        }

        if(flagz==1)//ducth pay
        {



            strcpy(msg,"`");//function use flag
            write(sock, msg, 1);

            printf("Input How many? : ");
            scanf("%d",&howmany);
            sprintf(howm,"%d",howmany);
            write(sock, howm, 2);//People

            printf("Input total price : ");
            scanf("%d",&price);
            sprintf(totalprice,"%d",price);
            write(sock, totalprice, 10);//Total Price


            flagz=0;
            memset(msg,0,sizeof(msg));
            continue;

        }

        else if (flagz==2)
        {

            while(flagz==2)//////game//////////////////////////////////
            {//int i;

                strcpy(msg,")");
                write(sock, msg, 1);
                memset(msg,0,sizeof(msg));

                printf("\n 야구게임 입니다~!  3자리 숫자를 입력하세요 (0 제외): \n");
                scanf("%d",&gamenum);
                //printf(" Enter Please \n");
                sprintf(gamec,"%d",gamenum);
                write(sock, gamec,4);

            }

            flagz=0;
            memset(msg,0,sizeof(msg));
            continue;//dkfo write tlfgodgkwl dksgrldnlgo
        }

        else if (flagz==3)
        {
            memset(name_msg,0,sizeof(name_msg));
            filetransfer(sock);
//	memset(name_msg,0,sizeof(name_msg));
            flagz=0;
            continue;
        }

        else if (flagz==4)
        {
            memset(name_msg,0,sizeof(name_msg));
            filenamewrite(sock);

            flagzz=9;
            flagz=0;
            usleep(1000000);
            continue;
        }


        else if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }

        // send message qq, 2002
        sprintf(name_msg,   "%s %s", name,msg);
        write(sock, name_msg, strlen(name_msg));//2
    }
    return NULL;
}

void* recv_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    int str_len;
    char buf[2];
    char temp[3];
    int a;
    while(1)
    {
        if(flagz==2)///////game///////////
        {
            read(sock, buf, BUF_SIZE);
            if(buf[0] == 3)
            {	printf("Congratulations!  \n");
                memset(name_msg,0,sizeof(name_msg)); //초기화
                flagz=0; //초기화
            }
            else printf("%d Strike ,%d ball, %d out \n", buf[0], buf[1], buf[2]);
        }

        else if(flagz==4 || flagzz==9 )
        {

            FILE *fp;
            char filebuf[100];
            memset(filebuf, 0x00,100);
            int read_cnt;
            char filesize[5];
            int ifsize = 0;


            strcpy(filesize,name_msg);//read name_msg, filesize
            fp=fopen(filename,"wb");

            ifsize = atoi(filesize);

            memset(name_msg,0,sizeof(name_msg));//name_msg = 0
            usleep(400000);

            read_cnt=read(sock,filebuf,ifsize);//file read
            fwrite((void*)filebuf, 1, read_cnt, fp);//file fwrite
            printf(" %s is stored!! ",filename);
            fclose(fp);



            flagz=0;
            flagzz=0;
        }

        str_len=read(sock, name_msg, NORMAL_SIZE+BUF_SIZE-1);

        if (str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        fputs(name_msg, stdout);
    }
    return NULL;
}//flagz=2;


void menuOptions(int sock)
{
    int select;

    // print menu
    printf("\n\t**** menu mode ****\n");
    printf("\t1. change name\n");
    printf("\t2. clear/update\n\n");
    printf("\t3. dutchpay\n\n");
    printf("\t4. minigame\n\n");
    printf("\t5. file transfer\n\n");
    printf("\t6. file download\n\n");
    printf("\tthe other key is cancel");
    printf("\n\t*******************");
    printf("\n\t>> ");

    scanf("%d", &select);
    getchar();
    switch(select)
    {
        // change user name
        case 1:
            changeName();flagz=0;
            break;

            // console update(time, clear chatting log)
        case 2:
            menu();flagz=0;
            break;

        case 3:
            printf("dutchpay funtion start\n");
            flagz=1;
            break;

        case 4:
            printf("minigame funtion start\n");
            flagz=2;
            break;

        case 5:
            printf("filetransfer funtion start\n");
            flagz=3;
            break;

        case 6:
            printf("filedownload funtion start\n");
            flagz=4;
            break;

            // menu error
        default:
            printf("\tcancel.");flagz=0;
            break;
    }
}

void dutchpay()
{
/*
int price;
char totalprice[100];
int howmany;
char howm[100];
		strcpy(msg,"!price");
		write(sock, msg, strlen(msg));

	printf("Input total price : ");
	scanf("%d",&price);
	sprintf(totalprice,"%d",price);
	write(sock, totalprice, strlen(totalprice));

	printf("Input How may? : ");
	scanf("%d",&howmany);
	sprintf(howm,"%d",howmany);
	write(sock, howm, strlen(howm));
*/
}

void minigame()
{
}

void filetransfer(int sock)
{

    int i = 0;

    FILE *fp;
    char filebuf[256];
    int read_cnt;
    char name_cnt[2];

    int ifsize = 0;
    char fsize[5];
//memset(filebuf, 0x00, 256);

    strcpy(msg,"_");
    write(sock, msg, 1);//flag write

    printf("Input filename :");
    fgets(filename, 20, stdin);

    for(i = 0 ; filename[i] !=0; i++)
    {// if ==\n
        if(filename[i] == '\n') {
            filename[i] = 0;
            break;
        }
    }

    sprintf(name_cnt,"%d",strlen(filename));
    write(sock,name_cnt,2);//filename length

    write(sock,filename,(strlen(filename))); // filename notice


    fp=fopen(filename,"rb");
    fseek(fp, 0, SEEK_END);
    ifsize = ftell(fp);//fsize == filesize
    fseek(fp, 0, SEEK_SET);

    sprintf(fsize,"%d",ifsize);
    write(sock,fsize,5);
    if(fp!=NULL)
    {

        while(1)
        {
            read_cnt=fread((void*)filebuf,1,ifsize,fp);
            if(read_cnt<ifsize)
            {
                write(sock,filebuf,read_cnt);
                break;
            }
            write(sock,filebuf,read_cnt);
        }
    }

    flagz=0;
    fclose(fp);


}

void filedownload(int sock)
{
/*
int i = 0;

FILE *fp;
char filebuf[100];
 memset(filebuf, 0x00,30);
int read_cnt;
char name_cnt[2];
char filesize[5];
int ifsize = 0;
char fsize[5];
char temp[10];
//memset(filebuf, 0x00, 256);

	read(sock,filesize,5); //file size read
	fp=fopen(filename,"wb");
  //  pthread_mutex_lock(&mutx);
ifsize = atoi(filesize);
//printf("\n%s\n",filesize);//size rhdqor;
printf("\n%s\n",filename);
printf("\n%s\n",filebuf);
printf("\n%d\n",ifsize);

	read_cnt=read(sock,filebuf,ifsize);//file read

	fwrite((void*)filebuf, 1, read_cnt, fp);//file fwrite
 //   pthread_mutex_unlock(&mutx);
	fclose(fp);
*/

}

void filenamewrite(int sock)
{
    int i = 0;

    FILE *fp;
    char filebuf[256];
    int read_cnt;
    char name_cnt[2];
    char filesize[5];
    int ifsize = 0;
    char fsize[5];
//memset(filebuf, 0x00, 256);

    strcpy(msg,"}");
    write(sock, msg, 1);//flag write

    printf("Input filename :");
    fgets(filename, 20, stdin);

    for(i = 0 ; filename[i] !=0; i++)
    {// if ==\n
        if(filename[i] == '\n') {
            filename[i] = 0;
            break;
        }
    }

    sprintf(name_cnt,"%d",strlen(filename));
    write(sock,name_cnt,2);//filename length

    write(sock,filename,(strlen(filename))); // filename notice

}



/** change user name **/
void changeName()
{
    char nameTemp[100];
    printf("\n\tInput new name -> ");
    scanf("%s", nameTemp);
    strcpy(msg,name);
    strcat(msg," --> change name! --> ");
    sprintf(name, "  [%s] -", nameTemp);
    printf("\n\tComplete.\n\n");

}

void menu()
{
    system("clear");
    printf(" **** moon/sum chatting client ****\n");
    printf(" server port : %s \n", serv_port);
    printf(" client IP   : %s \n", clnt_ip);
    printf(" chat name   : %s \n", name);
    printf(" server time : %s \n", serv_time);
    printf(" ************* menu ***************\n");
    printf(" if you want to select menu -> !menu\n");
    printf(" 1. change name\n");
    printf(" 2. clear/update\n");
    printf(" **********************************\n");
    printf(" Exit -> q & Q\n\n");
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
