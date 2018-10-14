/*
 * Soubor: proj3.c                                                            *
 * Datum: 2012/12/02                                                          *
 * Autor: Michal Melichar, xmelic17@stud.fit.vutbr.cz                         *
 * Projekt: Reseni osmismerky                                                 *
 * Do predmetu: IZP                                                           *
 * Cislo projektu: 3                                                          *
 * Popis programu: Program slouzi k reseni slovnich rebusu - osmismerek. .....*
 *                 Osmismerka je matice obecnzch romezmeru obsahujici pismena.*
                   Na vstupu budou vzdz mala pismena. Program slouzi k hledani*
                   zadanych slov ve vsech osmi smerech.                       *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>//Pro porovnani retezcu.
#include <ctype.h>
#include <stdbool.h>//Pro true a false


#define M_CH 123
#define V_CH 91
#define ROZDIL_M_V 32
#define MEZERA 32


const int BLOCK_INCREMENT = 32;


/*Textovy retezec s napovedou.*/
const char *HELP =
"Program: Reseni osmismerky.\n"
"Autor:   Michal Melichar(c) 2012\n"
"Popis:   Program zpracuje zadanou matici s malymi pismeny a hleda v ni\n"
"         zadane slova, ktera mohou byt zapsana v osmi smerech.\n\n"
"Pouziti: proj2 -h                     Vytiskne napovedu k pouziti programu.\n"
"         proj2 --test                 Zkontroluje zda zadany soubor\n"
"                                      neobsahuje chyby a vypise naformatovou\n"
"                                      osmismerku na standartni vystup.\n"
"                       data.txt:      Libovolny textovy soubor obsahujici\n"
"                                      vstupni matici.\n"
"         proj2 --search=slovo         Pokusi se najit vyskyty hledaneho slova\n"
"                                      v osmismerce nachazejici se v zadanem\n"
"                                      souboru.\n"
"                       data.txt:      Libovolny textovy soubor obsahujici\n"
"                                      vstupni matici.\n"
"         proj2 --solve                Pokusi se vylustit tajenku vyskrtanim\n"
"                                      zadanych slov ze zadane osmismerky.\n"
"                       data.txt:      Libovolny textovy soubor obsahujici\n"
"                                      vstupni matici.\n"
"                       slova.txt      Libovolny textovy soubor obsahujici\n"
"                                      hledane slova.\n"
"----------------------------------------------------------------------------\n";



/*Jednotlive chyby, krere mohou v prubehu programu nastat.*/
enum {
  E_PARAMETR,
  E_OPEN,
  E_ROZMER,
  E_CLOSE,
  E_ISLOWER,
  E_ALOK,
  E_VELIKOST,
  E_NENI_SLOVO,
};



typedef struct{
  int r;
  int s;
  char **pole_matice;

}Tmatice;



/*Jednotliva chybova hlaseni, ktera se vazou k vyse uvedenemu vyuctovemu typu.*/
const char *ERROR[] = {
  "Spatne zadane parametry programu.",
  "Soubor se nepodarilo otevrit.",
  "Chybne zadane rozmery matice.",
  "Soubor se nepodarilo zavrit.",
  "Matice neobsahuje pouze mala pismena.",
  "Doslo k chybe pri alokaci pameti.",
  "Zadana matice nema spravny tvar.",
  "Zadane slovo nebylo v matici nalezeno.",
};



/*Funkcni prototypy funkci.*/
void printf_err(int error);
bool test_matice(char *soubor, Tmatice *matice);
bool alokuj(Tmatice *matice);
void vypis_matice (Tmatice *matice);
void free_matice(Tmatice *matice);
char *zis_slovo(char *argv);
bool hledani_slova(Tmatice *matice, char *hled_slovo, bool solve);
void otoceni_slova(char *pole);
void nahrad(char *slovo);
void vyskrt_slova(Tmatice *matice, char *soubor);
void vypis_tajenky(Tmatice *matice);



