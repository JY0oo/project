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
void menu();
void changeName();
void menuOptions(int sock);

typedef struct CalcInfo {
    double num1;
    double num2;
    char operator;
    double result;
} CalcInfo;

char name[NORMAL_SIZE]="[DEFALT]";     // name
char serv_time[NORMAL_SIZE];        // server time
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // server port number
char clnt_ip[NORMAL_SIZE];            // client ip address
volatile int flagz = 0;
pthread_mutex_t mutx;

void input_fflush() {
    while (1) {
        if (getchar() == '\n') {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread;
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

    while(1) {
        fgets(msg, BUF_SIZE, stdin);

        if (!strcmp(msg, "!menu\n")) {
            menuOptions(sock);
        }
        //Calculator
        if (flagz == 1) {
            strcpy(flag,"/"); //function use flag
            if (write(sock, flag, 1) < 0)
                perror("flag write error");

            CalcInfo *calc_info = (CalcInfo *)malloc(sizeof(CalcInfo));

            printf("\n Calculator Operation Start \n");

            printf("First number  : ");
            scanf("%lf", &(calc_info->num1));

            printf("+, -,  * , / : ");
            scanf("%s", &(calc_info->operator));

            printf("Second number : ");
            scanf("%lf", &(calc_info->num2));
            calc_info->result = 0;

            if (write(sock, calc_info, sizeof(CalcInfo)) < 0)
                perror("calc_info write error");

            if (read(sock, calc_info, sizeof(CalcInfo)) < 0)
                perror("calc_info read error");

            printf("%g %c %g = %g\n", calc_info->num1, calc_info->operator, calc_info->num2, calc_info->result);
            printf("Return to Chat Server\n");
            flagz = 0;
            memset(msg,0,sizeof(msg));
            input_fflush();
            continue;
        }
        else if (flagz == 2) {
            strcpy(flag, ")");
            if (write(sock, flag, 1) < 0)
                perror("flag write error");
            while(1) {
                printf("\n 야구게임 입니다~!  3자리 숫자를 입력하세요 (0 제외): \n");
                scanf("%d", &gamenum);
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
            input_fflush();
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
    printf("\t3. 계산기 \n\n");
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
            printf(" 계산기 실행  \n");
            flagz=1;
            break;
        case 4:
            printf("minigame 실행 \n");
            flagz=2;
            break;
            // menu error
        default:
            printf("\tcancel.");flagz=0;
            break;
    }
}
void changeName() {
    char nameTemp[100];
    printf("\n\t새로운 이름을 입력하세요 -> ");
    scanf("%s", nameTemp);
    strcpy(msg,name);
    strcat(msg," --> change name! --> ");
    sprintf(name, "  [%s] -", nameTemp);
    printf("\n\t변경 완료.\n\n");
}
void menu() {
    system("clear");
    printf(" ====== chatting client ======\n");
    printf(" 서버 포트 : %s \n", serv_port);
    printf(" 클라이언트 IP   : %s \n", clnt_ip);
    printf(" 사용자 이름  : %s \n", name);
    printf(" 서버 시간 : %s \n", serv_time);
    printf(" ================================\n");
    printf(" 메뉴 이용시 !menu\n");
    printf(" ====================================\n");
    printf(" Exit -> q & Q\n\n");
}
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
