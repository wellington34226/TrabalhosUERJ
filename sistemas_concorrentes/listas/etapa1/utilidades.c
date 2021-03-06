#include "parametros.h"//utilizacao de uma biblioteca predefinida pelo programador.

int sizememo = sizeof(struct memoria);//recuperacao do tamanho da area de memoria compartilhada.

mc criar_mem(void){
	mc mem;
	if(( memoriaid = shmget((ftok(PAI, (key_t)KEY )), sizememo, IPC_CREAT|IPC_EXCL|SHM_R|SHM_W) ) == -1){
		if(( memoriaid = shmget((ftok(PAI, (key_t)KEY )), sizememo,SHM_R|SHM_W) ) == -1){
			printf("%sErro ao tentar criar a area de memoria compartilhada.\n",VERMELHO);  
			fprintf(arq_log,"Erro ao tentar criar a area de memoria compartilhada.\n",VERMELHO);        
			perror("Erro no shmget.");
			printf("%s\n",NORMAL);
			return '\0';
		}
	}else{
		printf("Area de memoria compartilhada criada pelo processo pai - PID %d com id: %d\n",getpid(),memoriaid);
		fprintf(arq_log,"Area de memoria compartilhada ja criada pelo processo pai - PID %d com id: %d.\n",getpid(),memoriaid);
	}
	if( ( mem = shmat(memoriaid,0,0) ) == (mc)-1){
		printf("%sErro ao tentar se acoplar a area de memoria compartilhada.\n",VERMELHO);  
		fprintf(arq_log,"Erro ao tentar se acoplar a area de memoria compartilhada.\n",VERMELHO);        
		perror("Erro no shmat.");
		printf("%s\n",NORMAL);
		return '\0';
	}else{
		mem->boton = 0;
		mem->top = 300;
	}
	return mem;
}//cria/acopla a memoria compartilhada(recuperando o apontador) e tratando os erros

mc acoplar_mem(int id_tp){
	char* padrao;//padrao de cor do processo
	mc mem;
	switch (id_tp){
		case 0:{
			padrao=AZUL;
			break;
		}
		case 1:{
			padrao=VERDE;
			break;
		}
		case 2:{
			padrao=AMARELO;
			break;
		}
		default:{
			break;
		}
	}
	if ( (memoriaid = shmget( ftok(PAI, (key_t)KEY), sizememo, 0666) ) == -1){
		printf("%sFilho %d - PID nao recuperou o id da memoria compartilhadda.\n",VERMELHO,id_tp,getpid());
		fprintf(arq_log,"Filho %d - PID %d nao recuperou o id da memoria compartilhadda.\n",id_tp,getpid());
		perror("Erro no shmget.");
    	}
	else{
		if ( (mem = shmat(memoriaid, NULL, 0666) ) == (mc)-1){
			printf("%sFilho %d - PID nao se acoplou a memoria compartilhada.\n",VERMELHO,id_tp,getpid());
			fprintf(arq_log,"Filho %d - PID nao se acoplou a memoria compartilhada.\n",id_tp,getpid());
			perror("Erro no shmat.");
		}
		else{
			fprintf(arq_log,"O filho %d - PID %d foi acoplado a memoria compartilhada: %d.\n",id_tp,getpid(),memoriaid);
			printf("%sO filho %d - PID %d foi acoplado a memoria compartilhada: %d.\n",padrao,id_tp,getpid(),memoriaid);
			printf("%s\n",NORMAL);
			return mem;
		}
	}
	printf("%s\n",NORMAL);
	return '\0';
}//acopla a memoria compartilhada recuperando o apontador e tratando os erros.

int destruir_mem(int idmemoria){
	if((shmctl(idmemoria,IPC_RMID,0))==-1){
		return 0;
	}//destroi a memoria compartilhada  e verifica se ocorreu erro na destruicao
	return 1;
}//destruir area de memoria compartilhada.

void CapturarCtrlC(int sig){
	signal(sig,SIG_IGN);
	ftime(&fim);
	tempo = ((double)fim.time + (double)fim.millitm*0.001) - ((double)inicio.time + (double)inicio.millitm*0.001);//calcula o tempo de execucao.
	if(!destruir_mem(memoriaid)){
		printf("%sImpossivel destruir area de memoria compartilhada.\n",VERMELHO);
		fprintf(arq_log,"Impossivel destruir area de memoria compartilhada.\n");
		perror("Erro no shmctl");
	}//destroi a memoria compartilhada  e verifica se ocorreu erro na destruicao
	else{
		printf("%sProcesso pai - PID %d.\n",VERMELHO,pai);
		printf("%sExecucao cancelada pelo usuario, ja destrui a area de memoria compartilhada.\n",VERMELHO);
		printf("%sProcessamento teve duracao: %.3lf segundos\nVariavel boton: %d. Variavel top: %d.\n",VERMELHO,tempo,mem_comp->boton,mem_comp->top);
		printf("%sAgora eu irei terminar.\n\n",VERMELHO);
		fprintf(arq_log,"Processo pai - PID %d.\nExecucao cancelada pelo usuario, ja destrui a area de memoria compartilhada.\nProcessamento teve duracao de: %.3lf segundos\nVariavel boton: %d. Variavel top: %d.\nAgora eu irei terminar.\n\n",pai,tempo,mem_comp->boton,mem_comp->top);
	}
	fclose(arq_log);//fecha arquivo
	printf("%s\n",NORMAL);
	exit(0);
}//capturar sinal de ctrl+C