int main(int argc, char **argv)
{
  char *hled_slovo;
  Tmatice matice;


/*Podminka pro vyspis napovedy.*/
  if(argc == 2){
    if(!strcmp(argv[1], "-h")){
      printf("%s", HELP);
    }

    else{
      printf_err(E_PARAMETR);
    }
  }

/*Podminka pro spusteni testu matice.*/
  else if(argc == 3){
    if(!strcmp(argv[1], "--test")){
      if(test_matice(argv[2], &matice) == false){
        return 1;
      }
      vypis_matice(&matice);
    }

    else if((strcmp(argv[1], "--search=")) >= 0){
      hled_slovo = zis_slovo(argv[1]);
      if(test_matice(argv[2], &matice) == false){
        return 1;
      }

      nahrad(hled_slovo);
      hledani_slova(&matice, hled_slovo, false);
      otoceni_slova(hled_slovo);
      hledani_slova(&matice, hled_slovo, false);
      vypis_matice(&matice);
    }

    else{
      printf_err(E_PARAMETR);
    }
  }

  else if(argc == 4){
    if(!strcmp(argv[1], "--solve")){
      if(test_matice(argv[2], &matice) == false){
        return 1;
      }

      vyskrt_slova(&matice, argv[3]);
    }
  }

  else{
    printf_err(E_PARAMETR);
  }

  return 0;
}



/*Jednotliva tela funkci.*/
/*Funkce pro vypisovani chybovych hlaseni.*/
void printf_err(int error)
{
  fprintf(stderr, "%s\n", ERROR[error]);
}


/*Funkce pro nacteni matice.*/
bool test_matice(char *soubor, Tmatice *matice){
  FILE *fr;
  //int r;/*Promenna pro ulozeni poctu radku.*/
  //int s;/*Promenna pro ulozeni poctu sloupcu.*/
  //char **al_matice = NULL;/*Promenna pro ulozeni adresy alokovaneho pole.*/
  char nac_znak;/*Promenna pro konkretni nacteny znak ze souboru.*/
  int pocet_cteni = 0;
  int pom_znak;/*Pomocna promenna pro testovani dalsich znaku v souboru.*/


/*Otevreni souboru spolu osetrenim chyb pri otevreni.*/
  if((fr = fopen(soubor, "r")) == NULL){
    printf_err(E_OPEN);
    return false;
  }

/*Nacteni rozmeru matice s osetrenim spatne zadanych rozmeru a zavrenim *
 *souboru.                                                              */
  if((fscanf(fr, "%d %d", &matice->r, &matice->s)) != 2){
      printf_err(E_ROZMER);
      if(fclose(fr) == EOF){
        printf_err(E_CLOSE);
      }
      return false;
  }

  if(alokuj(matice) == false){/*Ulozeni adresy alokovaneho pole do promenne.*/
  return false;
  }

/*Cyklus pro nacteni hodnot ze souboru do pole matice.*/
  for(int i = 0; i < matice->r; i++){
    for(int j = 0; j < matice->s; j++){
      if(fscanf(fr, "%c", &nac_znak) != EOF){
        /*Testovani zda nacteny znak neni bily  znak.*/
        if(isspace(nac_znak) != 0){
          j--;
          continue;
        }

        /*Testovani zda nacteny znak je male pismeno.*/
        else if(islower(nac_znak) == 0){
          printf_err(E_ISLOWER);
          free_matice(matice);
          return false;
        }

      /*Testovani zda dalsi znak neni pismeno h.*/
        else if((nac_znak == 'c') && ((pom_znak = fgetc(fr)) == 'h')){
            matice->pole_matice[i][j] = M_CH;
            pocet_cteni++;
        }

        else{
          matice->pole_matice[i][j] = nac_znak;
          pocet_cteni++;
        }
      }
    }
  }

/*Cyklus pro overeni zda nenasleduji jeste dalsi znaky.*/
  while((pom_znak = fgetc(fr)) != EOF){
    if(isspace(pom_znak) == 0){
        pocet_cteni++;
    }
  }

/*Podminak pro overeni zda matice byla dobre zadana.*/
 if(pocet_cteni != (matice->r * matice->s)){
    printf_err(E_VELIKOST);
    free_matice(matice);
    return false;
  }

/*Zavreni souboru.*/
  if(fclose(fr) == EOF){
    printf_err(E_CLOSE);
    free_matice(matice);
    return false;
  }

  return true;
}


/*Funkce pro alokace matice.*/
bool alokuj(Tmatice *matice)
{
  //char **pole2D = NULL;

/*Alokace pole ukazatelu na char.*/
  if((matice->pole_matice = malloc(matice->r * sizeof(char *))) == NULL){
    printf_err(E_ALOK);
    return false;
  }

/*Alokace jedlotlivych radku pole.*/
  for(int i = 0; i < matice->r; i++){
    if((matice->pole_matice[i] = malloc(matice->s * sizeof(char))) == NULL){
      /*Dojde-li k chybe zacne se uvolnovat alokovane pole.*/
      printf_err(E_ALOK);
      for(int j = 0; j < i; j++){
        free(matice->pole_matice[i]);
      }
      free(matice->pole_matice);
      return false;
    }
  }

  return true;
}


