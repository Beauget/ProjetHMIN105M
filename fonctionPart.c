
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
/*SUPPRIME UNE VALEUR SELON SA POSITION                                                                                   INUTILISER
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
}*/
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


int recvAll2(int socket, char * buf, int len) { // recvAll function
  int remaining = len;
  key_t keysem = ftok("shmfile", 10);
  int idSem = semget(keysem, 7, IPC_CREAT | 0666);
  while (remaining) {
    int received = recv(socket, buf, remaining, 0);
    if (received <= 0) {
      semctl(idSem, 5, SETVAL, semctl(idSem, 5, GETVAL) - 1);
     // printf( "[Quit] Client %s:%d disconnected !\n", ip, port);
      exit(1);
    }
    buf += received;
    remaining -= received;
  }
  return 0;
}


int recvWithSize2(int sock, char * data) {
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
                if ( result == -1) { perror("send error"); exit(1);}    // not exit if errno == EAGAIN
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