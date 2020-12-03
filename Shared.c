

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

