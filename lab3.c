/*
 * Namn: Daniel Evertsson
 * Lab3 Systemnärprogrammering 2011
 *
 * Programmet tar in en eller flera sökvägar och ett fil namn och lista alla
 * sökvägar till filen med matchande namn. Eventuellt kan en flagga -t med
 * tillhörade argument anges för att bergänsa sökningen till vissa fil typer
 * -t d där d enbart är kataloger.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

/*  Funktionen söker efter en fil (mapp, vanlig fil respektive länk).
    Sökningen startar i den mapp som angivits som argument. Sökningen
    genomförs med hjälp av rekursiva anrop.

    Parametrar:
        c:
            c motsvarar den bokstav som angivits som argument till flaggan -t.
        searchString:
            Namnet på filen man söker.
        path:
            en stäng som innehåller sökvägen till den mapp i vilken sökningen
            ska börja. */
 int search(char c, char * searchString, char * path){

    DIR * d;
    struct dirent *dir;
	struct stat	buf;
    char currentPath[4096];

    /* Byter current working direcory*/
    if (chdir(path) < 0){
        fprintf(stderr, "Failed to change dir");
        return 0;
    }

    /* öppnar mappen man står i (path) */
    if ((d = opendir(".")) == NULL){
        fprintf(stderr, "Failed to open dir");
        return 0;
    }


    while ((dir = readdir(d)) != NULL){

        /* Läser in info om den aktuella filen */
        if (lstat(dir->d_name, &buf) < 0){
            fprintf(stderr, "Failed to do lstat");
        }

        /* kontrollerar om det finns en mapp som går att öppna om det finns
           så anropar den sig själv rekursivt med sökvägen till amppen den
           hittade */
        if (S_ISDIR(buf.st_mode)){
            if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){

                if (chdir(dir->d_name) >= 0){
                        if (getcwd(currentPath, sizeof(currentPath)) == NULL){
                            fprintf(stderr, "Failed to do getcwd");
                        }

                        if (search(c, searchString, currentPath) == NULL){
                            fprintf(stderr, "Failed to do recursion");
                        }

                        if (chdir(path) < 0){
                            fprintf(stderr, "Chdir failed");
                        }
                }
            }
        }

        /* Kotrollerar vad den ska sök efter med hjälp av c*/
        switch (c){

            case 'd':
                if (S_ISDIR(buf.st_mode)){
                    if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                        if (!strcmp (dir->d_name, searchString)){
                            printf("%s/%s\n",path, dir->d_name);
                        }
                    }
                }
                break;

            case 'f':
                if (S_ISREG(buf.st_mode)){
                    if (!strcmp (dir->d_name, searchString)){
                        printf("%s/%s\n",path, dir->d_name);
                    }
                }
                break;

            case 'l':
                if (S_ISLNK(buf.st_mode)){
                    if (!strcmp (dir->d_name, searchString)){
                        printf("%s/%s\n",path, dir->d_name);
                    }
                }
                break;

            case 'a':
                if (S_ISDIR(buf.st_mode) || S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)){
                    if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                        if (!strcmp (dir->d_name, searchString)){
                           printf("%s/%s\n",path, dir->d_name);
                        }
                    }
                }
                break;

            default:
                fprintf(stderr, "The flags argument doesn't exist");
                break;
        }
    }

    closedir(d);
    return 1;
 }

/* beräknas index på det sista argumentet som skickats i från kommandoraden */
int getFileIndex(int argc, char **argv){

    int i;

    for (i = optind; i < argc; i++){

        if (i == (argc -1)){
            return i;
        }
    }
    return 0;
}

/* ./mfind [-t d|f|l] start1 [start2 ...] name */
int main (int argc, char **argv) {

    int c;
    char * searchString;
    int flagExist = 0;
    char * path;
    int i;

    opterr = 0;

    searchString = strdup(argv[getFileIndex(argc, argv)]);

    if (!strcmp(searchString , argv[0])){
        fprintf(stderr, "Did not find any search string");
    }

    /*  Loopar igenom alla sökvägar som skickats i som argument från
        kommandoraden*/
    for(i = 1; i < getFileIndex(argc, argv); i++){

        /* kontrollerar om användaren har angett någron flagga*/
        while ((c = getopt (argc, argv, "t:")) != -1){

            path = argv[i - 1];

            if (c == 't'){

                flagExist = 1;
                search(optarg[0], searchString, path);

            }
            else{
                fprintf(stderr, "The flag doesn't exist!");
            }
        }

        /*  om ingen flagga har angetts så ska programmet söka efter alla
            fil typer */
        if (!flagExist){
            path = argv[i];
            search('a', searchString, path);
        }
    }

    free (searchString);
    return 1;
}
