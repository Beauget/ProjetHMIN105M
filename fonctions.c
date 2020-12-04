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
           data[i].id = i;
           data[i].maxGo = data[i].go;
           data[i].maxCpu = data[i].cpu;

           for (int j = 0; j < 101; ++j)
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

    for (int i = 0; i < 101; ++i)
    {
        strcpy(client->exclu[i].site,"");
        strcpy(client->exclu[i].type,"");
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

void affichageEtatMaj(struct dataStruct* data) {
        printf(YEL"                 --- MISE A JOURS SYSTEME --- \n \n");
        printf(RED" Une réservation a eu lieu, l'état du système a changer \n \n");
    for(int i = 0; i < taille;i++) {
        printf(YEL"Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",data[i].site,data[i].go,data[i].maxGo,data[i].cpu,data[i].maxCpu);
    }
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

/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lSharedSize(struct LShared * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].name)==0) && (l[i].quantity==0)) //vérifie si le nom et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

/*RENVOIE LA QUANTITY MAX DE LA LISTE PARTAGER*/
int maxLShared(struct LShared * l){
    int max = 0;
    int size = lSharedSize(l);

    if (size==0)
        return max;

    for (int i = 0; i < size; ++i)
    {
        if (max<l[i].quantity)       
            max = l[i].quantity;
    }
    return max;
}

int maxLSharedType(struct dataStruct * data,int position,char * type){
    int max = -1;

    if (strcmp(type,"GO")==0)
    {
        max = maxLShared(data[position].LSGo);
    }

    if (strcmp(type,"CPU")==0)
    {
        max = maxLShared(data[position].LSCpu);
    }
    return max;
}



int lSharedPosition(struct LShared * l,char * name){
    int size =  lSharedSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].name,name)==0 && strlen(name)!=0)
            return i;
    }
    return -1;
}

void suppressionShared(struct LShared * l, int position, int size){
    for (int i = 0; i < size; ++i)
    {
        l[i]=l[i+1];
    }
}

/*DONNE LA LONGEUR DE LA LISTE, RENVOIE -1 SI LA LISTE EST PLEINE */
int lExcluSize(struct LExclu * l){
    for (int i = 0; i < 100; ++i)
    {
        if((strlen(l[i].site)==0)&& (l[i].quantity==0)) //vérifie si le site et vide et la quantité est égale à 0;
            return i;
    }
    return -1;
}

/*RENVOIE LA POSITION DU SITE SI IL EST DANS LA LISTE EXCLUSIF -1 SINON*/
int lExcluPosition(struct LExclu * l,char * type, char * site){
    int size = lExcluSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].type,type)==0 && strcmp(l[i].site,site)==0)
            return i;
    }
    return -1;
}

/*MET A JOUR LA STRUCTURE EXLUSIF EN DIMINUANT LE NOMBRE DE DE GO/CPU */
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

/*MET A JOUR LA STRUCTURE EXCLUSIF EN AJOUTANT LE NOMBRE DE DE GO/CPU */
void suppressionExclu(struct dataStruct* data, char * site, char * type, int value){
    int position = positionSite(data,site);

    if (strcmp(type,"GO")==0)
    {   
        data[position].go = data[position].go + value;
    }

    if (strcmp(type,"CPU")==0)
    {
        data[position].cpu = data[position].cpu + value;
    }
}

/*MET A JOUR LA STRUCTURE CLIENT*/
void actionExcluClient(struct clientStruct * client,int size, char * type, char * site, int value){
    strcpy(client->exclu[size].type,type);
    strcpy(client->exclu[size].site,site);
    client->exclu[size].quantity= value;    
}
void suppressionExcluClient(struct clientStruct * client,int position, int size){
    for (int i = position; i < size; ++i)
    {
        client->exclu[i] = client->exclu[i+1] ;
    }
    //la magie est que les tableau sont de taille 101 donc forcement il y aura un site ="" et quantity = 0
}

/*ADD DANS LA LISTE DES RESERVATION PARTAGER DANS LA STRUCTURE PARTAGER*/
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

