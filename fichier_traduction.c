#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "header_traduction.h"
#define MAX 1024
#define MaxInstruction 16384 //65536/4

void affiche_liste_etiquette(t_liste_etiquette *L){
    t_etiquette *p=L->tete; int i=1;
    while (p){
        printf("Etiquette numero %d, nom : %s, adresse: %d\n",i,p->nom,p->adresse);
        p=p->suiv;
        i++;
    }
}
int supprime_occurence(char *str, char c){  // supprime les occurences de char c, et renvoit un booleen permettant de savoir si la liste ne contenait que des char c
    int i,j=0;
    for (i=0;i<strlen(str);i++){
        if (str[i]!=c){
            str[j]=str[i];
            j++;
        }}
        str[j]='\0';
    return (j==0);
}
void supprime_occurence_debut(char *str, char c){  
    char * copy=str;
    while (*(str)==c){
             *str=':';
             str++;
    }
    supprime_occurence(copy,':');
}

int contient_espace_tabulation_milieu(char *str) {
    for (int i=0; str[i]; i++) {
        if (str[i]==' ' || str[i]==9) {
            if (i>0 && str[i - 1]!=' ' && str[i - 1]!= 9) {    // vérifier s'il y a un caractère différent avant l'espace
                for (int j=i+1;str[j];j++) {   
                    if (str[j]!=' ' && str[j]!=9 ) {
                        return 1; // il y a aussi un caractère différent après l'espace
                    }
                }
            }
        }
    }
    return 0;
}

int nb_occurences(t_etiquette *q,t_liste_etiquette*L){
    if (!L || !q) return 0; 
    int cpt=0;
    t_etiquette *p=NULL; 
    for (p=L->tete;p;p=p->suiv) if (!strcmp(p->nom,q->nom)) cpt++;
    return cpt;
}



int contient_doublon(t_liste_etiquette *L){
    t_etiquette *p;
    for (p=L->tete;p;p=p->suiv){
        if (nb_occurences(p,L)>1){
            printf("Errreur a la ligne %d, redefinition d'etiquette (\"%s\").\n",((p->adresse)/4) +1,p->nom);
            return 1;
        }
    }
    return 0;
}



void ajout_tete_liste_etiquette(t_liste_etiquette *L,t_etiquette *p){
    if (!L || !p) return ;
    p->suiv=L->tete;
    L->tete=p;
   // L->nb_etiquette++;
}

int des_chiffres_et_des_lettres(const char *str){
    for (int i=0;i<strlen(str);i++)
        if ((!(str[i]>='a' && str[i]<='z')) && (!(str[i]>='A' && str[i]<='Z')) && (!(str[i]>='0' && str[i]<='9'))) return 0;
    return 1;
}

void libere_liste_etiquette(t_liste_etiquette *L){
    if(!L) return;
    t_etiquette *p=L->tete;
    t_etiquette *tmp=NULL;
    while (p){
        tmp=p->suiv;
        free(p->nom);
        free(p);
        p=tmp;}
    free(L);
}

int verif_tabulation(char * string,int ligne){
    char * copy=string; 
    int h=0; 
    while (*copy) {  
        if ((*copy!=' ') && (*copy!=9)) h++; 
        copy++;
    }
    if (!h) {
        printf("Erreur à la ligne %d, l'etiquette ne contient que des tabulations et/ou des espaces.\n",ligne +1);
        return 1;
    }
    else if (string[0]==9){
        printf("Erreur à la ligne %d, l'etiquette commence par une ou plusieurs tabulations.\n",ligne +1);
        return 1;
    }
    else if (contient_espace_tabulation_milieu(string)){
        printf("Erreur à la ligne %d, le nom de l'etiquette contient des tabulations et/ou espaces entre ses caracteres.\n",ligne +1);
        return 1;
    }
    else if (string[strlen(string)-1]==9){
        printf("Erreur à la ligne %d, le nom de l’etiquette finit par une ou plusieurs tabulations, et n'est donc pas immediatement suivi d’un ':' lors de sa définition.\n",ligne +1);
        return 1;
    }
    return 0;
}
t_etiquette * initialise_etiquette(void){
    t_etiquette *p=malloc(sizeof(t_etiquette));
    if (!p) {printf("malloc à échoué sur la creation de t_etiquette.\n");
        free(p);
        return NULL;
        }
    p->adresse=-1;p->nom=NULL;p->suiv=NULL;
    return p;
}


