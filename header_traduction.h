#ifndef HEADER_TRADUCTION_H
	#define HEADER_TRADUCTION_H

typedef struct{
    int op;
    int dest;
    int src1;
    int imm;
    int src2;
}t_instruction;




typedef struct t_etiquette{
    char * nom;
    struct t_etiquette *suiv;
    int adresse;
}t_etiquette;


typedef struct{
    t_etiquette * tete;
   // int nb_etiquette;
}t_liste_etiquette;

void affiche_liste_etiquette(t_liste_etiquette *L); // auxiliaire à I
int supprime_occurence(char *str, char c); // auxiliaire à I
int contient_espace_tabulation_milieu(char *str);// auxiliaire à I
int nb_occurences(t_etiquette *q,t_liste_etiquette*L); // auxiliaire à I
int contient_doublon(t_liste_etiquette *L); // auxiliaire à I
void ajout_tete_liste_etiquette(t_liste_etiquette *L,t_etiquette *p); // auxiliaire à I
int des_chiffres_et_des_lettres(const char *str); // auxiliaire à I
void libere_liste_etiquette(t_liste_etiquette *L); 
t_etiquette * initialise_etiquette(void);
int verif_tabulation(char * string,int ligne);
t_liste_etiquette* intialise_liste_etiquette(FILE *f);   //PRINCIPALE I
int nb_instruction(FILE *f);
t_instruction* intialise_liste_instruction(FILE *f,int nbr_instruction); // PRINCIPALE II
int est_mnemonique(char * ope, char ** liste_mnemonique);
void enlever_mnemonique(char *str,int i);
int nb_operande(char * str);
int recupere_champ(int ligne,int nb_operande,char *str, int *dest, int *src1,int *src2,int Imm,int hexa, int store);
int verif_3_operandes(char * str, int ligne,int * dest,int * src1,int * imm, int * src2);
void permutation_autour_virgule(char * str, char * virgule);
int verif_2_operandes_load(char * str, int ligne,int * dest,int * src1,int * imm, int * src2, int store);
int verif_2_operandes_store(char * str, int ligne,int * dest,int * src1,int * imm, int * src2);
int est_etiquette(char * str, t_liste_etiquette *L);
int verif_saut(char *str,int ligne,int * imm,int *src2, t_liste_etiquette *L);
int verif_inandout(char *string, int i,int *dest);
t_instruction* intialise_liste_instruction(FILE *f,int nbr_instruction);
void genere_fichier_hexa(t_instruction * L,int nbr_instruction);
void supprime_occurence_debut(char *str, char c);
void traduction(char * nom_fichier);
int verif_debut(char *string, int ligne);




#endif