void suppressionSharedType(struct dataStruct* data,char * site,char * type,int pos,int size){
    int position = positionSite(data,site);  
    if (strcmp(type,"GO")==0)
    {   
        suppressionShared(data[position].LSGo, pos, size);
    }

    if (strcmp(type,"CPU")==0)
    {
        suppressionShared(data[position].LSCpu, pos, size);
    }
}






void *Reservation(void *param) {
}



/*
* FONCTIONS SEND COTÉ CLIENT
*/


int isExluSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if ((strcmp(m,"e")==0 ||strcmp(m,"E")==0||strcmp(m,"EXCLUSIF")==0||strcmp(m,"exclusif")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"1");
        return 1;
    }

    if ((strcmp(m,"p")==0 ||strcmp(m,"P")==0||strcmp(m,"PARTAGE")==0||strcmp(m,"partage")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"0");
        return 1;
    }

    return -1;

}

int siteSend(char * msg, struct dataStruct *data){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if(positionSite(data,m)<0){ //la ville n'existe pas
        return -1;
    }

    strcpy(msg,m);
    return 1;
}

int typeSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if ((strcmp(m,"cpu")==0 ||strcmp(m,"CPU")==0||strcmp(m,"c")==0||strcmp(m,"C")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"CPU");
        return 1;
    }

    if ((strcmp(m,"go")==0 ||strcmp(m,"GO")==0||strcmp(m,"g")==0||strcmp(m,"G")==0)&&strlen(m)!=0)
    {
        strcpy(msg,"GO");
        return 1;
    }

    return -1;
}

int valueSend(char * msg){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';

    if (isInt(m)<1)
    {
        return -1;
    }

    strcpy(msg,m);
    return 1;
}


int SendClient(struct dataStruct* data, struct clientStruct * client,char * size){
    int nbrRequetes=isInt(size);
    char isExlu[20];
    char site[20];
    char type[20];
    char value[20];



    for (int i = 0; i <nbrRequetes; ++i)
    {   printf("Requête : %i\n",i+1 ); 
        //printf("exclu?,Site,type,value\n");

        printf(GRN"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        while(isExluSend(isExlu)<0){
            printf(YEL"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        }

        printf(GRN"Quel ville ?\n"RESET);
        while(siteSend(site,data)<0){
            printf(YEL"Quel ville ?\n"RESET);
        }

        printf(GRN"Cpu(c/C) ou go (g/G) ?\n"RESET);
        while(typeSend(type)<0){
            printf(YEL"Cpu(c/C) ou go (g/G) ?\n"RESET);
        }

        printf(GRN"Combien ?\n"RESET);
        while(valueSend(value)<0){
            printf(YEL"Combien ?\n"RESET);
        }

        if (sendAll(client->socket, isExlu) < 1)
            return -1;
        printf("EXclu\n");

        if (sendAll(client->socket, site) < 1)
            return -1;
        printf("SITE\n");

        if (sendAll(client->socket, type) < 1)
            return -1;
        printf("TYPE\n");

        if (sendAll(client->socket, value) < 1)
            return -1;
        printf("VALUE\n");         
    }
    
    return 1;
}

/*FONCTIONS RECV COTÉ SERVEUR*/
int recvServer(struct clientStruct  client,struct recvStruct * recvS ,int size){
    char m[20];
    char recv[4][20];
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (recvAll(client.socketServer,m)<1)
            {
                printf("Client %s : erreur au recv", client.name);
                return -1;
            }
            strcpy(recv[j],m);
                    
        }  

        strcpy(recvS[i].name,client.name);
        recvS[i].isExclu=isInt(recv[0]);
        strcpy(recvS[i].site,recv[1]); 
        strcpy(recvS[i].type,recv[2]);
        recvS[i].value= isInt(recv[3]);

        printf("Requête %i de %s : ",i+1,recvS[i].name);
        printf("Exlusif?:%i ",recvS[i].isExclu);
        printf("%s ",recvS[i].site);
        printf("%s ",recvS[i].type);
        printf("%i\n",recvS[i].value); 
    }
    return 1;
}