#include "fonctions.h"


union semun {
    struct semid_ds *buf;    /* Tampon pour IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Tableau pour GETALL, SETALL */
    struct seminfo  *__buf;  /* Tampon pour IPC_INFO */
};


int nbLigne(FILE *file) {
    int nbLignes = 0;
    int cpt;
    while((cpt = fgetc(file)) != EOF) if(cpt == '\n') nbLignes++;
        rewind(file);
        return nbLignes;
    }

int taille = 0; //Pour plus de modularité plus tard

int initTaille(){
    FILE* file = NULL;
    file = fopen("data.txt","r");
    taille = nbLigne(file);
}

void InitDataFromFile(struct dataStruct *data) {
    FILE* file = NULL;
    file = fopen("data.txt","r");
    taille = nbLigne(file);

    if(file != NULL) {
        for(int i = 0; i < taille; i++) {
           if(fscanf(file,"%s %i %i", data[i].site,&data[i].go,&data[i].cpu) == 0) {
               printf("Erreur fscanf !");
               exit(0);
           }
           data[i].maxGo = data[i].go;
           data[i].maxCpu = data[i].cpu;
        }
    }
    else {
        printf("erreur file");
    }
}


void affichageEtat(struct dataStruct* data) {
    printf(YEL"                 --- AFFICHAGE ETAT SYSTEME --- \n \n");
for(int i = 0; i < taille;i++) {
    printf("Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",data[i].site,data[i].go,data[i].maxGo,data[i].cpu,data[i].maxCpu);
}
printf(RED"|||| POUR TOUTE DEMANDE VEUILLEZ LIRE NOTRE DOCUMENTATION |||| \n" RESET);

}


int isInt(char * msg){
    int size = strlen(msg);
    int mult = 1;
    int number = 0;
    for (int i = size-1; i > -1; --i)//je vais de la droite vers la gauche
    {
        if(isdigit(msg[i])){
            number = number + ((msg[i] - 48) * mult); //ex : number = number (char '5' - 48) * 1 sachant que int('5') = 5+48
            mult = mult * 10; //je change de de mutiple  15  = 5*1 + 1*10
       }
       else
        return -1;
    }
    return number;
}
 

int sendFunction(int dsClient, char *message, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = send(dsClient,message + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur sendFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur sendFunction()");
            return 0;
        }
        nbOctet += value;
    }
    return nbOctet;
}

int RecvFunction(int dsClient, char *reponse, int taille) {
    int value = 0;
    int nbOctet = 0;

    while(nbOctet != taille) {
        value = recv(dsClient,reponse + nbOctet,taille - nbOctet,0);

        if(value == -1) {
            printf("Erreur RecvFunction() \n");
            return -1;
        }
        else if(value == 0) {
            printf("Erreur RecvFunction()");
            return 0;
        }
        nbOctet += value;
    }
    return 1;
}

int sendAll(int socket,char * m){
    int size = strlen(m);
    int returnSend; 

    if(sendFunction(socket, (char*)&size, sizeof(size))<1){
        printf("sendSize \n");
        return -1;}

    if(sendFunction(socket, m, size)<1){
        printf("sendMsg\n");
        return -1;}
    return 1;

}

int recvAll(int socket,char * msg){
    int size =0;
    int returnSend; 

    if(RecvFunction(socket, (char*)&size, sizeof(size))<1){
        printf("recvSize \n");
        return -1;}

    char * buffer = malloc(sizeof(char) * (size + 1));

    if(RecvFunction(socket, buffer, size)<1){
        printf("recvMsg \n");
        return -1;}

   // printf("Serveur : %s a envoyé : <%s> \n", client.name, buffer);

    strcpy(msg, buffer);//enregiste le message dans la variable msg
    return 1;

}

