/*
 * Soubor: proj2.c                                                            *
 * Datum: 2012/11/11                                                          *
 * Autor: Michal Melichar, xmelic17@stud.fit.vutbr.cz                         *
 * Projekt: Iteracni vypocty                                                  *
 * Do predmetu: IZP                                                           *
 * Cislo projektu: 2                                                          *
 * Popis programu: Program pomoc iteracnich algoritmu zpracuje libovolne      *
 *                 dlouhou posloupnost ciselnych hodnot typu double, ktere ...*
 *                 budou v textove podobe zapsane na vstup. Na kazdou nactenou*
 *                 hodnotu bude aplikovana vybrana funkce. Na vystup se vypise*
 *                 ciselna posloupnost vysledku.                              *
 */



/*Hlavickove soubory.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>//Pro porovnani retezcu.
#include <float.h>//Kvuli DBL_DIG.
#include <math.h>




/*Definice globalnich konstant.*/
const double IZP_E = 2.7182818284590452354;        // e
const double IZP_PI = 3.14159265358979323846;      // pi
const double IZP_2PI = 6.28318530717958647692;     // 2*pi
const double IZP_PI_2 = 1.57079632679489661923;    // pi/2
const double IZP_PI_4 = 0.78539816339744830962;    // pi/4
const double ODMOCNINA = 1.73205080756887729352;   // odmocnina ze 3
const double DESETINNY_RAD = 0.1;
const double ODMOCNINA_2 = 0.5;//odmocnina ze 2


/*Textovy retezec s napovedou.*/
const char *HELP =
  "Program: Iteracni vypocty.\n"
  "Autor:   Michal Melichar(c) 2012\n"
  "Popis:   Program pomoci iteracnich vypoctu zpracuje libovolne dlouhou\n"
  "         posloupnost ciselnych hodnot typu double.\n"
  "Pouziti: proj2 -h                 Vytiskne napovedu k pouziti programu.\n"
  "         proj2 --powxa sigdig a   Vypocet mocninne funkce s realnym\n"
  "                                  exponentem.\n"
  "                                  sigdig: Presnost zadana jako pocet\n"
  "                                          platnych cifer.\n"
  "                                  a: Realny exponent.\n"
  "         proj2 --arctg sigdig     Vypocet arcus tangens.\n"
  "                                  sigdig: Presnost zadana jako pocet\n"
  "                                          platnych cifer.\n"
  "         proj2 --argsinh sigdig   Vypocet argumentu hyperbolického sinu.\n"
  "                                  sigdig: Presnost zadana jako pocet\n"
  "                                          platnych cifer.\n"
  "----------------------------------------------------------------------------\n";



/*Jednotlive chyby, krere mohou v prubehu programu nastat.*/
enum {
  E_PARAMETR,
  E_EPS,
  E_ZERO,
};



/*Jednotliva chybova hlaseni, ktera se vazou k vyse uvedenemu vyuctovemu typu.*/
const char *ERROR[] = {
  "Spatne zadane parametry programu.",
  "Chybne zadana presnost vypoctu.",
  "Zadana presnost vypoctu je mensi nez 0.",
};



/*Funkcni prototypy funkci.*/
/*Hlavni funkce.*/
double vypocet_mocniny(double eps, double exponent, double mocnenec);
double arctg(double eps, double nactene_cislo);
double argsinh(double eps, double nactene_cislo);

/*Funkce na vypis hodnot.*/
void printfERR(int error);
void vypis_hodnot(double vysledek);

/*Pomocne funkce.*/
int podminky_eps(int eps, double *p_eps);
double vypocet_ln(double eps, double zaklad_log);
void nacteni_hodnot(double eps, double exponent, int typ_funkce);



