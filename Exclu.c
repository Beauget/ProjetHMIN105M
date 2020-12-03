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


