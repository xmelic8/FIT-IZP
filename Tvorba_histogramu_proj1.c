/*
 * Soubor: proj1.c                                                            *
 * Datum: 2012/10/21                                                          *
 * Autor: Michal Melichar, xmelic17@stud.fit.vutbr.cz                         *
 * Projekt: Tvorba histogramu                                                 *
 * Do predmetu: IZP                                                           *
 * Cislo projektu: 1                                                          *
 * Popis programu: Program provede vypocet cetnosti vyskytu znaku vstupniho   *
 *                 textu. Po znacich precte vsechny znaky ze standartniho     *
 *                 vstupu a na vystup vytiskne textovou tabulku s jejich      *
 *                 cetnosti.                                                  *
 */


/*Hlavickove soubory*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>


//Definice konstatnt*/
#define VELIKOST_POLE 256
#define HORNI_MEZ_POLE 255
#define MAX_CISLO 256


/*Definice chyb*/
#define ERR1 "Spatne zadane parametry programu."


/*Textovy retezec s napovedou.*/
const char *HELP =
  "Program: Tvorba histogramu.\n"
  "Autor:   Michal Melichar(c) 2012\n"
  "Popis:   Program provede vypocet cetnosti vyskytu znaku vstupniho textu.\n"
  "Pouziti: proj1 -h         Vypsani napovedy k parametrum programu.\n"
  "         proj1 --help     Vypsani napovedy k parametrum programu.\n"
  "         proj1            Bez parametru. Vytiskne vsechny relevatni radky\n"
  "                          histogramu v poradi danem ordinalnimi hodnotami\n"
  "                          znaku.\n"
  "         proj1 N          Jeden ciselny parametr. Vytiskne prvnich N radku\n"
  "                          histogramu v poradi danem cestnosti znaku\n"
  "                          od nejcastejsiho znaku.\n"
  "------------------------------------------------------------------------------\n";


/*Deklarace funkci.*/
int nacteni(void);
void tisk(int hodnota, int vyskyt);
void prace_s_polem(int nacteny_parametr, int spusteni_s_parametrem);
int prevod_parametru(char **argv);
void razeni_znaku(unsigned char *pole_znaku, int pocet_parametru, int nacteny_parametr);



int main(int argc, char *argv[])
{
  int nacteny_parametr;/*Hodnota ciselneho parametru.*/

/*Pokud neni zadan parametr.*/
  if(argc == 1){
    nacteny_parametr = VELIKOST_POLE;
    prace_s_polem(nacteny_parametr, argc);
  }

/*Pokud je zadan parametr.*/
  else if(argc == 2)
  {
    if((!strcmp (argv[1],"-h")) || (!strcmp (argv[1], "--help"))){
      printf("%s", HELP);
    }

    else{
      nacteny_parametr = prevod_parametru(argv);
      if(nacteny_parametr == EOF){
        fprintf(stderr, "%s\n", ERR1);
        return 1;
      }
      prace_s_polem(nacteny_parametr, argc);
    }
  }

  else{
    fprintf(stderr, "%s\n", ERR1);
  }

  return 0;
}



/*Definice funkci.*/
/*Funkce nacte jeden znak a vraci nacteny jeden znak.*/
int nacteni(void)
{
  int znak;

  znak = getchar();

  return znak;
}


/*Funkce pro vypis znaku, dle zadanych kriterii.       *
 *Je-li znak vetsi nez UCHAR_MAX - 1, vytiskne se nize *
 *uvedeny formatovaci retezec.                         */
void tisk(int hodnota, int vyskyt)
{
  if(vyskyt > (UCHAR_MAX - 1)){
    printf("%d '%c': NNN\n", hodnota, hodnota);
  }
  else if((isprint(hodnota) == 0) && (vyskyt != 0)){
    printf("%d: %d\n", hodnota, vyskyt);
  }
  else if((vyskyt != 0)){
    printf("%d '%c': %d\n", hodnota, hodnota, vyskyt);
  }
}


/*Funkce, ktera vytvori pole, vola funkce pro nacteni znaku do pole.*
 *Dael se zde vola funkce pro sparve serazeni hodnot pred vypisem   */
void prace_s_polem(int nacteny_parametr, int pocet_parametru)
{
  unsigned char pole_znaku[VELIKOST_POLE] = {0};
  int hodnota;/*Promenna pro ulozeni ASCII hodnoty znaku.*/

/*Zavolani funkce pro nacteni znaku, ulozeni znaku do pole a kontrola preteceni.*/
  while((hodnota = nacteni()) != EOF){
    if((pole_znaku[hodnota]) > (UCHAR_MAX - 1)){
      ;
    }
    else{
      pole_znaku[hodnota]++;
    }
  }

  razeni_znaku(pole_znaku, pocet_parametru, nacteny_parametr);
}


/*Funkce pro prevod parametru, ve tvaru retezce, na ciselnou hodnotu.*/
int prevod_parametru(char **string)
{
  int znak = 0;/*Vysledna hodnota znaku.*/
  int pomocny_znak;
  int i = 0;/*Pomocna promenna pro posun v poli retezce.*/

  while((*(string[1] + i)) != '\0'){
    if(isdigit(*(string[1] + i)) == 0){
      return EOF;
    }

    pomocny_znak = (*(string[1] + i) - '0');
    znak = (znak * 10) + pomocny_znak;
    i++;
    if(znak > MAX_CISLO){
      return EOF;
    }
  }

  /*Jestlize prevedeny parametr je roven 0.*/
  if(znak == 0){
    return EOF;
  }

  return znak;
}


/*Funkce pro spravne razeni znaku pred zavolanim funkce pro tisk tabulky.*/
void razeni_znaku(unsigned char *pole_znaku, int pocet_parametru, int nacteny_parametr)
{
  int pocet_tisknuti = 1;/*Promenna pro pocitani radku vystupni tabulky.*/

  /*Tisk vystupni tabulky, pri spusteni programu bez parametru..*/
  if(pocet_parametru == 1){
    for(int hodnota = 0; hodnota <= HORNI_MEZ_POLE; hodnota++){
    tisk(hodnota, pole_znaku[hodnota]);
    }
  }

/*Tisk vystupni tabulky, pri spusteni programu s parametrem.*/
  else if(pocet_parametru == 2){
    /*Prvni cyklus for kontroluje nejvyssi vyskyty znaku.                        *
     *Druhy cyklus for kontroluje spravne poradi znaku se stejnym poctem vyskytu.*
     *Zaroven hlida zvoleny pocet tisknutych radku, pomoci parametru.            */
    for(int vyskyt_znaku = HORNI_MEZ_POLE; vyskyt_znaku >= 0; vyskyt_znaku--){
      for(int hodnota = 0; hodnota <= HORNI_MEZ_POLE; hodnota++){
        if(pole_znaku[hodnota] == vyskyt_znaku){
          if(nacteny_parametr >= pocet_tisknuti++){
            tisk(hodnota, pole_znaku[hodnota]);
          }
        }
      }
    }
  }
}
