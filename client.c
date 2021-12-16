#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>




#define BUF_SIZE 100
#define NORMAL_SIZE 20

void* send_msg(void* arg);
void error_handling(char* msg);
ssize_t writen(int socketFD, const char *buffer, size_t fixedLength);
ssize_t writeHeader(int socketFD, const char* buffer, size_t length);
void menu();
void changeName();
void menuOptions(int sock);

char name[NORMAL_SIZE]="[DEFALT]";     // name
char serv_time[NORMAL_SIZE];        // server time
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // server port number
char clnt_ip[NORMAL_SIZE];            // client ip address
volatile int flagz = 0;
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;

    if (argc!=4) {
        printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

    /** local time **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);
    sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);

    sprintf(name, "  [%s] -", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" conncet() error");

    // call menu
    menu();
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    close(sock);
    return 0;
}

void* send_msg(void* arg) {
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    int gamenum;
    char flag[2], game_input[10], game_result[10];
    char cal[3];
    char add[2];
    int num1;
    char num_1[100];
    char num_2[100];
    int num2;

    while(1) {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "!menu\n")) {
            menuOptions(sock);
        }
        if(flagz==1)//..........................................
        {
            strcpy(msg,"`");//function use flag
            write(sock, msg, 1);

            printf("First number  : ");
            scanf("%d",&num1);
            sprintf(num_1, "%d", num1);
            write(sock, num_1, 100);// 첫번째 숫자

            printf(" +, -,  * , / : ");
            scanf("%s", cal);
            sprintf(add, "%s", cal);
            write(sock, add, 2);// 사칙 연산

            printf(" Second number : ");
            scanf("%d",&num2);
            sprintf(num_2,"%d",num2);
            write(sock, num_2, 100); // 두번째 숫자

            flagz=0;
            memset(msg,0,sizeof(msg));
            continue;
        }
        else if (flagz == 2) {
            strcpy(flag, ")");
            if (write(sock, flag, 1) < 0)
                perror("flag write error");
            while(1) {
                printf("\n 야구게임 입니다~!  3자리 숫자를 입력하세요 (0 제외): \n");
                scanf("%d", &gamenum);
                while(getchar() != '\n');
                sprintf(game_input,"%d", gamenum);
                if (write(sock, game_input,strlen(game_input)) < 0)
                    perror("game_input write error");

                if (read(sock, game_result, 10) < 0)
                    perror("game_result read error");

                printf("strike : %d, ball : %d, out : %d\n",
                       (int)game_result[0] - 48, (int)game_result[1] - 48, (int)game_result[2] - 48);
                if ((int)game_result[0] - 48 == 3) {
                    printf("You Win\n");
                    printf("Return to Chat Server\n");
                    break;
                }
            }
            flagz = 0;
            memset(msg,0,sizeof(msg));
            continue;
        }
        else if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        if (write(sock, name_msg, strlen(name_msg)) < 0)
            perror("name msg write error");
    }
    return NULL;
}
void menuOptions(int sock) {
    int select;
    // print menu
    printf("\n\t------- 메뉴 --------\n");
    printf("\t1. 닉네임 변경 \n");
    printf("\t2. 화면 초기화 \n\n");
    printf("\t3. 베스킨라빈스 31 게임 \n\n");
    printf("\t4. 야구 게임 \n\n");

    printf("\tthe other key is cancel");
    printf("\n\t-----------------------");
    printf("\n\t>> ");

    scanf("%d", &select);
    while(getchar() != '\n');

    switch(select) {
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
void changeName() {
    char nameTemp[100];
    printf("\n\tInput new name -> ");
    scanf("%s", nameTemp);
    strcpy(msg,name);
    strcat(msg," --> change name! --> ");
    sprintf(name, "  [%s] -", nameTemp);
    printf("\n\tComplete.\n\n");
}
void menu() {
    system("clear");
    printf(" ====== chatting client ======\n");
    printf(" server port : %s \n", serv_port);
    printf(" client IP   : %s \n", clnt_ip);
    printf(" chat name   : %s \n", name);
    printf(" server time : %s \n", serv_time);
    printf(" ================================\n");
    printf(" if you want to select menu -> !menu\n");
    printf(" ====================================\n");
    printf(" Exit -> q & Q\n\n");
}
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
ssize_t writen(int socketFD, const char *buffer, size_t fixedLength)
{
    size_t leftBytes = fixedLength;
    ssize_t writenBytes = 0;
    const char *readingPointer = buffer;

    while(leftBytes > 0)
    {
        if((writenBytes = write(socketFD, readingPointer, leftBytes)) <= 0)
        {
            if(errno == EINTR)
                writenBytes = 0; // Write again
            else
                return -1;
        }
        else
        {
            leftBytes -= writenBytes;
            readingPointer += writenBytes;
            // Write n bytes
        }
    }

    return fixedLength - leftBytes;
}
ssize_t writeHeader(int socketFD, const char* buffer, size_t length)
{
    size_t messageLength = htonl(length);
    ssize_t writeBytes;

    // 길이를 먼저 보낸다.
    writeBytes = writen(socketFD , (char*)&messageLength, sizeof(size_t));

    if(writeBytes != sizeof(size_t))
        return writeBytes < 0 ? -1 : 0;

    // 그다음 문자열을 보낸다.
    writeBytes = writen(socketFD, buffer, length);

    if(writeBytes != length)
        return writeBytes < 0 ? -1 : 0;

    return writeBytes;
}
