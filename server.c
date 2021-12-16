

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
const char* recv_str(int sock);
void send_str(int sock, char *buf);
int flagz=0;

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
    int clnt_answer[3], correct_answer[3], check, i, j;
    char game_input[10], game_result[10], chat_msg[BUF_SIZE], flag[2];
    int str_len;
    int out = 0, strike = 0, ball = 0;
    char cal[100];
    char num1[100];
    char num2[100];
    int num_2 =1;
    int num_1 =1;
    int result=1;
    char msg[BUF_SIZE];
    char result_c[100];
    while(1) {
        // Select menu by the flag
        if (read(clnt_sock,flag,1) < 0)
            perror("flag read error");
        // Bulls and cows game start
        if(strcmp(flag,"`")==0)//ducth pay
        {

            read(clnt_sock, num1, BUF_SIZE);//첫번째
            num_1 = atoi(num1);

            read(clnt_sock, cal, 2);//기호

            read(clnt_sock, num2, 2); // 두번째
            num_2 = atoi(num2);
            if (cal== "+"){
                result = num_2 + num_1; // caculate
                sprintf(result_c,"%d",result);
                strcat(msg,result_c);
            }
            else if(cal == "-"){
                result = num_1 - num_2 ; // caculate
                sprintf(result_c,"%d",result);
                strcat(msg,result_c);
            }
            else if(cal == "/"){
                result = num_1/num_2 ; // caculate
                sprintf(result_c,"%d",result);
                strcat(msg,result_c);
            }
            else if(cal == "*"){
                result = num_1 * num_2 ; // caculate
                sprintf(result_c,"%d",result);
                strcat(msg,result_c);
            }
            /*strcpy(msg," people : ");
            strcat(msg, num1);

            strcat(msg,"  cal : ");//msg add information
            strcat(msg, cal);

            strcat(msg," won ,  Per person: ");
            */

            strcat(msg," result = ");
            str_len=strlen(msg);
        }
        else if (strcmp(flag, ")") == 0) {
            // correct_answer random generation
            i = 1;
            srand((unsigned int)time(NULL));
            correct_answer[0] = rand() % 10;
            while(i < 3) {
                correct_answer[i] = rand() % 10;
                for (check = 0 ; check < i; check++)
                    if (correct_answer[check] == correct_answer[i]) break;
                if (check == i) i++;
            }
            printf("correct_answer is : %d %d %d\n", correct_answer[0], correct_answer[1], correct_answer[2]);
            while (1) {
                // Read client answer
                if (read(clnt_sock,game_input,10) < 0)
                    perror("game_input read error");
                for (i = 0; i < 3; i++)
                    clnt_answer[i] = (int)game_input[i] - 48;
                // Calculate strike, ball, out
                strike = ball = out = 0;
                for (i = 0; i < 3; i++) {
                    for (j = 0; j < 3; j++) {
                        if (correct_answer[i] == clnt_answer[j]) {
                            if (i == j)
                                strike++;
                            else
                                ball++; } } }
                out = 3 - (strike + ball);
                // Send game result
                sprintf(game_result, "%d%d%d", strike, ball, out);
                if (write(clnt_sock, game_result, strlen(game_result)) < 0)
                    perror("game_result write error");
                if (strike == 3)
                    break;
            }
        }
        else {
            str_len = read(clnt_sock, chat_msg, sizeof(chat_msg));
            if (str_len == 0) break;
        }
        send_msg(chat_msg, str_len);
    }
    // remove disconnected client
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++) {
        if (clnt_sock==clnt_socks[i]) {
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

void send_msg(char* msg, int len) {
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
    printf(" ====== chatting server ======\n");
    printf(" server port    : %s\n", port);
    printf(" server state   : %s\n", serverState(clnt_cnt));
    printf(" max connection : %d\n", MAX_CLNT);
    printf(" ================================\n");
}

void send_str(int sock, char *buf) {
    if (write(sock, buf, sizeof(char) * BUF_SIZE) < 0)
        perror("write() error");
}
const char* recv_str(int sock) {
    static char buf[BUF_SIZE] = "";
    if (read(sock, buf, sizeof(char) * BUF_SIZE) < 0)
        perror("read() error");
    return buf;
}