/*Hlavni funkce.*/
int main(int argc, char **argv)
{
  int sigdig;/*Promenna pro ulozeni presnoti, zadane jako cele cislo.*/
  double exponent;/*Promenna pro ulozeni realneho exponentu.*/
  double eps;/*Promenna pro ulozeni presnosti jako desetinne cislo.*/
  int typ_funkce;

/*Podminky pro vyber spravne aproximacni funkce. */
/*Vypsani napovedy.*/
  if(argc == 2){
    if(!strcmp(argv[1], "-h")){
     printf("%s", HELP);
    }
    else{
      printfERR(E_PARAMETR);
    }
  }

/*Vypocet arcus tangens.*/
  else if(argc == 3){
    if(!strcmp(argv[1], "--arctg")){
      sscanf(argv[2], "%d", &sigdig);
      if(podminky_eps(sigdig, &eps) == 1){
        return 1;
      }

      typ_funkce = 2;
      nacteni_hodnot(eps, 0, typ_funkce);
    }

/*Vypocet argumentu hyperbolickeho sinu.*/
    else if (!strcmp(argv[1], "--argsinh")){
      sscanf(argv[2], "%d", &sigdig);
      podminky_eps(sigdig, &eps);
      typ_funkce = 3;
      nacteni_hodnot(eps, 0, typ_funkce);
    }

    else{
      printfERR(E_PARAMETR);
    }
  }

/*Vypocet mocnine funkce s realnym exponentem.*/
  else if(argc == 4){
    if(!strcmp(argv[1], "--powxa")){
      sscanf(argv[2], "%d", &sigdig);

      if(podminky_eps(sigdig, &eps) == 1){
        return 1;
      }

      sscanf(argv[3], "%lf", &exponent);
      typ_funkce = 1;
      nacteni_hodnot(eps, exponent, typ_funkce);

    }
    else{
      printfERR(E_PARAMETR);
    }
  }

  else{
    printfERR(E_PARAMETR);
  }

  return 0;
}




/*Jednotliva tela funkci.*/
/*Funkce pro vypisovani chybovych hlaseni.*/
void printfERR(int error)
{
  fprintf(stderr, "%s\n", ERROR[error]);
}



/*Funkce pro prevod sigdig na eps. Funkce prevedenou presnost na eps, ulozi *
 *pomoci ukazatele do promenne eps ve funkci main.                          */
int podminky_eps(int sigdig, double *p_eps)
{
  if(sigdig > DBL_DIG){/*DBL_DIG = Udava pocet platnych cifer mantisy.*/
    printfERR(E_EPS);
    return 1;
  }

/*Presnost mensi nez 0 nema smysl, proto ukoncime program s chybou.*/
  else if(sigdig <= 0){
    printfERR(E_ZERO);
    return 1;
  }

/*Prevod sigdig na eps, pomoci mocnine funkce pow z knihovny math.h.*/
  *p_eps = pow(DESETINNY_RAD, sigdig);

  return 0;
}



/*Funkce pro vypisovani vyslednych hodnot.*/
void vypis_hodnot(double vysledek)
{
  printf("%.10e\n", vysledek);
}



/*Funkce pro nacitani hodnoty mocnence pro vypocet mocniny.*/
void nacteni_hodnot(double eps, double exponent, int typ_funkce){
  int hodnota_sf;
  double nactene_cislo;
  double vysledek;

  while((hodnota_sf = scanf("%lf", &nactene_cislo)) != EOF){
/*Jestlize scanf nacte neco co neni cislo typu double, promenna se nastavi *
 *jako NAN a preskoci se tento znak pomoci scanfu na dalsi znak.           */
    if(hodnota_sf == 0){
      scanf("%*s");
      nactene_cislo = NAN;
    }

    if(typ_funkce == 1){
      vysledek = vypocet_mocniny(eps, exponent, nactene_cislo);
      vypis_hodnot(vysledek);
    }

    else if(typ_funkce == 2){
      vysledek = arctg(eps, nactene_cislo);
      vypis_hodnot(vysledek);
    }

    else if(typ_funkce == 3){
      vysledek = argsinh(eps, nactene_cislo);
      vypis_hodnot(vysledek);
    }
  }
}



/*Funkce pro vypocet mocniny s realnym exponentem.*/
double vypocet_mocniny(double eps, double exponent, double mocnenec)
{
  int faktorial;/*Promenna pro ulozeni faktorialu, nastavena na 1, kvuli *
                     *prvnimu clenu Taylerovi rady pro vypocet mocniny.      */
  double vysledek;/*Promenna pro konecny vysledek.*/
  double logaritmus;/*Promenna pro vypocet logaritmu.*/
  double prirustek;/*Promenna pro ulozeni hodnoty konkretniho clenu *
                          *Taylerova rozvoje.                             */
  int prevod_kladne = 0;

/*Podminka pro zrychleni vypoctu, pokud je x = 0; tak rovnou tiskni *
 *vysledek a preskoc cucklus while.                                 */
  if(mocnenec == 0.0){
    vysledek = 0.0;
    return vysledek;
  }

  if(isnan(mocnenec)){
    return NAN;
  }

  if(mocnenec < 1e-10){
    mocnenec = 1.0 / mocnenec;
    prevod_kladne = 1;
  }
  logaritmus = vypocet_ln(eps, mocnenec);/*Predpripraveni hodnoty logaritmu.*/
  /*Nastaveni hodnot jednotlivych promennych. Kvuli podobe vzorce pro vypocet *
   *mocniny pomoci Taylerovi rady.                                            */
    prirustek = 1.0;
    vysledek = 1.0;
    faktorial = 1;

    while(fabs(prirustek) > fabs(eps * vysledek)){
      prirustek = prirustek * (exponent * logaritmus) / (faktorial);
      vysledek = vysledek + prirustek;
      faktorial++;

      if(isinf(fabs(vysledek))){
        return INFINITY;
      }
    }

  if(prevod_kladne == 1){
    vysledek = 1 / vysledek;
  }

  return vysledek;
}



