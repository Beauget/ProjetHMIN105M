#include "fonctions.h"
//#include "fonctionPart.c"


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

/*PREMIERE APPARITION DU CLIENT DANS LA LISTE                                               Inutiliser*/
int lSharedPosition(struct LShared * l,char * name,int size){
    //int size =  lSharedSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].name,name)==0 && strlen(name)!=0)
            return i;
    }
    return -1;
}


/*SUPPRESSION SUIVANT UNE POSITION */
void suppressionShared(struct LShared * l, int position, int size){
    for (int i = position; i < size; ++i)
    {
        l[i]=l[i+1];
    }
}
/*SUPPRIME UNE VALEUR SELON SA POSITION                                                                                   INUTILISER*/
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
/*SUPPRESSION DE TOUTE LES APPARITIONS DU CLIENT */
void suppressionSharedClient(struct LShared * l, char* name, int size){
    for (int i = 0; i < size; ++i)
    {
        if(strcmp(name,l[i].name)==0 && strlen(name)!=0){
            suppressionShared(l,i, size);
            i = i -1;
            size = size-1;
        }
    }
}


/*ADD/MISE A JOUR DANS LA LISTE DES RESERVATION PARTAGER DANS LA STRUCTURE PARTAGER*/
void actionShared(struct dataStruct* data,int position,int pos, char * name, char * type, int value){  
    struct LShared l;
    strcpy(l.name , name);
    l.quantity = value;


    if (strcmp(type,"GO")==0)
    {   
        //int size = lSharedSize(data[position].LSGo);
        data[position].LSGo[pos]= l;
    }

    if (strcmp(type,"CPU")==0)
    {
        //int size = lSharedSize(data[position].LSCpu);
        data[position].LSCpu[pos] = l;
    }

}


void printShared(struct LShared * l, int size){
    for (int i = 0; i < size; ++i)
    {
        printf("position %i : %s : %i \n",i, l[i].name, l[i].quantity );
    }
}


int inShared(struct LShared * l, char *name, int size){

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].name,name)==0&&strlen(name)!=0)
        {
            return i;
        }
    }
    return -1;
}

int isInSharedType(struct dataStruct * data,int position, struct clientStruct * client ,char * type){

    if (strcmp(type,"GO")==0)
    {   int size = lSharedSize(data[position].LSGo);
        int pos = inShared(data[position].LSGo, client->name,size);
        if (pos<0)
            return size;
        else
            return pos;
        printShared(data[position].LSGo,size);
    }

    if (strcmp(type,"CPU")==0)
    {
        int size = lSharedSize(data[position].LSCpu);
        int pos =inShared(data[position].LSCpu, client->name,size);
        if (pos<0)
            return size;
        else
            return pos;        
        printShared(data[position].LSCpu,size);
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

/*RENVOIE LA POSITION DU SITE SI IL EST DANS LA LISTE EXCLUSIF -1 SINON                                                              inutilisé*/    
int lExcluPosition(struct LExclu * l,char * type, char * site){
    int size = lExcluSize(l);

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].type,type)==0 && strcmp(l[i].site,site)==0)
            return i;
    }
    return -1;
}

/*SUPRESSION D'UN  SITE EN PARTICULIER                                                                 INUTILISÉ*/
void suppressionExcluClient(struct clientStruct * client,int position, int size){
    for (int i = position; i < size; ++i)
    {
        client->exclu[i] = client->exclu[i+1] ;
    }
    //la magie est que les tableau sont de taille 101 donc forcement il y aura un site ="" et quantity = 0
}


/*MET A JOUR LA STRUCTURE CLIENT OU BIEN AJOUTE À LA FIN DE LA LISTE*/
void actionExcluClient(struct clientStruct * client,int size, char * type, char * site, int value){
    strcpy(client->exclu[size].type,type);
    strcpy(client->exclu[size].site,site);
    client->exclu[size].quantity= value;    
}


int returnMaxSite(struct dataStruct * data,int position,char * type){
    if (strcmp(type,"GO")==0)
    {   
        return data[position].maxGo;
    }

    if (strcmp(type,"CPU")==0)
    {
        return data[position].maxCpu;

    }
    return -1;    
}