t_liste_etiquette* intialise_liste_etiquette(FILE *f){     // creation de t_liste_etiquette sans erreur de syntaxe
    t_liste_etiquette* L=malloc(sizeof(t_liste_etiquette)); 
    if (!L) {
        printf("malloc à échoué sur la creation de t_liste_etiquette.\n");
        free(L);
        fclose(f);
        exit(EXIT_FAILURE);
    }
   // L->nb_etiquette=0;
    L->tete=NULL;
    char string[MAX];
    int i=0,j,error=0;
    char * name=NULL;
    t_etiquette *p=NULL;
    while (fgets(string,MAX-1,f)){
        name = strstr(string,":"); // ou  strchr
        if (name){
            if (name!=strrchr(string,':')){  // plusieurs ':' 
                    printf("Erreur à la ligne %d, était attendu au plus 1 caractere ':' dans la ligne, plusieurs ont été données.\n",i+1);
                    libere_liste_etiquette(L);
                    fclose(f);
                    exit(EXIT_FAILURE);
            }
            else { j=name-string; // j represente le nombre de caractere de l'etiquette (sans le ':')
                    p=initialise_etiquette(); if (!p) {libere_liste_etiquette(L); fclose(f); exit(EXIT_FAILURE);}
                    ajout_tete_liste_etiquette(L,p);   // on inverse ainsi l'odre des etiquettes, aucune importance
                    p->adresse=i*4; 
                    p->nom=malloc(sizeof(char)*(j+1));
                    for (j=0;j<name-string;j++) p->nom[j]=string[j];
                    p->nom[j]='\0'; 
                    int h=(p->nom[j-1]==' ')? 1:0;
                    int k=(p->nom[0]==' ')? 1:0;
                    if (!j) {
                     printf("Erreur à la ligne %d, l'etiquette est vide (aucun caractere).\n",i+1); 
                     error ++;
                    }
                    else if (verif_tabulation(p->nom,i)) error++;
                    //else if (contient_espace_tabulation_milieu(p->nom)){
                      //  printf("Erreur à la ligne %d, le nom de l'etiquette contient des espaces entre ses caracteres.\n",i+1); error++;
                    //}
                    else if (supprime_occurence(p->nom,' ')){     // cette ligne est à posteriori de trop si l'on gere les tabulations, on l'a garde quand meme
                    printf("Erreur à la ligne %d, l'etiquette ne contient que des espaces.\n",i+1); error++;
                    }
                    else if(k){
                         printf("Erreur à la ligne %d, l'etiquette commence par un ou plusieurs espaces.\n",i+1); error++;
                    }
                    else if (h) {
                    supprime_occurence(p->nom,9);  // permet un meilleur affichage des erreurs;
                    printf("Erreur à la ligne %d, le nom de l’etiquette (\"%s\") n'est pas immediatement suivi d’un ':' lors de sa définition.\n",i+1,p->nom); error++;
                    }
                    else if (!des_chiffres_et_des_lettres(p->nom)) {
                    printf("Erreur à la ligne %d, le nom de l’etiquette (\"%s\") n'est pas uniquement composee de lettre minuscules ou majuscules, et de chiffres.\n",i+1,p->nom); error++;
                    }
                    if (contient_doublon(L)) error++;
                    if (error){
                       libere_liste_etiquette(L);
                       fclose(f);
                       exit(EXIT_FAILURE);
                       return NULL;
                    }
            }
            
        }     i++;
    } 
    fseek(f, 0, SEEK_SET); //revenir au debut
return L;
}

                                      /*ICI COMMENCE LES INSTRUCTIONS*/    
/* ------------------------------------------------------------------------------------------------------------------------------------------*/


int nb_instruction(FILE *f){
    int i=0;
    char nom[MAX];
    while (fgets(nom,MAX-1,f)) i++;
    fseek(f, 0, SEEK_SET);  // revenir au debut 
    //if (i>MaxInstruction) {
      //  printf("Erreur, le code source ecrit en langage assembleur contient trop d'instruction (%d) et depasse la valeur maximale atteignable (%d), il ne pourra pas donc pas être chargé en mémoire principale.\n",i,MaxInstruction);
        //return -1;
    //}
    return i;
}

int est_mnemonique(char * ope, char ** liste_mnemonique){
    if ((ope[0]=='i') || ((ope[0]=='o') && (ope[1]=='r') )) ope[2]='\0';
    for (int i=0;i<32;i++){
        if (!strcmp(liste_mnemonique[i],ope)) return i;
    }
    return -1;
}

void enlever_mnemonique(char *str,int d){  // on a pas à ce soucier du cas où i>len(str) puisqu'on appelle la fonction dans un cas precis
        int i=0;
        while (str[i+d]){
            str[i]=str[i+d];
            i++;
        }str[i]='\0';
}

int nb_operande(char * str){   // compte le nombre de virgules
  int i=0,j=-1; 
  while (str[++j]) if (str[j]==',') i++;
  return i;
}