/*Funcke pro vypis matice v pozadovanem tvaru.*/
void vypis_matice (Tmatice *matice){
  fprintf(stdout, "%d %d\n", matice->r, matice->s);

  for(int i = 0; i < matice->r; i++){
    for(int j = 0; j < matice->s; j++){
      if(matice->pole_matice[i][j] == M_CH){
        fprintf(stdout, "ch ");
      }

      else if(matice->pole_matice[i][j] == V_CH){
        fprintf(stdout, "CH ");
      }

      else{
        fprintf(stdout, "%c ", matice->pole_matice[i][j]);
      }

    }
    fprintf(stdout, "\n");
  }

  free_matice(matice);
}


/*Funkce pro uvolneni pole.*/
void free_matice(Tmatice *matice){
  /*Uvolnime jednotlive radky.*/
  for(int i = 0; i < matice->r; i++){
    free(matice->pole_matice[i]);
  }
/*Uvolnime pole ukazatelu.*/
  free(matice->pole_matice);
}


/*Funkce pro ziskani hledaneho slova z parametru.*/
char *zis_slovo(char *argv){
  char *hled_slovo;
  hled_slovo = argv + strlen("--search=");

  return hled_slovo;
}


/*Funkce pro vyhledani slova v matici.*/
bool hledani_slova(Tmatice *matice, char *hled_slovo, bool solve)
{
  int delka_slova;
  int ir;
  int js;
  int k;/*Promenna pro posun v retezci hledaneho slova.*/
  bool naslo_slovo = false;


  delka_slova = strlen(hled_slovo);

/*Cyklus pro prochazeni zda je tam hledane pismenko.*/
  for(int i = 0; i < matice->r; i++){
    for(int j = 0; j < matice->s; j++){


      /*Podmninka pro hledane pismeno.*/
      /*Podminka pro hledani slova ve vodorovnem smeru.*/
      if((matice->pole_matice[i][j] == *hled_slovo) ||
        ((matice->pole_matice[i][j] + ROZDIL_M_V) == *hled_slovo)){
        /*Podminka, pokud se hledane slovo nevejde na radek.*/
        if(delka_slova <= (matice->s - j)){
          k = 1;
          /*Cyklus pro hledani pismen smerem doprava.*/
          for(js = (j + 1); js < matice->s; js++){
            if(*(hled_slovo + k) == '\0'){
              naslo_slovo = true;
              break;
            }
          /*Podminka jestli se najite pismeno nerovna dalsimu znaku v matici.*/
            else if(matice->pole_matice[i][js] != *(hled_slovo + k)){
              if((matice->pole_matice[i][js] + ROZDIL_M_V) == *(hled_slovo + k)){
                k++;
                continue;
              }

              else{
                naslo_slovo = false;
                break;
              }
            }

            k++;
          }
          /*Jestlize dalsi znak je uz \0, tak nastavime promennou na true.*/
          if(*(hled_slovo + (k++)) == '\0'){
              naslo_slovo = true;
          }

          /*Cyklus pro prevedeni velkych pismen.*/
          if(naslo_slovo == true){
            for(int y = j; y < js ; y++){
              if((matice->pole_matice[i][y] >= 'a') && (matice->pole_matice[i][y] <= M_CH)){
                matice->pole_matice[i][y] -= ROZDIL_M_V;
              }

              else{
                solve = false;
              }
            }
          }
          if((naslo_slovo == true) && (solve == true)){
            return true;
          }
        }


        /*Pominka pro hledani slova svislym smerem.*/
        if(delka_slova <= (matice->r - i)){
          k = 1;
          /*Cyklus pro vyhledavani smerem nahoru.*/
          for(ir = (i + 1); ir < matice->r; ir++){
            if(*(hled_slovo + k) == '\0'){
              naslo_slovo = true;
              break;
            }

          /*Porovnavame pismeno v matici s nasim heldanym pismenem.*/
            else if(matice->pole_matice[ir][j] != *(hled_slovo + k)){
              if((matice->pole_matice[ir][j] + ROZDIL_M_V) == *(hled_slovo + k)){
                k++;
                continue;
              }

          /*Jestlize hledane pismeno nenajdeme, tak nastavime promennou na false.*/
              else{
                naslo_slovo = false;
                break;
              }
            }
            k++;
          }

          if(*(hled_slovo + (k++)) == '\0'){
            naslo_slovo = true;
          }

          /*Cyklus pro prevod najitych slov na velka pismena.*/
          if(naslo_slovo == true){
            for(int x = i; x < ir; x++){
              if((matice->pole_matice[x][j] >= 'a') && (matice->pole_matice[x][j] <= M_CH)){
                matice->pole_matice[x][j] -= ROZDIL_M_V;
              }

              else{
                solve = false;
              }
            }
          }
          if((naslo_slovo == true) && (solve == true)){
            return true;
          }
        }


      /*Podminka pro hledani v sikmem smeru.*/
        if((delka_slova <= (matice->r - i)) && (delka_slova <= (matice->s - j))){
          k = 1;
          ir = (i + 1);
          for(js = (j + 1); js < matice->s; js++){
            if(ir >= matice->r){
              break;
            }

            if(*(hled_slovo + k) == '\0'){
              naslo_slovo = true;
              break;
            }

        /*Porovnavame pismeno v matici s nasim hledanym pismenem.*/
            else if(matice->pole_matice[ir][js] != *(hled_slovo + k)){
              if((matice->pole_matice[ir][js] + ROZDIL_M_V) == *(hled_slovo + k)){
                k++;
                ir++;
                continue;
              }
          /*Jestlize hledane pismeno nenajdeme, tak nastavime promennou na false.*/
              else{
                naslo_slovo = false;
                break;
              }
            }
            k++;
            ir++;
          }

          if(*(hled_slovo + (k++)) == '\0'){
            naslo_slovo = true;
          }

          /*Cyklus pro prevedeni velkych pismen.*/
          if(naslo_slovo == true){
            int x = i;
            for(int y = j; y < js ; y++){
              if(x >= ir){
                break;
              }

              if((matice->pole_matice[x][y] >= 'a') && (matice->pole_matice[x][y] <= M_CH)){
                matice->pole_matice[x][y] -= ROZDIL_M_V;
              }

              else{
                solve = false;
              }

              x++;
            }
          }
          if((naslo_slovo == true) && (solve == true)){
            return true;
          }
        }


        /*Podmina pro hledani v druhem sikmem smeru.*/
        if((delka_slova <= (matice->r - i)) && (((j + 1) - delka_slova) >= 0)){
          k = 1;
          ir = (i + 1);
          for(js = (j - 1); js >= 0; js--){
            if(ir >= matice->r){
              break;
            }

            if(*(hled_slovo + k) == '\0'){
              naslo_slovo = true;
              break;
            }

        /*Porovnavame pismeno v matici s nasim hledanym pismenem.*/
            else if(matice->pole_matice[ir][js] != *(hled_slovo + k)){
              if((matice->pole_matice[ir][js] + ROZDIL_M_V) == *(hled_slovo + k)){
                k++;
                ir++;
                continue;
              }
          /*Jestlize hledane pismeno nenajdeme, tak nastavime promennou na false.*/
              else{
                naslo_slovo = false;
                break;
              }
            }
            k++;
            ir++;
          }

          if(*(hled_slovo + (k++)) == '\0'){
            naslo_slovo = true;
          }

          /*Cyklus pro prevedeni velkych pismen.*/
          if(naslo_slovo == true){
            int x = i;
            for(int y = j; y > js ; y--){
              if(x >= ir){
                break;
              }

              if((matice->pole_matice[x][y] >= 'a') && (matice->pole_matice[x][y] <= M_CH)){
                matice->pole_matice[x][y] -= ROZDIL_M_V;
              }

              else{
                solve = false;
              }
              x++;
            }
          }

        }
        if((naslo_slovo == true) && (solve == true)){
            return true;
        }
      }
/*Konec podminky, zda jsme nasli prvni pismeno  v matici.*/
      }
    }
    return false;
  }



