//bibliotecas utilizadas no codigo.
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <errno.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/msg.h>
//definicoes das constantes utilizadas no programa.
#define KEY 34226
#define EVALEXC 1
#define AMARELO "\033[1;33m" 
#define VERMELHO "\033[0;31m" 
#define VERDE "\033[1;32m"
#define AZUL "\033[1;34m"  
#define NORMAL "\033[0m"
#define PAI "processopai.c"
#define MAXMSG 1024

/*variaveis globais.*/
typedef struct memoria* mc;//definicao de um apontador para a memoria compartilhada.

struct memoria{
	int boton;
	int top;
	int excs[3];
};//estrutura da memoria compartilhada, duas variaveis inteiras e um array com quantidade de execucoes de cada processo.

struct msgtext{
	long mtype;
	char mtext[MAXMSG];
}msg;//definicao de um apontado para a mensagem da fila de mensagens.

struct timeb inicio,fim;//captura em uma estrutura os tempos de inicio e fim.
double tempo;//tempo de execucao fim-inicio
FILE* arq_log;//arquivo de saida, log.
pid_t memoriaid;//id da memoria compartilhada
int semid,fmsgid;//identificador do semaforo e da fila de mensagens
pid_t pai;//pid do pai
mc mem_comp;//apontador para memoria compartilhada

/*Funcoes utilizadas*/
mc criar_mem(void);//criacao da area de memoria compartilhada.
mc acoplar_mem(int id_tp);//acoplamento na area de memoria compartilhada.
int destruir_mem(int idmemoria);//destruir area de memoria compartilhada.
int criar_filamsg(void);//criar fila de mensagens
int destruir_filamsg(int idfila);//destruicao da fila de mensagens

void CapturarCtrlC(int sig);//capturar sinal de ctrl+C

//funcao de execucao dos processos filhos
void filho(int id_tp);//execucao do processo
//funcoes de manipilacao de semaforo
int sem_criar(int iniciaval);//funca ode criacao de semaforo
void sem_pedido(int sem,int semf);//funcao de pedido de entrada na area de memoria compartilhada, bloqueia o semaforo 
void sem_libera(int sem,int semf);//funcao de pedido de saida da area de memoria compartilhada, liberando o semaforo
int sem_destroi(int sem);//funcao que destroi o semaforo
