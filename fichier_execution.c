#include "header_execution.h"


void charge_programme_en_memoire(FILE * f, char * memoire){ 
    int code_instruction,octet=0;
    while (fscanf(f,"%X\n",&code_instruction)!=EOF){
        if (octet+4>taille_memoire){  
            printf("Le programme contient trop d'instructions et ne peut pas être chargé dans la memoire.\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
        memoire[octet]=((code_instruction>>24) & 0x0FF);  // ici l'utilisation de unsigned int code_instruction est utile uniquement pour le premier decalage, dans tous les cas on devra utiliser des masquages de bits
        memoire[++octet]=((code_instruction>>16) & 0x0FF);
        memoire[++octet]=((code_instruction>>8) & 0x0FF);
        memoire[++octet]=((code_instruction) & 0x0FF);
        octet++;
    }
}

void execute_programme(FILE* f,char* memoire, short * registre, int *N, int *Z, int *C, unsigned short *PC){
	int fin=0;
	charge_programme_en_memoire(f,memoire);
	while (!fin) {
        if (execute_instruction(memoire,registre,N,Z,C,PC,&fin)) {
          fclose(f); 
          exit(EXIT_FAILURE);
        }
    }
}


void maj_bit_Z_N(short resultat,int *N, int *Z){
    *Z=(resultat)? 0:1;
    *N=(resultat>=0)? 0:1;
}

int execute_instruction(char* memoire,short * registre,int *N, int *Z, int *C, unsigned short *PC,int * fin){
    t_instruction2 * p = intialise_instruction(memoire,PC);
    if (!p) return 1;
    if (!(p->imm) && (p->src2>nb_registre-1 || p->src2<0)) {
        printf("Erreur, reference registre invalide : (%d) n'est pas dans l'intervalle [0,31].\n",p->src2);
        free(p);
        return 1;
    }
    short resultat=0;  int puissance=0,puissance2=0; unsigned short adresse=0,adresse2=0; short octet1=0,octet2=0;
    short contenu_src2= (p->imm)? p->src2 : registre[p->src2];
    short contenu_src1= registre[p->src1];
    int maj_registre_etat=1,int_resultat=0,dans_registre=0,int_contenu_src1=(((int)contenu_src1) & 0x0FFFF),int_contenu_src2=((((int)contenu_src2) & 0x0FFFF)); 
   
    switch (p->op){
        case 0: //add remarque: GIGO: on s'occupe pas des debordements arithmetique
            int_resultat=int_contenu_src1+int_contenu_src2;
            *C=((int_resultat>>16) & 0x01); 
            resultat=contenu_src1+contenu_src2;  
            *PC=*PC+4;
            dans_registre=1;
            break;
        
        case 1: //sub
            int_resultat=int_contenu_src1+int_contenu_src2;
            *C=((int_resultat>>16) & 0x01); 
            resultat=contenu_src1-contenu_src2; 
            *PC=*PC+4;
            dans_registre=1;
            break;
        
        case 2: //mul
            int_resultat=(int_contenu_src1 & 0x0FF)*(int_contenu_src2 & 0x0FF); 
            *C=((int_resultat>>16) & 0x01);
            resultat=(contenu_src1 & 0x0FF)*(contenu_src2 & 0x0FF);
            *PC=*PC+4;
            dans_registre=1;
            break;
        
        case 3: //div
            if (!contenu_src2) {
				printf("Erreur, division par zero.\n");
                free(p);
                return 1;
			}
			resultat = contenu_src1/contenu_src2;
			*PC=*PC+4;
            *C=0; // il n'y a jamais de retenue lors de la division et on doit la remettre à 0, cf poly archi page 100
            dans_registre=1;
			break;
        
        case 4: //and
            resultat = contenu_src1 & contenu_src2;
			*PC=*PC+4;
            *C=0;
            dans_registre=1;
			break;
        
        case 5: //or
			resultat = contenu_src1 | contenu_src2;
			*PC=*PC+4;
            *C=0;
            dans_registre=1;
			break;
        
        case 6: //xor
			resultat = contenu_src1 ^ contenu_src2;
			*PC=*PC+4;
            *C=0;
            dans_registre=1;
			break;
         
        case 7: //shl
            dans_registre=1; 
			*PC=*PC+4;
            if (contenu_src2 > 16 || contenu_src2< -16){     //on perd tous les bits
                resultat=0;
                *C=0;
            }
            //else if (contenu_src2 == 16 || contenu_src2==-16) // on perd tous les bits et C vaut le bit de poids faible\fort
            //{
              //  resultat=0;
               // *C=(int)contenu_src2==16?(contenu_src1 & 0x01): (((contenu_src1 & 0x8000)>>15)& 0x01);
            //}
            else {
                resultat = (short) (contenu_src2>0? ((int_contenu_src1<<contenu_src2) &  0x0FFFF): ((int_contenu_src1>>(-1*contenu_src2)) & 0x0FFFF)); // obliger d'utiliser int_contenu_src1 car lorsqu'on decale un nombre negatif, on a ajout de 1 et pas 0 dans les bits qui disparaissent(recopie du bit de poids fort)
                puissance= contenu_src2>0? 16-contenu_src2 : (-1*contenu_src2)-1; // on cherche le dernier bit sorti
                if (puissance==-1) *C=0;
                else {
                    puissance2=puissance;
                    puissance=(int)pow(2,puissance);
                    *C=((int_contenu_src1 & puissance)>>(puissance2)& 0x01);
                }
            } 
			break;
        
        case 10: //ldb
			adresse=(unsigned short) contenu_src1+contenu_src2;
			resultat = (short)memoire[adresse];
			*PC=*PC+4;
            *C=0;
            dans_registre=1;
			break;
        
        case 11: //ldw
            adresse=(unsigned short) contenu_src1+contenu_src2;
            adresse2=(unsigned short) adresse +1;
            octet1=(short)memoire[adresse]; octet2=(short)memoire[adresse2];
            octet1=(octet1 & (0x0FF));octet2=((octet2 & (0x0FF))<<8);
            resultat= octet1 | octet2;
            *PC=*PC+4;
            *C=0;
            dans_registre=1; 
            break;
        
        case 12: //stb
			adresse=(unsigned short) registre[p->dest]+contenu_src2;
            memoire[adresse]=(char)(contenu_src1 & 0x0FF);
			*PC=*PC+4;
            *C=0;
            resultat=(contenu_src1 & 0x0FF);
			break; 
        
        case 13: //stw
            adresse=(unsigned short) registre[p->dest]+contenu_src2;
            adresse2=(unsigned short)adresse+1;
            memoire[adresse]=(char)(contenu_src1 & 0x0FF);
            memoire[adresse2]=(char)((contenu_src1>>8) & 0x0FF);
            *C=0;
			*PC=*PC+4;
            resultat=contenu_src1;
            break;
        
        case 20: //jmp remarque pour les jump: si adresse negative , on se ramene à positive, on ne modifie pas les bits du registre d'etat 		
			*PC=(unsigned short)contenu_src2;
            maj_registre_etat=0;
            break;

        case 21: //jzs
			*PC = (unsigned short)((*Z)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;
        
        case 22: //jzc
			*PC = (unsigned short)((!*Z)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;

        case 23: //jcs
			*PC = (unsigned short)((*C)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;
         
        case 24: //jcc
			*PC = (unsigned short)((!*C)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;
        
        case 25: //jns
			*PC = (unsigned short)((*N)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;   

        case 26: //jnc
			*PC = (unsigned short)((!*N)? contenu_src2: *PC+4);
            maj_registre_etat=0;
			break;   

        case 27: //in
			do {
	           printf("\nEntrez un entier : ");
	           fflush(stdin);
            }
	        while (!scanf("%hd",&resultat));
			*PC=*PC+4;
            *C=0;
            dans_registre=1;
			break;
			
		case 28: //out
			resultat=registre[p->dest];
			printf("Contenue du registre r%d : %hd\n",p->dest,resultat);
			*PC=*PC+4;
            *C=0;
			break;
        
        case 29: //rnd
            if (contenu_src1>=contenu_src2){
                printf("Erreur, l'intervalle d'entier pour la generation de nombre aléatoire est invalide %hd >= %hd, [%hd,%hd[ est vide.\n", contenu_src1,contenu_src2,contenu_src1,contenu_src2);
                free(p);
                return 1;
            }
            resultat= contenu_src1 + rand()%(contenu_src2-contenu_src1);
            *PC=*PC+4;
            dans_registre=1;
            *C=0; 
			break;

        case 31: //hlt
            *fin=1;
            maj_registre_etat=0;
            break;
        
        default:
			printf("Erreur, %d n'est pas le code operation d'une instruction faisant partie de notre jeu d'instruction.\n",p->op);
            free(p);
            return 1;
			break;
    }
    
    if (maj_registre_etat) maj_bit_Z_N(resultat,N,Z);
    if (dans_registre==1) registre[p->dest]=resultat; 
    registre[0]=0;
    free(p);
    return 0;

}

t_instruction2 * intialise_instruction(char* memoire,unsigned short *PC){
    if ((*PC>taille_memoire-1)) {  
        printf("Bus error: reference memoire invalide (%d n'est l'adresse d'aucun octet existant dans notre memoire, on considere qu'il y a \"rien\" à partir de l'adresse du dernier octet).\n",*PC);
        return NULL;
    }
    else if (((*PC)+3>taille_memoire-1)){
        printf("Erreur, l'instruction est à charger à partir d'une adresse invalide (65 533, 65 534 ou 65 535), ici: %d.\n",*PC);
        return NULL;
    }
    t_instruction2 *p=malloc(sizeof(t_instruction2));
    if (!p) {
        printf("Malloc a échoué.\n");
        return NULL;
    }
    char octet1=memoire[*PC],octet2=memoire[*PC+1],octet3=memoire[*PC+2],octet4=memoire[*PC+3];
    short octet3bis=(short)octet3, octet4bis=(short)octet4;
    p->op=(int)((octet1>>3) & 0x1F);
    p->dest=(int)(((octet1 <<2) & 0x1C) | ((octet2>>6) & 0x03));
    p->src1=(int)(((octet2>>1) & 0x1F));
    p->imm=(int)(octet2 & 0x01);
    p->src2=(octet3bis<<8 | (octet4bis & 0x00FF));
    return p;
}


void execution(){
FILE * f=fopen("hexa.txt","r"); 
if (!f){
    printf("Le fichier contenant le programme écrit en langage machine n'a pas pu s'ouvrir ou n'existe pas.\n");
    return ;
}
char memoire[taille_memoire]={0};
short registre[nb_registre]={0}; 
int N=0,Z=0,C=0;
unsigned short PC=0;
srand(time(NULL));
execute_programme(f,memoire,registre,&N,&Z,&C,&PC);
fclose(f);
return ;
}   