/*Funkce pro otoceni hledaneho slova.*/
void otoceni_slova(char *pole)
{
  char c;/*Pomocna promenna pro ulozeni znaku.*/
  int delka;/*Delka hledaneho slova.*/
  int opacny_prvek;/*Promenan pro vypocet pozenice od konce retezce.*/
  int pocet_otoceni;/*Promenna pro vypocet podminky cyklu for.*/

  delka = strlen(pole);
  /*Pocet znaku vydelim 2, abych dostal pulku slova.*/
  pocet_otoceni = (delka / 2);

  for(int i = 0; i <= (pocet_otoceni - 1); i++){
    opacny_prvek = (delka - 1)  - i;
    c = pole[i];
    pole[i] = pole[opacny_prvek];
    pole[opacny_prvek] = c;
  }
}


/*Funkce pro nahrazeni znaku c-h za jeden znak ch.*/
void nahrad(char *slovo)
{
  int k = 0;
  while(*(slovo + k) != '\0'){
    if(*(slovo + k) == 'c'){
      k++;
      if(*(slovo + k) == 'h'){
        k--;
        *(slovo + k) = M_CH;
        /*Posunuti retezce o danou delku.*/
        memmove(slovo + (k + 1), slovo + (k + 2), (strlen(slovo) - k + 1));
      }
    }
    k++;
  }
}


