#include "fonctions.h"




void initMutex(pthread_mutex_t verrou,pthread_cond_t cond) {
    pthread_mutex_init(&verrou,NULL);
    pthread_cond_init(&cond,NULL);
}


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

           for (int j = 0; j < 100; ++j)
           {
               strcpy(data[i].LSGo[j].name,"");
               data[i].LSGo[j].quantity = 0;

               strcpy(data[i].LSCpu[j].name,"");
               data[i].LSCpu[j].quantity = 0;
           }
        }
    }
    else {
        printf("erreur file");
    }
}

void initClient(struct clientStruct * client,char * name,int socket, int socketServer, char * ip ,char * port,struct dataStruct * data){
    strcpy(client->name,name);
    client->socket = socket;
    client->socketServer = socketServer;
    strcpy(client->ip,ip);
    strcpy(client->port,port);
    client->data = data;

    for (int i = 0; i < 100; ++i)
    {
        strcpy(client->exclu[i].site,"");
       client->exclu[i].quantity = 0;
    }
}

void affichageClient(struct clientStruct client){
    printf(GRN"%s : ",client.name);
    printf("socketSend: %i ",client.socket );
    printf("socketRecv %i ",client.socketServer );
    printf("ip: %s ", client.ip );
    printf("port: %s\n"RESET, client.port );
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
int positionSite(struct dataStruct * data, char * site){
    for (int i = 0; i < taille; ++i)
    {
        if( strcmp(data[i].site,site)==0){
            return i;
        }
    }
    return -1;
}

int lSharedSize(struct LShared * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].name)==0) && (l[i].quantity==0)) //vérifie si le nom et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

int lExcluSize(struct LExclu * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].site)==0)&& (l[i].quantity==0)) //vérifie si le site et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

/*Diminue le nombre de de go et cpu dans la structure*/
void actionExclu(struct dataStruct* data, char * site, char * type, int value){
    int position = positionSite(data,site);

    if (strcmp(type,"GO")==0)
    {
        data[position].go = data[position].go - value;
    }

    if (strcmp(type,"CPU")==0)
    {
        data[position].cpu = data[position].cpu - value;
    }
}

/*Ajoute le client dans la structure partagé*/
void actionShared(struct dataStruct* data,char * site, char * name, char * type, int value){
    int position = positionSite(data,site);    
    struct LShared l;
    strcpy(l.name , name);
    l.quantity = value;


    if (strcmp(type,"GO")==0)
    {   
        int size = lSharedSize(data[position].LSGo);
        data[position].LSGo[size]= l;
    }

    if (strcmp(type,"CPU")==0)
    {
        int size = lSharedSize(data[position].LSCpu);
        data[position].LSCpu[size] = l;
    }

}

void *triggerAffichage(void *param) {
}

void *Reservation(void *param) {
}