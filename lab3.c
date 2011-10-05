#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


 int search(char c, char * searchString, char * path){

    DIR * d;
    struct dirent *dir;
	struct stat	buf;
    char currentPath[4096];

    if (chdir(path) < 0)
        printf("chdir failed\n");

    if ((d = opendir(".")) == NULL){
        printf("Kan inte öppna sökvägen");
        return 0;
    }


    while ((dir = readdir(d)) != NULL){

        lstat(dir->d_name, &buf);


        if (S_ISDIR(buf.st_mode)){
            if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                if (chdir(dir->d_name) >= 0){
                        getcwd(currentPath, sizeof(currentPath));
                        search(c, searchString, currentPath);

                        if (chdir(path) < 0)
                            printf("chdir failed\n");
                }
            }
        }


        switch (c){

            /* Är den aktuella filen en mapp anropa search rekursivt return 0
               när det inte finns några mappa att gå till */
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
                printf("Flag argument doesn't exist");
                break;
        }
    }

    closedir(d);
    return 1;
 }

int getFileIndex(int argc, char **argv){

    int i;

    for (i = optind; i < argc; i++){

        if (i == (argc -1)){
            return i;
        }
    }
    return 0;
}


int getPathStartIndex(int argc, char **argv){

    int i = 0;
    int pathStartIndex = 0;

    for (i = optind; i < argc; i++){

        if (i <= (argc -2)){
            pathStartIndex = i;
        }
    }
    return pathStartIndex;
}


int main (int argc, char **argv) {

    int c;
    char * searchString;
    int flagExist = 0;
    char * path;
    int i;

    opterr = 0;
    searchString = strdup(argv[getFileIndex(argc, argv)]);

    for(i = 1; i < getFileIndex(argc, argv); i++){

        while ((c = getopt (argc, argv, "t:")) != -1){

            path = argv[i - 1];

            if (c == 't'){

                flagExist = 1;
                search(optarg[0], searchString, path);

            }
            else{
                printf("The flag doesn't exist!");
            }
        }

        if (!flagExist){
            path = argv[i];
            search('a', searchString, path);
        }
    }

    return 0;
}