int recvAll2(int socket, char * buf, int len) {
  int remaining = len;
  while (remaining) {
    int received = recv(socket, buf, remaining, 0);
    if (received <= 0) {
        return -1;
    }
    buf += received;
    remaining -= received;
  }
  return 0;
}


int recv2(int sock, char * data) {
  char sizeToRecv[4];
  if (recvAll2(sock, sizeToRecv, sizeof(sizeToRecv)) == 1) {
    return -1;
  };
  if (recvAll2(sock, data, *((int * ) sizeToRecv)) == 1) {
    return -1;
  };
  return *((int * ) sizeToRecv);
}

int sendall2(int sock, const char* data, int data_length){
        int bytessend = 0;
        while ( bytessend < data_length) {
                int result = send(sock, data + bytessend, data_length - bytessend, 0);
                if ( result == -1) 
                {
                 return -1;
                } 
                bytessend += result;
        }
        return bytessend;
}


int send2(int sock, const char* data, int data_length){
        char sizeToSend[4];
        *((int*) sizeToSend ) = data_length;
        sendall2(sock, sizeToSend,sizeof(sizeToSend) );
        sendall2(sock, data,data_length);
        return 1;
}



void intToChar(int value, char * msg){
    char * nb =  malloc(sizeof(char)* 20);
    sprintf(nb, "%d", value);
    strcpy(msg,nb);
    free(nb);
}

int sendStruct(struct dataStruct * data,int size,int socket){
    for (int i = 0; i < size; ++i)
    {
        char * site =  malloc(sizeof(char)* 20);
        char * go =  malloc(sizeof(char)* 20);
        char * goMax =  malloc(sizeof(char)* 20);
        char * cpu =  malloc(sizeof(char)* 20);
        char * cpuMax =  malloc(sizeof(char)* 20);
        //char * msg =  malloc(sizeof(char)* 200);  
        char msg[200];
        strcpy(msg,"");  

        intToChar(data[i].go,go);
        intToChar(data[i].maxGo,goMax);
        intToChar(data[i].cpu,cpu);
        intToChar(data[i].maxCpu,cpuMax);
        strcpy(site,data[i].site);

        strcat(msg,"Site : ");
        strcat(msg,site); 
        strcat(msg," ,GO : "); 
        strcat(msg,go);
        strcat(msg,"/");  
        strcat(msg,goMax);
        strcat(msg,"/"); 
        strcat(msg," ,CPU : "); 
        strcat(msg,cpu);
        strcat(msg,"/");  
        strcat(msg,cpuMax);

        if(send2(socket,msg, sizeof(msg))<0){
            return -1;
        }
        free(go);
        free(goMax);
        free(cpu);
        free(cpuMax);
        strcpy(msg,"");
    }

    return 1;
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
           //data[i].id = i;
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
printf(RED"|||| MERCI DE RESPECTER LES MAJUSCULES ET LES MINUSCULES DANS LES DEMANDES|||| \n" RESET);
printf(RED"|||| POUR TOUTE DEMANDE VEUILLEZ LIRE NOTRE DOCUMENTATION |||| \n" RESET);

}

/*
void affichageEtatMaj(struct dataStruct* data) {
        printf(YEL"                 --- MISE A JOURS SYSTEME --- \n \n");
        printf(RED" Une réservation a eu lieu, l'état du système a changer \n \n");
    for(int i = 0; i < taille;i++) {
        printf(YEL"Site : %s, GO DISPONIBLE : %i/%i, CPU DISPONIBLE : %i/%i \n",data[i].site,data[i].go,data[i].maxGo,data[i].cpu,data[i].maxCpu);
    }
}
*/

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
            return -1;
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
            return -1;
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
    int size =8;
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
        if( strcmp(data[i].site,site)==0&&strlen(site)!=0){
            return i;
        }
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
void suppressionExclu(struct dataStruct* data, int position, char * type, int value){
    //int position = positionSite(data,site);

    if (strcmp(type,"GO")==0)
    {   
        data[position].go = data[position].go + value;
    }

    if (strcmp(type,"CPU")==0)
    {
        data[position].cpu = data[position].cpu + value;
    }
}