/*Funkce pro vypocet arctg s presnoti eps.*/
double arctg(double eps, double nactene_cislo)
{
  double prirustek;/*Promenna pro ulozeni jednoho clenu Taylerova rozvoje.*/
  double vysledek;/*Promenan pro ulozeni konecneho vysledku a mezi souctu *
                   *clenu Taylerova rozvoje.                              */
  int i;
  int zaporne;/*Promenna pro nastaveni zda nactene cislo je zaporne.*/
  int vetsi;/*Promenna pro nastaveni zna nactene cislo je vetsi nez 1.*/
  int vetsi2;/*Promenna pro nasvaeni zda nactene cislo je vetsi *
              *nez (2 - odmocnina ze 3).                          */
  //int hodnota;/*Promenna pro ulozeni  hodnoty, kterou vraci funkce scanf.*/
  double mocnina;/*Promenna pro vypocet n-te mocniny.*/


    zaporne = 0;
    vetsi = 0;
    vetsi2 = 0;

/*Jestlize na vstup posleme NAN, jako vysledek dostaneme take NAN.*/
    if(isnan(nactene_cislo)){
      return NAN;
    }

/*Jestlize na vstup posleme INF, jako vysledek dostaneme INF.*/
    if(isinf(fabs(nactene_cislo))){
      return INFINITY;
    }

/*Pokud je nactene cislo rovno 1, tak se jako vysledek nastavi rovnou *
 *PI/4 a preskoci se vypocet.                                         */
    if(nactene_cislo == 1.0){
      vysledek = IZP_PI_4;
      return vysledek;
    }

/*Pokud je nactene cislo rovno -1, tak se jako vysledek nastavi rovnou *
 *-PI/4 a preskoci se vypocet.                                         */
    if(nactene_cislo == -1.0){
      vysledek = IZP_PI_4 * (-1);
      return vysledek;
    }

/*Pokud je nactenne cislo zaporne, tak se pomoci absolutni hodnoty z nej udela *
 *kladne cislo.                                                                */
    if(nactene_cislo < 0.0){
      nactene_cislo = fabs(nactene_cislo);
      zaporne = 1;
    }

/*Pokud je nactenne cislo vetsi nez 1, tak z nej udelame mensi cislo, pro *
 *zpresneni a zrychleni vypoctu.                                          */
    if(nactene_cislo > 1.0){
      nactene_cislo = (1 / nactene_cislo);
      vetsi = 1;
    }

/*Pokud je cislo vetsi nez cca. 0.2679, tak jej jeste zmensime, pro dosazeni *
 *vetsi presnosti a rychlosti vypoctu.                                       */
    if(nactene_cislo > (2 - ODMOCNINA)){
      nactene_cislo = ((ODMOCNINA * nactene_cislo - 1) / (ODMOCNINA + nactene_cislo));
      vetsi2 = 1;
    }

/*Nastaveni hodnot promennych, tak aby vypocet odpovidal  *
 *Taylerovu rozvoji pro arc tg.                           */
    mocnina = nactene_cislo;
    prirustek = 1.0;
    vysledek = nactene_cislo;
    i = 3;
    int minus = -1;

    while(fabs(prirustek) > fabs(eps * vysledek)){
      mocnina = mocnina * nactene_cislo* nactene_cislo;
      prirustek = minus * (mocnina / i);
      minus *= -1;
      vysledek = vysledek + prirustek;
      i = i + 2;
    }

/*Pokud jsme nastavili nejakou vlastno cisla, tak pred konecnym vypisem  *
 *vysledku, musime cislo jeste upravit do spravneho formatu.             */
    if(vetsi2 == 1){
      vysledek += (IZP_PI / 6);
    }

    if(vetsi == 1){
      vysledek = IZP_PI_2 - vysledek;
    }

    if(zaporne == 1){
      vysledek = vysledek * (-1);
    }

    return vysledek;
}