int recupere_champ(int ligne,int nb_operande,char *str, int *dest, int *src1,int *src2,int Imm,int hexa, int store){
    char * tempo=strstr(str,"h"); 
    int nb_symbole_hexa=0; 
    if (tempo) while (*(++tempo)) nb_symbole_hexa++; 
    long long DEST=0,SRC1=0,SRC2=0;
    if (nb_operande==3){
       if (!Imm) sscanf(str,"r%lld,r%lld,r%lld",&DEST,&SRC1,&SRC2);
       else if (Imm && hexa) sscanf(str,"r%lld,r%lld,#h%llx",&DEST,&SRC1,&SRC2);
       else sscanf(str,"r%lld,r%lld,#%lld",&DEST,&SRC1,&SRC2);
    }
    else if (nb_operande==2){
       if (!Imm) sscanf(str,"r%lld,(r%lld)r%lld",&DEST,&SRC1,&SRC2);
       else if (Imm && hexa) sscanf(str,"r%lld,(r%lld)#h%llx",&DEST,&SRC1,&SRC2);
       else sscanf(str,"r%lld,(r%lld)#%lld",&DEST,&SRC1,&SRC2);
    }
    if (DEST<0 || DEST>31) {
        if (store) printf("Erreur à la ligne %d, le registre src1 est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,DEST);
        else printf("Erreur à la ligne %d, le registre destination est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,DEST);
        return 1;
    }  
    else if (SRC1<0 || SRC1>31) {
        if (store) printf("Erreur à la ligne %d, le registre destination est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,SRC1);
        else printf("Erreur à la ligne %d, le registre source1 est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,SRC1);
        return 1;
    }
    else if (!Imm && (SRC2<0 || SRC2>31) ){
        printf("Erreur à la ligne %d, le registre source2 est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,SRC2);
        return 1;
    }
    else if (!Imm && (SRC2>=0 && SRC2<=31)) *src2=SRC2;
    short temp=SRC2; *src2=temp;
    if ((SRC2>SHRT_MAX || SRC2<SHRT_MIN ) && (!(hexa && nb_symbole_hexa<=4))) printf("Warning à la ligne %d, la valeur immediate indiqué depasse la plage de representation d'un nombre binaire sur 16 bits: conversion implicite change la valeur de %lld à %d.\n",ligne+1,SRC2,temp);     
    *dest=DEST;*src1=SRC1;
    return 0;
}

int verif_3_operandes(char * str, int ligne,int * dest,int * src1,int * imm, int * src2){ 
    int i=nb_operande(str);
    if (i!=2) {
        if (i==0) printf("Erreur à la ligne %d, était attendu exactement 3 operandes, un seul a été donné.\n",ligne+1);
        else printf("Erreur à la ligne %d, était attendu exactement 3 operandes, %d ont été donnés.\n",ligne+1,i+1);
        return 1;
    }
    char * virgule1=strstr(str,",");char * virgule2=strrchr(str,','); // ou  strchr pour virgule1
    int Imm= (virgule2[1]=='r')? 0: ((virgule2[1]=='#')? 1:-1); *imm=Imm;
    int hexa=(virgule2[2]=='h')? 1: 0;
    if (str[0]!='r') { 
        printf("Erreur à la ligne %d, le nom du premier registre est invalide (ne commmence pas par un 'r').\n",ligne+1);
        return 1;
    }
    else if (virgule1[1]!='r'){
        printf("Erreur à la ligne %d, le nom du deuxieme registre est invalide (ne commmence pas par un 'r').\n",ligne+1);
        return 1;
    }
    else if (Imm==-1){
        printf("Erreur à la ligne %d, le nom du dernier operande est invalide (ni une valeur immediate, ni un registre valide).\n",ligne+1);
        return 1;
    }
    char *copy=str;
    while (*(++str)){
        if ((!(str==copy+1 && *str=='-')) && (str<virgule1) && (!((str[0]>='0') && (str[0]<='9')))){
            printf("Erreur à la ligne %d, le nom du premier registre est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if ((!(str==virgule1+2 && *str=='-')) && ((str!=(virgule1+1)) && (str<virgule2) && (str>virgule1)) && (!((str[0]>='0') && (str[0]<='9')))){
            printf("Erreur à la ligne %d, le nom du deuxieme registre est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if ( (!(str==virgule2+2 && *str=='-')) &&  ((str!=(virgule2+1)) && (str>virgule2)) && (Imm==0) && (!((str[0]>='0') && (str[0]<='9')))){
            printf("Erreur à la ligne %d, le nom du dernier operande est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if (((str!=(virgule2+1)) && (str>virgule2)) && ((Imm==1) && (!hexa)) && (!(str==(virgule2+2) && (*str=='-')) ) && (!((str[0]>='0') && (str[0]<='9')))){
            printf("Erreur à la ligne %d, le nom du dernier operande est invalide (ce qui suit le '#' n'est pas un nombre entier exprime en decimal).\n",ligne+1);
            return 1;
        }
        else if (((str!=(virgule2+1)) && (str!=(virgule2+2)) && (str>virgule2)) && ((Imm==1) && (hexa)) && (!(((str[0]>='0') && (str[0]<='9')) || ((str[0]>='a') && (str[0]<='f')) || ((str[0]>='A') && (str[0]<='F'))))){
            printf("Erreur à la ligne %d, le nom du dernier operande est invalide (ce qui suit le '#h' n'est pas un nombre entier exprime en hexadecimal).\n",ligne+1);
            return 1;
        }
    }
    if ((virgule1[-1]=='r') || ((virgule1[-2]=='r') && (virgule1[-1]=='-') )){
        printf("Erreur à la ligne %d, le nom du premier registre est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
        return 1;
    }
    else if ((virgule2[-1]=='r') || ((virgule2[-2]=='r') && (virgule2[-1]=='-') )){
        printf("Erreur à la ligne %d, le nom du deuxieme registre est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
        return 1;
    }
    else if ((virgule2[1]=='r' && virgule2[2]=='\0') || ((virgule2[1]=='r') && (virgule2[2]=='-') && (virgule2[3]=='\0') )){
        printf("Erreur à la ligne %d, le nom du troisieme registre est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
        return 1;
    }
    else if ((virgule2[2]=='h' && virgule2[3]=='\0')){
      printf("Erreur à la ligne %d, le nom du dernier operande est invalide (aucun nombre hexadecimale ne suit le '#h').\n",ligne+1);
        return 1;
    }
    else if ((virgule2[2]=='\0') || (virgule2[2]=='-'  && virgule2[3]=='\0')){
        printf("Erreur à la ligne %d, le nom du dernier operande est invalide (aucun nombre decimale ne suit le '#').\n",ligne+1);
        return 1;
    }
    if (recupere_champ(ligne,3,copy,dest,src1,src2,Imm,hexa,0)) return 1;
    char * test=strstr(copy,"r-0");
    if (test) printf("Remarque: à la ligne %d, on suppose que r-0 correspond r0.\n",ligne+1);
    return 0;
    }

void permutation_autour_virgule(char * str, char * virgule){
    char str1[256]; int lenstr1=0;
    char str2[256]; int lenstr2=0;
    char *copy2=str;
    while (*str){
        if (str<virgule){
            str1[lenstr1]=*str;
            lenstr1++;
        }
        else if (str>virgule){
            str2[lenstr2]=*str;
            lenstr2++;
        }
        str++;
    } str1[lenstr1]='\0';str2[lenstr2]='\0';
    char copy[MAX]="";
    strcpy(copy,str2);
    copy[lenstr2]=',';
    for (int i=lenstr2+1;i<lenstr1 + lenstr2 +1;i++){
        copy[i]=str1[i-(lenstr2+1)];
    }
    copy[lenstr1+lenstr2+1]='\0';
    str=copy2;
    strcpy(str,copy);   // str et copy ont exactement la même taille
}




int verif_2_operandes_load(char * str, int ligne,int * dest,int * src1,int * imm, int * src2, int store){
    int i=nb_operande(str); 
    if (i!=1) {
        if (i==0) printf("Erreur à la ligne %d, était attendu exactement 2 operandes, un seul a été donné.\n",ligne+1);
        else printf("Erreur à la ligne %d, était attendu exactement 2 operandes, %d ont été donnés.\n",ligne+1,i+1);
        return 1;
    }
    char *virgule=strstr(str,",");char *parenthese1=strstr(str,"(");char *parenthese2=strstr(str,")");
    if (!parenthese1) {
        if (store) printf("Erreur à la ligne %d, manquement de la parenthese ouvrante '(' pour le champ destination.\n",ligne+1);
        else printf("Erreur à la ligne %d, manquement de la parenthese ouvrante '(' pour le champ src1.\n",ligne+1);
        return 1;
    }
    else if (!parenthese2){
        if (store) printf("Erreur à la ligne %d, manquement de la parenthese fermante ')' pour le champ destination.\n",ligne+1);
        else printf("Erreur à la ligne %d, manquement de la parenthese fermante ')' pour le champ src1.\n",ligne+1);
        return 1;
    }
    /*else if (parenthese1>parenthese2){
        printf("Erreur à la ligne %d, ordre d'appartition de '(' et ')' inversé.\n",ligne+1);
        return 1;
    }*/
    int Imm= (parenthese2[1]=='r')? 0: ((parenthese2[1]=='#')? 1:-1); *imm=Imm;
    int hexa=(parenthese2[2]=='h')? 1: 0;
    if (str[0]!='r'){
        if (store) printf("Erreur à la ligne %d, le nom du registre src1 est invalide (ne commmence pas par un 'r').\n",ligne+1);
        else printf("Erreur à la ligne %d, le nom du registre destination est invalide (ne commmence pas par un 'r').\n",ligne+1);
        return 1;
    }
    else if(virgule>parenthese1){
        if (store) printf("Erreur à la ligne %d, le nom du champ src1 est invalide, il contient une parenthese '('.\n",ligne+1);
        else printf("Erreur à la ligne %d, le nom du champ destination est invalide, il contient une parenthese '('.\n",ligne+1);
        return 1;
    }
    else if ((virgule+1)!=parenthese1){
        if (store) printf("Erreur à la ligne %d, le registre destination est invalide, il ne commmence pas par un '('.\n",ligne+1);
        else printf("Erreur à la ligne %d, le registre source1 est invalide, il ne commmence pas par un '('.\n",ligne+1);
        return 1;
    }
    else if(*(parenthese1+1)!='r'){
        if (store) printf("Erreur à la ligne %d, le nom du champ destination est invalide, il ne commmence pas par un '(r'.\n",ligne+1);
        else printf("Erreur à la ligne %d, le nom du champ source1 est invalide, il ne commmence pas par un '(r'.\n",ligne+1);
        return 1;
    }
    else if (Imm==-1){
        printf("Erreur à la ligne %d, le champ source2 est invalide (ni une valeur immediate indiquee par #, ni un registre valide).\n",ligne+1);
        return 1;
    }
    char *copy=str;
    while (*(++str)){
        if ( (!((str==copy+1) && *str=='-')) && (str<virgule) && (!((str[0]>='0') && (str[0]<='9')))){
            if (store) printf("Erreur à la ligne %d, le nom du registre src1 est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            else printf("Erreur à la ligne %d, le nom du registre destination est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if ( (!(str==parenthese1+2 && *str=='-')) &&((str<parenthese2) && (str>(parenthese1+1))) && (!((str[0]>='0') && (str[0]<='9')))){
            if (store) printf("Erreur à la ligne %d, le registre destination est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            else printf("Erreur à la ligne %d, le registre source1 est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if ( (!(str==parenthese2+2 && *str=='-')) && ((str>(parenthese2+1))) && (Imm==0) && (!((str[0]>='0') && (str[0]<='9')))){
            printf("Erreur à la ligne %d, le registre source2 est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
            return 1;
        }
        else if (  ((str>(parenthese2+1))) && ((Imm==1) && (!hexa)) && (!(str==(parenthese2+2) && (*str=='-')) ) && (!((str[0]>='0') && (str[0]<='9')))  ){
            printf("Erreur à la ligne %d, le champ source2 est invalide (ce qui suit le '#' n'est pas un nombre entier exprime en decimal).\n",ligne+1);
            return 1;
        }
        else if ((str>(parenthese2+2)) && ((Imm==1) && (hexa)) && (!(((str[0]>='0') && (str[0]<='9')) || ((str[0]>='a') && (str[0]<='f')) || ((str[0]>='A') && (str[0]<='F'))))){
            printf("Erreur à la ligne %d, le champ source2 est invalide (ce qui suit le '#h' n'est pas un nombre entier exprime en hexadecimal).\n",ligne+1);
            return 1;
        }
    }
    if ((virgule[-1]=='r') || ((virgule[-2]=='r') && (virgule[-1]=='-') )){
    if (store) printf("Erreur à la ligne %d, le nom du registre src1 est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
    else printf("Erreur à la ligne %d, le nom du registre destination est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
    return 1;
    }
    else if (((parenthese1+2)==parenthese2) || (*(parenthese1+2)=='-' && (parenthese1+3)==parenthese2) ){
        if (store) printf("Erreur à la ligne %d, le registre destination est invalide (aucun nombre ne suit le 'r')).\n",ligne+1);
        else printf("Erreur à la ligne %d, le registre source1 est invalide (aucun nombre ne suit le 'r')).\n",ligne+1);
        return 1;
    }
    else if ((parenthese2[1]=='r' && parenthese2[2]=='\0') || ((parenthese2[1]=='r') && (parenthese2[2]=='-') && (parenthese2[3]=='\0') )){
        printf("Erreur à la ligne %d, le registre source2 est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
        return 1;
    }
    else if ((parenthese2[2]=='h' && parenthese2[3]=='\0')){
        printf("Erreur à la ligne %d, le champ source2 est invalide (aucun nombre hexadecimale ne suit le '#h').\n",ligne+1);
        return 1;
    }   
    else if ((parenthese2[2]=='\0') || (parenthese2[2]=='-' && parenthese2[3]=='\0')){
        printf("Erreur à la ligne %d, le champ source2 est invalide (aucun nombre decimale ne suit le '#').\n",ligne+1);
        return 1;
    }
    if (store) { if (recupere_champ(ligne,2,copy,dest,src1,src2,Imm,hexa,1))   // meme alignement de champ(on a deja permute les champs)
                return 1;
                }
    else       { if (recupere_champ(ligne,2,copy,dest,src1,src2,Imm,hexa,0)) 
                return 1;
               }
    char * test=strstr(copy,"r-0");
    if (test) printf("Remarque: à la ligne %d, on suppose que r-0 correspond r0.\n",ligne+1);
    return 0;
}
int verif_2_operandes_store(char * str, int ligne,int * dest,int * src1,int * imm, int * src2){
    int i=nb_operande(str);
    if (i!=1) {
        if (i==0) printf("Erreur à la ligne %d, était attendu exactement 2 operandes, un seul a été donné.\n",ligne+1);
        else printf("Erreur à la ligne %d, était attendu exactement 2 operandes, %d ont été donnés.\n",ligne+1,i+1);
        return 1;
    }
    char *virgule=strstr(str,",");
    permutation_autour_virgule(str,virgule);
    if (verif_2_operandes_load(str,ligne,src1,dest,imm,src2,1)) return 1;
    return 0;
    }

int est_etiquette(char * str, t_liste_etiquette *L){
    t_etiquette * p=L->tete;
    while (p) { 
        if (!strcmp(p->nom,str)) return p->adresse;
        p=p->suiv;
    }
    return -1;
    }


int verif_saut(char *str,int ligne,int * imm,int *src2, t_liste_etiquette *L){ 
    int i=nb_operande(str); 
    if (i) {
        printf("Erreur à la ligne %d, était attendu exactement 1 operande, %d ont été donnés.\n",ligne+1,i+1);
        return 1;
    }
    int h=est_etiquette(str,L);
    if (h!=-1){
        if (str[0]=='r') printf("Warning à la ligne %d, on suppose que \"%s\" est une etiquette valable, et l'adresse de saut ne sera pas le contenu du registre (si le nom de l'etiquette (\"%s\") en tant que registre est bien valide)\n",ligne+1,str,str);
        *src2=h; *imm=1;  // à priori, l'adresse est valable puisqu'on a deja tester si le nbr d'instruction etait conforme 
        return 0;
        }
    int Imm= (str[0]=='r')? 0: ((str[0]=='#')? 1:-1); *imm=Imm;
    int hexa=(str[1]=='h')? 1: 0;
    char *copy=str; long long SRC2=0; int nb_symbole_hexa=0;
    if (Imm==-1){
        printf("Erreur à la ligne %d, le champ source2 est invalide (ni une valeur immediate indiquee par #, ni un registre valide, ni une etiquette definie).\n",ligne+1);
        return 1;
    }
    else if (!Imm){
        while (*(++str)){
            if ((!((str[0]>='0') && (str[0]<='9'))) && str[0]!='-') {
                printf("Erreur à la ligne %d, le nom du registre src2 est invalide (ce qui suit le 'r' n'est pas un nombre entier).\n",ligne+1);
                return 1;
            }
        }
            if (!(copy[1]) || (((copy[2]=='\0') && (copy[1]=='-') )) )  {
                printf("Erreur à la ligne %d, le nom du registre src2 est invalide (aucun nombre ne suit le 'r').\n",ligne+1);
                return 1;
            } 
        sscanf(copy,"r%lld",&SRC2);
            if (!(SRC2 >=0 && SRC2<=31)) {
                printf("Erreur à la ligne %d, le registre source2 est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",ligne+1,SRC2);
                return 1;
            }
        *src2=SRC2;
        char * test=strstr(copy,"r-0");
        if (test) printf("Remarque: à la ligne %d, on suppose que r-0 correspond r0.\n",ligne+1);
        return 0;
    }
    else if (Imm && hexa){ 
        str ++;
        while (*(++str)){
            if (!(((str[0]>='0') && (str[0]<='9')) || ((str[0]>='a') && (str[0]<='f')) || ((str[0]>='A') && (str[0]<='F')))) {
                printf("Erreur à la ligne %d, le nom du registre src2 est invalide (ce qui suit le '#h' n'est pas un nombre entier exprime en hexadecimale).\n",ligne+1);
                return 1;
            }
        nb_symbole_hexa++;
        }
        if ( (copy[2]=='\0') || (((copy[3]=='\0') && (copy[2]=='-') ))) {
            printf("Erreur à la ligne %d, le nom du registre src2 est invalide (aucun nombre ne suit le '#h').\n",ligne+1);
            return 1;
        }
        sscanf(copy,"#h%llx",&SRC2);
        
    }
    else if (!hexa){
        while (*(++str)){
            if ((!((str[0]>='0') && (str[0]<='9'))) && str[0]!='-') {
                printf("Erreur à la ligne %d, le nom du registre src2 est invalide (ce qui suit le '#' n'est pas un nombre entier exprime en decimale).\n",ligne+1);
                return 1;
            }
        }
        if ( (copy[1]=='\0') || (((copy[2]=='\0') && (copy[1]=='-') ))) {
            printf("Erreur à la ligne %d, le nom du registre src2 est invalide (aucun nombre ne suit le '#').\n",ligne+1);
            return 1;
        }
        sscanf(copy,"#%lld",&SRC2);
    
    }
    short temp=SRC2; *src2=temp; 
    if ((SRC2>SHRT_MAX || SRC2<SHRT_MIN ) && (!(hexa && nb_symbole_hexa<=4))) printf("Warning à la ligne %d, la valeur immediate indiqué depasse la plage de representation d'un nombre binaire sur 16 bits: conversion implicite change la valeur de %lld à %d.\n",ligne+1,SRC2,temp);  
    if (Imm && (*src2)<0) printf("Warning à la ligne %d, l'adresse est negative.\n",ligne +1);
    else if (Imm && ((*src2)%4))   printf("Warning à la ligne %d, l'adresse n'est pas multiple de 4.\n",ligne +1);
    return 0;
    }
    
int verif_inandout(char *string, int i,int *dest){
                long long DEST;
                int j=nb_operande(string); 
                if (j) {
                    printf("Erreur à la ligne %d, était attendu exactement 1 operande, %d ont été donnés.\n",i+1,j+1); return 1;
                }
                else if (string[0]!='r') {
                    printf("Erreur à la ligne %d, le nom du champ destination est invalide, il ne commmence pas par un 'r'.\n",i+1);
                    return 1;
                }
                else if ((string[1]=='\0')|(string[1]=='-' && string[2]=='\0')){
                    printf("Erreur à la ligne %d, le nom du registre destination est invalide (aucun nombre ne suit le 'r').\n",i+1);
                    return 1;
                }
                char *str=string;
                while (*(++str)){
                    if ((!((str[0]>='0') && (str[0]<='9'))) && str[0]!='-') {
                        printf("Erreur à la ligne %d, le nom du registre destination est invalide (ce qui suit le 'r' n'est pas un nombre entier exprime en decimale).\n",i+1);
                        return 1;
                        }
                    }
                sscanf(string,"r%lld",&DEST);
                if (DEST<0 | DEST>31){
                printf("Erreur à la ligne %d, le registre destination est invalide (%lld n'est pas dans l'intervalle [0,31]).\n",i+1,DEST);
                return 1;
                }
                *dest=DEST;
                if (!strcmp("r-0",string)) printf("Remarque: à la ligne %d, on suppose que r-0 correspond r0.\n",i+1);
                return 0;
}

int verif_debut(char *string, int ligne){
      char copy[MAX];
      char *tab[MAX];
      strcpy(copy,string);
      char * mot= strtok(copy," "); int i=0;
      while (mot){
           tab[i]=mot;
           mot=strtok(NULL," ");
           i++;
      }
      for (int j=0;j<i;j++){
        if (j!=i-1 && strcmp(tab[j],",") && strcmp(tab[j+1],",") && tab[j][strlen(tab[j])-1]!=','){
            printf("Erreur à la ligne %d, les caracteres d'un meme operande sont espacés.\n",ligne+1);
            return 1;
        }
    }
    return 0;
}


t_instruction* intialise_liste_instruction(FILE *f,int nbr_instruction){ 
    t_liste_etiquette * L=intialise_liste_etiquette(f);
    char * liste_mnemonique[]={"add","sub","mul","div","and","or","xor","shl","","","ldb","ldw","stb","stw","","","","","","","jmp","jzs","jzc","jcs","jcc","jns","jnc","in","out","rnd","","hlt"};
    t_instruction * liste_instruction=malloc(nbr_instruction*sizeof(t_instruction)); // le free apres la traduction
    char string[MAX]=""; 
    char ope[4]="";  //ope pour operation
    t_instruction I; 
    int error=0; 
    int stop=0;
    for (int i=0;i<nbr_instruction;i++){
            fgets(string,MAX-1,f); 
            char * string2=strstr(string,":");
            if (string2){    //ie contient etiquette
                int j=0;
                while (*(++string2)) string[j++]=*string2;   // on met dans string l'instruction sans etiquette
                string[j]='\0';
                }
            supprime_occurence_debut(string,' '); supprime_occurence(string,'\n'); // ainsi est recupere l'instruction   
            if ((!(*string))){
                printf("Erreur à la ligne %d, absence d'instruction.\n",i+1);
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            strncpy(ope,string,3); 
            int op=est_mnemonique(ope,liste_mnemonique); 
            if (op==-1) {
                printf("Erreur à la ligne %d, \"%s\" n'est pas une instruction valide.\n",i+1,ope);
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            if (string[0]!='i' && !(string[0]=='o' && string[1]=='r') && (strcmp(string,"hlt"))){
                if (string[3]!=' ' && string[3]!=9){
                printf("Erreur à la ligne %d, le code operation de l'instruction n'est pas suivi d'un ou plusieurs espaces.\n",i+1);
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
                }
            }
            else if (string[0]=='i' || (string[0]=='o' && string[1]=='r')){
                if (string[2]!=' ' && string[2]!=9){
                printf("Erreur à la ligne %d, le code operation de l'instruction n'est pas suivi d'un ou plusieurs espaces.\n",i+1);
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
                }
            }
            I.op=op; 
            if ((I.op==5) || (I.op==27)) enlever_mnemonique(string,2) ;   // permet d'avoir dans string les données
            else enlever_mnemonique(string,3); 
            if (!(string[0]) && (I.op!=31) ) {
                printf("Erreur à la ligne %d, absence d'operande pour l'instruction (%s).\n",i+1,ope);
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            if (verif_debut(string,i)) {
                free(liste_instruction);
                libere_liste_etiquette(L);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            supprime_occurence(string,' '); supprime_occurence(string,9); // pour supprimer les tabulations horizontale
            if ((I.op>=0 && I.op<=7) |(I.op==29)){  // instruction à 3 operandes
                if (verif_3_operandes(string,i,&(I.dest),&(I.src1),&(I.imm),&(I.src2))) error++;
            }  
            else if ((I.op>=10 && I.op<=11)){
                if (verif_2_operandes_load(string,i,&(I.dest),&(I.src1),&(I.imm),&(I.src2),0)) error++;
            }
            else if ((I.op>=12 && I.op<=13)){
                if (verif_2_operandes_store(string,i,&(I.dest),&(I.src1),&(I.imm),&(I.src2))) error++;
            }
            else if ((I.op>=20 && I.op<=26)){
                I.dest=0;I.src1=0;
                if (verif_saut(string,i,&(I.imm),&(I.src2),L)) error ++;
            }
            else if ((I.op>=27 && I.op<=28)){
                I.src1=I.src2=I.imm=0; 
                if (verif_inandout(string,i,&(I.dest))) error++;
            }
            else if (I.op==31){
                if (string[0]) {
                    printf("Erreur à la ligne %d, aucune opérande n'est attendu pour l'instruction (hlt).\n",i+1); error ++;
                }
                I.src1=I.src2=I.imm=I.dest=0;
                stop++;
            }
            if (error){  // affaiblissemnt de recopiage de code
                libere_liste_etiquette(L);
                free(liste_instruction);
                fclose(f);
                exit(EXIT_FAILURE);
            }
            liste_instruction[i]=I;
        }
        libere_liste_etiquette(L);
        if (!stop) {
            printf("Warning, absence de l'instruction hlt qui termine l’exécution du programme, il risque d'y avoir une boucle infini, un bus error, ou une instruction à charger à partir d'une adresse invalide (65 533, 65 534 ou 65 535).\n");
        }
        return liste_instruction; 
}

void genere_fichier_hexa(t_instruction * L,int nbr_instruction){
   FILE *f=fopen("hexa.txt","w"); int hexa=0;
    for (int i=0;i<nbr_instruction;i++){
    hexa|= (L[i].op & 0x1F)<<27;     // on fait d'abord le ou bit à bit afin de ne conserver que les bits qui nous interessent
    hexa|= (L[i].dest & 0x1F)<<22;
    hexa|=(L[i].src1 & 0x1F)<<17;
    hexa|=(L[i].imm & 0x01)<<16;
    hexa|=(L[i].src2 & 0xFFFF);
    if (i==nbr_instruction-1) fprintf(f,"%08X",hexa); // enlever ce if et mettre tout le temps fprintf(f,"%08X\n",hexa);
    else fprintf(f,"%08X\n",hexa);
    hexa=0;
    }
    fclose(f);
}

void traduction(char * nom_fichier){
    FILE *f=fopen(nom_fichier,"r");
    if (!f) {
        printf("Le fichier n'a pas pu s'ouvrir ou n'existe pas.\n");
        //fclose(f); a priori on fait pas fclose(NULL);
        exit(EXIT_FAILURE);
        }
    else if (fgetc(f)<=0) {
        printf("Le fichier est vide.\n");
        fclose(f);
        exit(EXIT_FAILURE); 
        }
    fseek(f, 0, SEEK_SET); 
    int l=nb_instruction(f);
    t_instruction* L=intialise_liste_instruction(f,l);
    genere_fichier_hexa(L,l);
    free (L);
    fclose(f);
    if (l>MaxInstruction) {
        printf("La traduction a été réussi avec succès mais le code source contient trop d'instruction (%d) et depasse la valeur maximale atteignable (%d), il ne pourra pas donc pas être chargé en mémoire principale sous peine de saturation.\n",l,MaxInstruction);
        exit(EXIT_FAILURE);
    }
    printf("La traduction a été réussi avec succès.\n");
}