void suppressionSharedClientAll(struct dataStruct* data,char * name){
    for (int i = 0; i < taille; ++i)
    {   int maxGO = maxLSharedType(data , i ,"GO");
        int maxCpu = maxLSharedType(data , i ,"CPU");
        int sizeGo = lSharedSize(data[i].LSGo);
        int sizeCpu = lSharedSize(data[i].LSCpu);
        suppressionSharedClient(data[i].LSGo,name,sizeGo);
        suppressionSharedClient(data[i].LSCpu,name,sizeCpu);
        int nMaxGO = maxLSharedType(data , i ,"GO");
        int nMaxCpu = maxLSharedType(data , i ,"CPU");

        if (maxGO>nMaxGO){
            suppressionExclu(data,i,"GO",maxGO-nMaxGO); //réaloue les go 
        }

        if (maxCpu>nMaxCpu){
            suppressionExclu(data,i,"CPU",maxCpu-nMaxCpu);
            //printf("%s\n", );
        }
    }
}

void suppressionExcluClientAll(struct dataStruct* data,struct clientStruct * client){
    //int position = positionSite(data,client->exclu->site);
    int size = lExcluSize(client->exclu);

    for (int i = 0; i < size; ++i)
    {   int position = positionSite(data,client->exclu[i].site);
        suppressionExclu(data,position,client->exclu[i].type,client->exclu[i].quantity);
    }
}


/*
* FONCTIONS SEND COTÉ CLIENT
*/


int isExcluSend(char * msg){
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

int siteSend(char * msg/*, struct dataStruct *data*/){
    char m[20];
    fgets(m, sizeof(m), stdin);
    m[strlen(m) - 1] = '\0';
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


int SendClient(/*struct dataStruct* data, */struct gestionSendUpdate * client,char * size){
    int nbrRequetes=isInt(size);

    for (int i = 0; i <nbrRequetes; ++i)
    {   printf("Requête : %i\n",i+1 ); 
        //printf("exclu?,Site,type,value\n");
        char * isExclu = malloc (20 * sizeof (char));
        char * site =  malloc (20 * sizeof (char));
        char * type  =  malloc (20 * sizeof (char));
        char * value=  malloc (20 * sizeof (char));
        strcpy(isExclu,"");
        strcpy(site,"");
        strcpy(type,"");
        strcpy(value,"");


        printf(GRN"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        while(isExcluSend(isExclu)<0){
            printf(YEL"Exclusif(e/E) ou partagé(p/P)\n"RESET);
        }

        printf(GRN"Quel site ? (Veuillez respecter les majuscles et les miniscules) \n"RESET);
        while(siteSend(site/*,data*/)<0){
            printf(YEL"Quel site ?\n"RESET);
        }

        printf(GRN"Cpu(c/C) ou go (g/G) ?\n"RESET);
        while(typeSend(type)<0){
            printf(YEL"Cpu(c/C) ou go (g/G) ?\n"RESET);
        }

        printf(GRN"Combien ?\n"RESET);
        while(valueSend(value)<0){
            printf(YEL"Combien ?\n"RESET);
        }

        if (send2(client->socket, isExclu,sizeof(isExclu)) < 1)
            return -1;

        if (send2(client->socket, site,sizeof(site)) < 1)
            return -1;

        if (send2(client->socket, type,sizeof(type)) < 1)
            return -1;

        if (send2(client->socket, value,sizeof(value)) < 1)
            return -1;

        free(isExclu);
        free(site);
        free(type); 
        free(value);      
    }
    
    return 1;
}


/*FONCTIONS RECV COTÉ SERVEUR*/
int recvServer(struct clientStruct  client,struct recvStruct * recvS ,int size){
    //strcpy(m,"");
    char recv[4][20];

    strcpy(recv[0],"");
    strcpy(recv[1],"");
    strcpy(recv[2],"");
    strcpy(recv[3],"");

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {   
            char * m = malloc (20 * sizeof (char));
            strcpy(m,"");
            if (recvAll(client.socketServer,m)<1)
            {
                printf("Client %s : erreur au recv", client.name);
                return -1;
            }
            strcpy(recv[j],m);

            free(m);            
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

        strcpy(recv[0],"");
        strcpy(recv[1],"");
        strcpy(recv[2],"");
        strcpy(recv[3],"");


    }
    return 1;
}

int restantType(struct dataStruct * data,int position,char * type, int value){
    if (strcmp(type,"GO")==0)
    {   
        return(data[position].go-value);
    }

    if (strcmp(type,"CPU")==0)
    {
        return(data[position].cpu-value);
    }
}


void actionExcluAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS){
    int size = lExcluSize(client->exclu);

    actionExclu(data, recvS.site, recvS.type, recvS.value);
    actionExcluClient(client,size, recvS.type, recvS.site,recvS.value);
}



int getValue(struct dataStruct *data, int position,char * type, int posList){
    if (strcmp(type,"GO")==0)
    {   
        return(data[position].LSGo[posList].quantity);
    }

    if (strcmp(type,"CPU")==0)
    {
        return(data[position].LSCpu[posList].quantity);
    }
    return -1;
}

void actionSharedAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct recvS){
    int position= positionSite(data,recvS.site);

    int oldMax = maxLSharedType(data,position,recvS.type);
    //printf("%i\n", oldMax);

    int positionToAdd= isInSharedType(data,position,client,recvS.type);

    int value = recvS.value + getValue(data, position,recvS.type,positionToAdd);
    actionShared(data,position,positionToAdd,client->name, recvS.type, value);
    int newMax = maxLSharedType(data,position,recvS.type);

    if (oldMax<newMax)
    {
        actionExclu(data, recvS.site, recvS.type, (newMax-oldMax));
    }

}




