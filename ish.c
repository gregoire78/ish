/* ish.c : Ingesup Shell */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "tools.h"

#define VERSION 0.35 /* A mettre a jour a chaque evolution */

int RUN=1; /* RUN doit être mis a zero pour stopper le shell */

#define LBUF 512
char buf[LBUF];

extern char **environ;

void Liste_env(char *m)
{
char **Listenv;
char *Val;
   Listenv = environ;
   printf("Liste des variables de l'environnement %s\n",m);
   while (*Listenv != NULL) {
      if ((Val = getenv(*Listenv)) != NULL)
         printf("%s=%s\n",*Listenv,Val);
      else printf("%s\n", *Listenv);
      Listenv++;

   }
}

/* fonction qui determine si un caractere est un separateur */
int is_sepa(char c)
{
  if (c == ' ') return 1;
  if (c == '\t') return 1;
  if (c == '\n') return 1;
  return 0;
}

/* la fct premier mot va analyser le buffer b et var mettre dans pmot
l'adresse du premier mot et dans suite l'adresse du debut de la suite */
char * pmot, * suite;
int premier_mot (char * b)
{
char *d;
   d=b;
   /* on recherche le debut du premier mot */
   while (*d != '\0') {
      if (!is_sepa(*d)) break;
      d++;
   }
   if (*d == '\0') return 0; /* pas de premier mot */
   pmot=d;
   /* on recherche la fin du mot */
   while (*d != '\0') {
      if (is_sepa(*d)) break;
      d++;
   }
   suite = d;
   if (*d == '\0') return 1;
   *d='\0';
   d++;
   /* on recherche le debut du second mot */
   while (*d != '\0') {
      if (!is_sepa(*d)) break;
      d++;
   }
   suite = d;
   return 1; 
}

void commande_externe(char *c, char *p)
{
    printf("%s est une commande externe !\n",c);
}

/* cette fonction regarde si la commande c est interne et l'execute en 
   retournant VRAI sinon elle retourne FAUX */
int commande_interne(char *c, char *p)
{
char *rep;
  /* cas le la commande interne exit */
  if (strcmp(c,"exit") == 0) {
     RUN=0;
     return 1;
  }
  if (strcmp(c,"vers") == 0) {
     printf("ish version %1.2f\n",(float)VERSION);
     return 1;
  }
  if (strcmp(c,"pwd") == 0) {
     rep = getcwd(NULL,0);
     printf("%s\n",rep);
     free(rep);
     return 1;
  }
  if (strcmp(c,"cd") == 0) { /* commande cd */
     if (strlen(p) == 0) { /* cd sans parametre */
       /* je recupere le contenu de la variable HOME */
        rep = getenv("HOME");
     } else {  /* je recupere le 1er parametre */
        premier_mot(p);
        rep = pmot;
     }
     if (chdir(rep) < 0) perror(rep);
     return 1;
  }
  if (strcmp(c,"env") == 0) {
    Liste_env("AVANT");
    return 1;
  }
  return 0;
}

void traite_commande(void)
{
char *comm, * param;
  printf("Traitement de la commande : %s\n",buf);
  /* supprime les commentaires */
  
  /* detection du premier mot */
  if (premier_mot(buf)) {
     comm=pmot;
     param=suite;
     printf("La commande est <%s> le reste est <%s>\n",comm,param);
     /* si la commande est interne alors elle n'est pas externe !*/
     if (! commande_interne(comm,param)) commande_externe(comm,param);
  } else printf("la commande est vide !\n");
}

int main(int N, char *P[])
{

  /* initialisations diverses */
  signal(SIGINT, SIG_IGN); /* on ignore l'interruption du clavier */

  /* boucle principale */
  while (RUN) {
    /* affichage du prompt */
    printf("ish> ");
    fflush(stdout);
    /* lecture de la commande */
    if (lire_ligne(0,buf,LBUF) < 0)
       printf("La taille de la ligne est limitee a %d car. !\n",LBUF);
    else traite_commande();
  }
  printf("Au revoir !\n");
  return 0;
}


