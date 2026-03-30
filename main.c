#include "header_execution.h"
#include "header_traduction.h"

void affiche_bin_short(unsigned short  m){
	if (m>=2) {
		affiche_bin_short(m>>1);
	}
	printf("%u",m%2);
}
void affiche_bin_int(unsigned int  m){
	if (m>=2) {
		affiche_bin_int(m>>1);
	}
	printf("%u",m%2);
}

void affiche_bin_char(unsigned char m){
	if (m>=2) {
		affiche_bin_char(m>>1);
	}
	printf("%u",m%2);
}

int main(int argc, char* argv[]){
    if (argc==1){
        printf("Mettre le nom du fichier sur la ligne de commande.\n");
		return 1;
        }
    char * nom_fichier=argv[1];
    traduction(nom_fichier);
    execution();
	

}