void actionAll(struct dataStruct * data, struct clientStruct * client,struct recvStruct * recvS,int size){
    for (int i = 0; i < size; ++i)
    {
        if (recvS[i].isExclu==1)
        {
            actionExcluAll(data,client,recvS[i]);
        }

        if (recvS[i].isExclu==0){
            actionSharedAll(data,client,recvS[i]);
        }
    }
}






void initMutex(pthread_mutex_t * verrou) {
    pthread_mutex_init(verrou,NULL);
}


int P(int semid, int semnum, int n) {   //-1
    struct sembuf buf = {semnum, -n, 0};
    return semop(semid, &buf, 1);
}


// incrémentation du sémpahore
int V(int semid, int semnum, int n) { //+1
    struct sembuf buf = {semnum, n, 0};
    return semop(semid, &buf, 1);
}


// attente du 0 dans u sémaphore
int Z(int semid, int semnum) { //0
    struct sembuf buf = {semnum, 0, 0};
    return semop(semid, &buf, 1);
}




void * UpdateClient(void *param) {

    while(1){
        struct gestionSendUpdate * p = (struct gestionSendUpdate*) param;
        recvAll(p->socket, p->msg);

        if (strcmp(p->msg,"Requête(s) effectué(s).")==0)
            printf(GRN"%s \n"RESET, p->msg);
        else if (strcmp(p->msg,"Requête(s) annulée(s) : improbable(s)")==0)
            printf(RED"%s \n"RESET, p->msg);
        else if (strcmp(p->msg,"Requête(s) annulée(s) : trop de tentatives.")==0)
            printf(YEL"%s \n"RESET, p->msg);
        else
            printf(GRN"%s"RESET, p->msg);

        printf(RESET"\n"RESET);

        pthread_cond_broadcast(p->cond);

    }
}

void * UpdateServer(void *param) {
    struct SendUpdate * p = (struct SendUpdate*) param;

    key_t keySem = ftok("sharedSem.txt",10);
    int idSem = semget(keySem,taille,IPC_CREAT | 0666);

    while(1){
        if ( semctl(idSem, 1, GETVAL) > 0 ) {

            send2(p->socket, "La base de données a été mise à jour", sizeof("La base de données a été mise à jour"));
            sendStruct(p->etat,p->size,p->socket);
            P(idSem,1,1);
            Z(idSem,1);
        }

    }
}


