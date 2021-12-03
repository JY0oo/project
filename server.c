#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>

#define BUF_SIZE 100
#define MAX_CLNT 100 //max socket comunication 100
#define MAX_IP 30

void * handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);
char* serverState(int count);
void menu(char port[]);
char msgpay[BUF_SIZE];
int flagz=0;
//volatile int gameflag=0;

/****************************/

int clnt_cnt=0;//how much clnt ?
int clnt_socks[MAX_CLNT]; // max join 100, socket [100]
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;
    //socket create, and thread ready
    /** time log **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);

    if (argc != 2)
    {
        printf(" Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    //port input please
    menu(argv[1]);
    //information
    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));
    //in serv_sock
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if (listen(serv_sock, 5)==-1)
        error_handling("listen() error");
    //error check
    while(1)
    {//loop accept
        t=localtime(&timer);
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock;//new client join macthing clnt_sock[]
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);//thread
        pthread_detach(t_id);
        printf(" Connceted client IP : %s ", inet_ntoa(clnt_adr.sin_addr));
        printf("(%d-%d-%d %d:%d)\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday,
               t->tm_hour, t->tm_min);//join time
        printf(" chatter (%d/100)\n", clnt_cnt);
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg) //in thread
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];

    char filename[100];
    FILE *fp;
    char filebuf[100];

    char name_cnt[2];

    size_t bufsize = 0;
    int nbyte;
    memset(filebuf, 0x00,30);
    char filesize[5];

    int read_cnt;


    char totalprice[100];
    char howm[100];
    int price =1;
    int howmany =1;
    int result=1;
    char result_c[100];

    int gamenum;
    char gamec[100];
    char temp[100];

    char flag[2];
    char gamemsg[20];

    int win[3],won[3];
    int nan;
    int buf1=0;
    int o,s,b=0;
    int j=0;
    char under[2]; //base
    char up[2]; // out
    char please[2]; //strike
    char buf[BUF_SIZE];
    while(1) {
        srand((unsigned) time(NULL));
        nan = (rand() % 900) + 100;
        won[0] = nan / 100;
        won[1] = (nan % 100) / 10;
        won[2] = nan % 10;
        if ( won[0] != 0 && won[1] != 0 && won[2] != 0 && won[0] != won[1] && won[0] != won[2] && won[1] != won[2] )
            break;
    }
    printf("%d %d %d\n", won[0], won[1], won[2]);

    while(1)
    {//caculater ++ function
        read(clnt_sock,flag,1);


        if(strcmp(flag,"`")==0)//ducth pay
        {

            read(clnt_sock, howm, 2);//People
            howmany = atoi(howm);

            read(clnt_sock, totalprice, 10);//Total Price
            price = atoi(totalprice);


            strcpy(msg," people : ");
            strcat(msg,howm);

            strcat(msg,",  totalprice : ");//msg add information
            strcat(msg,totalprice);

            strcat(msg," won ,  Per person: ");
            result = price/howmany; // caculate
            sprintf(result_c,"%d",result);
            strcat(msg,result_c);//msg add result

            strcat(msg," won -");
            str_len=strlen(msg);
        }

        else if (strcmp(flag,")")==0) /////game//////////////////////////////////
        {
            while(strcmp(flag,")")==0)
            {
                read(clnt_sock,gamec,4);

                buf1 =atoi(gamec);
                win[0] = buf1 / 100;
                win[1] = (buf1 % 100) / 10;
                win[2] = buf1 % 10;

                //	strcpy(msg,gamec);
                str_len=strlen(msg);

                for(i=0;i<3;i++)
                {
                    for(j=0;j<3;j++)
                    {
                        if(i == j)
                        {
                            if(won[i] == win[j])
                                s++;
                        }
                        else if(won[i] == win[j])
                        {
                            b++;
                        }
                    }
                }
                o = 3 - (s + b);
                if(s==3)
                {
                    buf[0] = s;
                    buf[1] = b;
                    buf[2] = o;
                    write(clnt_sock, buf,BUF_SIZE);
                    break;
                }
                buf[0] = s;
                buf[1] = b;
                buf[2] = o;

                write(clnt_sock, buf, BUF_SIZE);
                s=0;
                b=0;
                o=0;
            }
            memset(msg,0,sizeof(msg));
        }

        else if (strcmp(flag,"_")==0)
        {

            memset(msg,0,sizeof(msg));
            read(clnt_sock, name_cnt,2);
            read(clnt_sock, filename, atoi(name_cnt));//filename read
            fp=fopen(filename,"wb");

            read(clnt_sock,filesize,5); //file size read

            int asize = atoi(filesize);
            read_cnt=read(clnt_sock,filebuf,asize);//file read

            fwrite((void*)filebuf, 1, read_cnt, fp);//file fwrite

            strcpy(msg,filesize);

            strcpy(msg,filename);
            strcat(msg," stored :\n");
            strcat(msg,filebuf);

            str_len=strlen(msg);
            fclose(fp);

        }

        else if (strcmp(flag,"}")==0)
        {
            int ifsize = 0;
            char fsize[5];
            memset(msg,0,sizeof(msg));
            read(clnt_sock, name_cnt,2);
            read(clnt_sock, filename, atoi(name_cnt));//filename read
            //good
            fp=fopen(filename,"rb");//file open

            fseek(fp, 0, SEEK_END);
            ifsize = ftell(fp);//fsize == filesize
            fseek(fp, 0, SEEK_SET);

            sprintf(fsize,"%d",ifsize);
            write(clnt_sock,fsize,5);//file size write good


            if(fp!=NULL)
            {//fail

                read_cnt=fread((void*)filebuf,1,ifsize,fp);//file read

            }
            usleep(500000);
            strcpy(msg,filebuf);//go msg, filebuf
            fclose(fp);

        }


        else
        {
            str_len=read(clnt_sock, msg, sizeof(msg));
//30000===================================================
            if(str_len==0)
            {break;}
        }

        send_msg(msg, str_len);//read and write all clnt_cnt[]
    }
    // remove disconnected client
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
        if (clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}



void send_msg(char* msg, int len)
{
    int i;
    pthread_mutex_lock(&mutx);

    for (i=0; i<clnt_cnt; i++)//all clnt
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

char* serverState(int count)
{
    char* stateMsg = malloc(sizeof(char) * 20);
    strcpy(stateMsg ,"None");

    if (count < 5)
        strcpy(stateMsg, "Good");
    else
        strcpy(stateMsg, "Bad");

    return stateMsg;
}

void menu(char port[])
{
    system("clear");
    printf(" **** moon/sun chat server ****\n");
    printf(" server port    : %s\n", port);
    printf(" server state   : %s\n", serverState(clnt_cnt));
    printf(" max connection : %d\n", MAX_CLNT);
    printf(" ****          Log         ****\n\n");
}