/*Pomocna funkce, ktera pocita ln, pro dalsi pouziti v jinych funkcich.*/
double vypocet_ln(double eps, double zaklad_log){
  double vysledek;/*Promenan pro ulozeni konecneho vysledku a mezi souctu *
                   *clenu Taylerova rozvoje.                              */
  double prirustek;/*Promenna pro ulozeni jednoho clenu Taylerova rozvoje.*/
  int i = 1;
  int j = 0;/*Promenna pro ulozeni kolikrat jsme nactene cislo upravovali, *
         *pro co nejrychlejsi a nejpresnejsi vypocet.                  */
  double mocnina_p1; /*Promenna pro vypocet mocniny + 1.*/
  double mocnina_m1; /*Promenan pro vypocet mocniny - 1.*/

/*Zvetseni presnoti vypoctu logaritmu o tri rady.*/
  eps = eps * 0.001;

/*Jestlize na vstup posleme NAN, jako vysledek dostaneme take NAN.*/
  if (isnan(zaklad_log)){
    return NAN;
  }

/*Jestlize na vstup posleme INF, jako vysledek dostaneme INF.*/
  else if (isinf(fabs(zaklad_log))){
    return INFINITY;
  }

/*Jestlize na vstup posleme cislo mensi nez 0, tak vysledek bude NAN.*/
  else if (zaklad_log <= 0){
    return NAN;
  }

/*Jestlize na vstup posleme cislo 1, tak pro zrychleni vypoctu, vysledek bude 0.0.*/
  else if (zaklad_log == 1){
    return 0.0;
  }

/*Jestlize cislo na vstupu je vetsi nebo rovno IZP_E, tak jej musime uravit *
 *aby jsme ziskali mensi cislo a tim presnejsi vypocet.                     */
  while(zaklad_log >= IZP_E){
    zaklad_log = zaklad_log / IZP_E;
    j++;
  }

/*Jestlize cislo na vstupu je vetsi nez 0 a mensi nebo rovno IZP_E, tak jej *
 *musime uravit, aby jsme ziskali vetsi cislo a tim presnejsi vypocet.      */
  while((zaklad_log > 0) && (zaklad_log <= IZP_E)){
    zaklad_log = zaklad_log * IZP_E;
    j--;
  }

/*Vypocet prvniho clenu Taylerova rozvoje pro ln.*/
  vysledek = (zaklad_log - 1) / (zaklad_log + 1);
  prirustek = vysledek;

/*Vypocet mocniny, pred pouzitim v iteracnim cyklu.*/
  mocnina_m1 = ((zaklad_log - 1) * (zaklad_log - 1));
  mocnina_p1 = ((zaklad_log + 1) * (zaklad_log + 1));

/*Zapoceti iteracnich vypoctu.*/
  while(fabs(prirustek) > fabs(eps * vysledek)){
    prirustek = prirustek * ((mocnina_m1 * i) / ((i + 2) * mocnina_p1));
    i += 2;
    vysledek = vysledek + prirustek;
    }

/*Vysledek vynasobime 2, dle vzorce pro vypocet ln.*/
  vysledek = vysledek * 2;

/*Pokud jsme cislo upravovali (zvetsovali a nebo zmensovali),    *
 *musime pricist cislo, ktere nam rika, kolikrat jsme upravovali.*/
  if(j != 0){
    vysledek = vysledek + j;
  }

  return vysledek;
}



/*Funkce pro vypocet argumentu hyperbolickeho sinu.*/
double argsinh(double eps, double nactene_cislo){
  double vysledek;/*Promenna pro ulozeni vysledneho vysledku.*/
  double prirustek;
  double pod_odmocninou;

/*Jestlize na vstup posleme NAN, jako vysledek dostaneme take NAN.*/
  if (isnan(nactene_cislo)){
    return NAN;
  }

/*Jestlize na vstup posleme INF, jako vysledek dostaneme INF.*/
  else if (isinf(fabs(nactene_cislo))){
    return INFINITY;
  }

    else{
      pod_odmocninou = (nactene_cislo * nactene_cislo) + 1;
      prirustek = vypocet_mocniny(eps, ODMOCNINA_2, pod_odmocninou);
      vysledek = vypocet_ln(eps, prirustek + nactene_cislo);
      return vysledek;
    }
  }