/*Funkce pro vyskrtavani zadanych slov ze zadanne matice.*/
void vyskrt_slova(Tmatice *matice, char *soubor)
{
  FILE *fr;
  char *buf = NULL;
  int znak;/*Nactene znak.*/
  int count = 0;/*Obsahuje pocet prectenych znaku.*/
  int bufsize = 0;/*Obsahuje skutecnou velikost bufferu.*/
  int k = 0;
  int pocet_pismen;/*Promenna pro ulozeni poctu znaku v hledanem slove.*/
  int zacatek_slova;/*Promenna pro ulozeni pozice v retezci vsech hledanych*
                     *slov, zadanych ze souboru.                           */
  bool mezera2 = false;


/*Otevreni souboru spolu osetrenim chyb pri otevreni.*/
  if((fr = fopen(soubor, "r")) == NULL){
    printf_err(E_OPEN);
    return;
  }

/*Realokace pole, kam se ukladaji data ze souboru, ktery obsahuje hledane *
 *slova. Dale nasleduje ulozeni hledanych slov do alokovaneho pole.       *
 *Na konec pridam 0, pro ukonceni retezce.                                */
  while((znak = fgetc(fr)) != EOF){
    if(count == bufsize){
      bufsize += BLOCK_INCREMENT;
      char* rebuf = realloc(buf, bufsize*sizeof(char));
      if(rebuf == NULL){
        free(buf);
        buf = NULL;
        break;
      }
      buf = rebuf;
    }

/*Testovani zda nahrany znak neni bily znak. Pokud ano, tak jej nahradim *
 *mezerou. Pokud je vice bilych znaku po sobe, tak nahradim pouze jednou *
 *mezerou a dalsi bile znaky preskacu.                                   */
    if(isspace(znak) != 0){
      if(mezera2 == false){
        buf[count++] = MEZERA;
        mezera2 = true;
      }
    }

    else{
      buf[count++] = znak;
      mezera2 = false;
    }
  }
  buf[count] = '\0';/*Na konec pole ulozim nulovy znak.*/


  /*Zavreni souboru.*/
  if(fclose(fr) == EOF){
    printf_err(E_CLOSE);
    return;
  }
  fr = NULL;
    nahrad(buf);/*Nahrazeni c-h za jeden znak ch.*/


/*Cyklus pro spocitani delky slova.*/
while(*(buf + k) != '\0'){
  if(*(buf + k) == MEZERA){
    k++;
  }

  pocet_pismen = 0;
  zacatek_slova = k;
  while((*(buf + k)) != MEZERA){
    if(*(buf + k) == '\0'){
      break;
    }

    pocet_pismen++;
    k++;
  }


/*Alokace druheho pole, ktere slouzi pro ulozeni jednoho hledaneho slova.*/
  char *hled_slovo = malloc((pocet_pismen + 1) * sizeof(char));
  if(hled_slovo == NULL){
    printf_err(E_ALOK);
    free(buf);
    free(matice);
    return;
  }

  strncpy(hled_slovo, (buf + zacatek_slova), pocet_pismen);
  *(hled_slovo + pocet_pismen) = '\0';


  if(hledani_slova(matice, hled_slovo, true) == false){
    otoceni_slova(hled_slovo);
    hledani_slova(matice, hled_slovo, true);
  }
  free(hled_slovo);
}

vypis_tajenky(matice);
free(buf);
}


/*Funkce pro vypis tajenky.*/
void vypis_tajenky(Tmatice *matice)
{
  for(int i = 0; i < matice->r; i++){
    for(int j = 0; j < matice->s; j++){
      if((matice->pole_matice[i][j] >= 'a') &&
         (matice->pole_matice[i][j] <= M_CH)){
        if(matice->pole_matice[i][j] == M_CH){
          printf("ch");
        }
        else{
          printf("%c", matice->pole_matice[i][j]);
        }
      }
    }
  }
  putchar('\n');
  free_matice(matice);
}