void printSharedData(struct dataStruct * data ){
    for (int i = 0; i < taille; ++i)
    {   printf(GRN"Données partagé dans : %s\n"RESET,data[i].site );
        int sizeGo= lSharedSize(data[i].LSGo);
        int sizeCpu= lSharedSize(data[i].LSCpu);
        printShared(data[i].LSGo, sizeGo);
        printShared(data[i].LSCpu,sizeCpu);
    }    
}



int inExclu(struct LExclu * l, char * site, int size){

    for (int i = 0; i < size; ++i)
    {
        if (strcmp(l[i].site,site)==0&&strlen(site)!=0)
            return i;      
    }
    return size;
}


int isPExclu(struct dataStruct * data,int position, struct LExclu * l,char * type, char * site,int value, char * name){
    int size = lExcluSize(l);
    if (size== -1)
        size = 100; //la liste est pleine , on voit si le site est dans la liste

    int pos = inExclu(l,site,size);
    int maxSite = returnMaxSite(data,position,type);

        if(pos==100){
            return -1;
            printf("Le tableau de %s est plein. Réservation impossible !\n",name );
        }

        if ((l[pos].quantity+value)>maxSite){
            printf("%s tente de réserver au total plus que le maximum. Réservation impossible !\n",name );
            return -1;

        }

        if (strcmp(type,"GO")==0)
            return (data[position].go-value>=0);

        if (strcmp(type,"CPU")==0)
            return (data[position].cpu-value>=0);
}

int isPSharedType(struct LShared * l,char * name){
    int size = lSharedSize(l);
    if (size == -1) //le tableau est plein
        size = 100;

    int position = inShared(l,name,size);

    if(position<0 && size==100) //n'est pas dans liste et plus de place
        return -1;
    if(position<0)//n'est pas dans la liste donc envoie la prochaine position
        return size;
    else
        return position;// est dans la liste et voila sa position
}

int isPShared(struct dataStruct * data,int position,char * type ,char * name,int value){
    if (strcmp(type,"GO")==0)
    {   
        int pos = isPSharedType(data[position].LSGo,name);
        if (pos == -1)
            return -1;

        int total = data[position].LSGo[pos].quantity + value; //0+value ou x+value

        if (maxLShared(data[position].LSGo)<total){//vérifie si il change le max
            return ((data[position].go - value)>=0);//0 = pas possible, positif = possible

        }
        else
            return 1; //n'affecte pas le max
    }

    if (strcmp(type,"CPU")==0)
    {
        int pos = isPSharedType(data[position].LSCpu,name);
        if (pos == -1)
            return -1;
        int total = data[position].LSCpu[pos].quantity + value; //0+value ou x+value
        
        if (maxLShared(data[position].LSCpu)<total){
            return ((data[position].cpu -value)>=0);
        }
        else
            return 1; //n'affecte pas le max
        }
    return -1;
}


int isPossible(struct dataStruct * data, struct clientStruct * client, struct recvStruct * recvS, int size){
    int probable = 0; 
    int val=1;

    for (int i = 0; i < size; ++i)
    {   
        int position = positionSite(data,recvS[i].site);
        char * type = recvS[i].type;
        char * name = recvS[i].name;
        int isExclu = recvS[i].isExclu;
        int value = recvS[i].value;

        if (position > - 1)
        {
            if ((returnMaxSite(data,position,type)-value)<0){
                    printf("%s tente de réserver plus que le total. Réservation annulé !\n",name );
                    return -1;
            }
    
            if (isExclu==0){
                val = isPShared(data,position,type,name,value);
            }

            if (isExclu==1){
                val = isPExclu(data,position,client->exclu,recvS[i].type,recvS[i].site,value, client->name);
                if (val == -1)
                    return -1;//réservation impossible !
            }
            if (val==0){
                probable = probable +1;
            }
        }
        else
            return -1;
    }
    return (1>probable); //0 si partiellement possible 
}
