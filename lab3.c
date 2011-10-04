#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct cell{

        struct cell * prev;
        void * value;
        struct cell * next;

}cell;

typedef struct {

        cell * head;
        cell * tail;

}head_tail;

head_tail create_list(void){

        head_tail h_t;

        h_t.head = (cell *)calloc(1, sizeof(cell));
        h_t.head->next = (cell *)calloc(1, sizeof(cell));

        h_t.tail = h_t.head->next;

        h_t.tail->prev = h_t.head;

        return h_t;

}

void insert(void * value, cell * pos, head_tail * h_t){

        cell * ny_cell;

        ny_cell = (cell *)calloc(1, sizeof(cell));

        ny_cell->next = pos;

        ny_cell->prev = pos->prev;

        pos->prev->next = ny_cell;

        pos->prev = ny_cell;

        pos->prev->value = value;

}

cell * first(head_tail * h_t){

        if (h_t->tail->prev != h_t->head){
                return h_t->head->next;
        }

        return h_t->tail;
}

cell * remove_cell(cell * pos, head_tail * h_t){

        cell * temp;

        pos->next->prev = pos->prev;

        pos->prev->next = pos->next;

        temp = pos->next;

        free (pos);

        return temp;

}

void * inspect(cell * pos, head_tail * h_t){

        return pos->value;

}

/*  lägger alla mappar den ska besöka i en lista
    insert(dir->d_name, first(&h_t), &h_t);
    str = inspect(first(&h_t), &h_t);
    printf("%s\n", str);
*/

 int search(char c, char * searchString){

    DIR * d;
    struct dirent *dir;
	struct stat	buf;
    char currentPath[4096];


    head_tail h_t;
    char * str;
    h_t = create_list();


    if (getcwd(currentPath, sizeof(currentPath)) == NULL)
        printf("getcwd failed\n");

    if (chdir("test") < 0)
        printf("chdir failed\n");

    d = opendir(".");

    while ((dir = readdir(d)) != NULL){

        stat(dir->d_name, &buf);


        switch (c){

            /* Är den aktuella filen en mapp anropa search rekursivt return 0
               när det inte finns några mappa att gå till */
            case 'd':
                if (S_ISDIR(buf.st_mode)){
                    if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                        if (!strcmp (dir->d_name, searchString)){
                            printf("%s/%s\n",currentPath, dir->d_name);
                        }
                    }
                }
                break;

            case 'f':
                if (S_ISREG(buf.st_mode)){
                    if (!strcmp (dir->d_name, searchString)){
                        printf("%s/%s\n",currentPath, dir->d_name);
                    }
                }
                break;

            case 'l':
                if (S_ISLNK(buf.st_mode)){
                    if (!strcmp (dir->d_name, searchString)){
                        printf("%s/%s\n",currentPath, dir->d_name);
                    }
                }
                break;

            case 'a':
                if (S_ISDIR(buf.st_mode) || S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)){
                    if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
                        if (!strcmp (dir->d_name, searchString)){
                            printf("%s/%s\n",currentPath, dir->d_name);
                        }
                    }
                }
                break;

            default:
                printf("Case character doesn't exist");
                break;
        }
    }

    closedir(d);
 }

int getFileIndex(int argc, char **argv){

    int i;

    for (i = optind; i < argc; i++){

        if (i == (argc -1)){
            return i;
        }
    }
}

 int main (int argc, char **argv) {

   int c;
   int fileIndex;
   char * searchString;
   int flagExist = 0;

   opterr = 0;
   searchString = strdup(argv[getFileIndex(argc, argv)]);

    while ((c = getopt (argc, argv, "t:")) != -1){

        if (c == 't'){

            flagExist = 1;
            search(optarg[0], searchString);
/*
            switch (optarg[0]){
                    case 'd':
                        search('d', searchString);
                        break;

                    case 'f':
                        search('f', searchString);
                        break;

                    case 'l':
                        search('l', searchString);
                        break;

                    default:
                        printf("The file type doesn,t exist\n");
                        break;
                }
*/
        }
        else{
            printf("The flag doesn't exist!");
        }
    }

    if (!flagExist){
        printf("Du har inte angett någon flagga\n");
        search('a', searchString);
    }

/*
    for (index = optind; index < argc; index++){

        if (index <= (argc -2)){
            printf ("Paths: %s\n", argv[index]);
        }

        else if (index == (argc -1)){
            printf ("File: %s\n", argv[index]);
        }
    }
*/
    return 0;
}
