#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 256
#define KBYTE 1024

int create_file(const char* filename, const char* buf) // create filename and write buf
{
    int fd = open(filename, O_CREAT | O_RDWR, S_IRWXU);
    if(fd == -1)
    {
        printf("Failed to create file [%s]\n", filename);
        return -1;
    }
    write(fd, buf, strlen(buf));
    close(fd);
    printf("[%s] created\n", filename);
    return 0;
}

int create_file(const char* filename, int size) // create filename of size bytes
{
    int fd = open(filename, O_CREAT | O_RDWR, S_IRWXU);
    if(fd == -1)
    {
        printf("Failed to create file [%s]\n", filename);
        return -1;    
    }
    ftruncate(fd, size);
    close(fd);
    printf("[%s] created\n", filename);
    return 0;
}

int create_hardlink(const char* path, const char* filename) // create hard link filaname on file path
{
    int n = link(path, filename);
    if(n == -1)
        return -1;
    printf("Hard link [%s] on file [%s] created \n", filename, path);
    return 0;
}

int create_symlink(const char* path, const char* filename) // create symbolic link filaname on file path
{
    int n = symlink(path, filename);
    if(n == -1)
        return -1;
    printf("Symbolic link [%s] on file [%s] created \n", filename, path);
    return 0;
}

int ifdir(const dirent *path) // filter for directories (besides for "../" & "./")
{
    struct stat buf;
    int str = stat(path->d_name, &buf);
    if (str == 0)
    {   
        if(strcmp(path->d_name, "..") && strcmp(path->d_name, "."))
        {
            if (buf.st_mode & S_IFDIR)
                return 1;
        }
    }
    return 0;
}

int iffile(const dirent *path) // filter for files
{
    struct stat buf;
    int str = stat(path->d_name, &buf);
    if (str == 0)
    {   
        if (buf.st_mode & S_IFDIR)
            return 0;
    }
    return 1;
}

int remove_dir(const char *path) // delete directory
{
    struct dirent **namelist;
    int n = scandir(path, &namelist, ifdir, alphasort);
    if (n == -1) 
        return -1;
    while (n--)
    {   
        char current[BUF_SIZE];
        strcpy(current, path);
        strcat(current, "/");   
        strcat(current, namelist[n]->d_name);
        remove_dir(current);
        free(namelist[n]);
    }
    free(namelist);
    
    n = scandir(path, &namelist, iffile, alphasort);
    if (n == -1) 
        return -1;
    while (n--)
    {   
        char current[BUF_SIZE];
        strcpy(current, path);
        strcat(current, "/");   
        strcat(current, namelist[n]->d_name);
        unlink(current);
        printf("Erased file %s\n", current);
        free(namelist[n]);
    }
    free(namelist);
    rmdir(path);
    printf("Erased folder %s\n", path);
    return 0;
}


/*
    TASK 1

    MAKE DIRECTORY "The Witcher"
        MAKE DIRECTORY "Geralt"
            MAKE "Geralt.txt" FILE OF SIZE 1 KB FILLED WITH ZEROS
        MAKE DIRECTORY "Zoltan & Dandelion"
            MAKE "Zoltan.txt" FILE WITH ZOLTAN'S QUOTE
            MAKE "Dandelion.txt" FILE WITH DANDELIONS'S QUOTE
        MAKE DIRECTORY "Triss & Yennifer"
            MAKE "Yennifer.txt" AS HARD LINK ON "Geralt.txt"
            MAKE "Triss.txt" AS SYMBOLIC LINK ON "Geralt.txt"

    TASK 2

    DELETE DIRECTORY "The Witcher"
*/


void create(const char *path)
{
    mkdir(path, S_IRWXU);
    chdir(path);
    mkdir("Geralt", S_IRWXU);
    mkdir("Zoltan & Dandelion", S_IRWXU);
    mkdir("Triss & Yennifer", S_IRWXU);

    chdir("Geralt");
    create_file("Geralt.txt", KBYTE);
    char geralt_path[BUF_SIZE];
    strcpy(geralt_path, path);
    strcat(geralt_path, "/Geralt/Geralt.txt");

    chdir("../Zoltan & Dandelion");
    create_file("Zoltan.txt", "Я думал, тут ждут меня: горячий окорок и холодное пиво, a тут жопа!");
    create_file("Dandelion.txt", "Людям для жизни необходимы три вещи: еда, питье и сплетни.");

    chdir("../Triss & Yennifer");
    create_hardlink(geralt_path, "Yennifer.txt");
    create_symlink(geralt_path, "Triss.txt");
    
    printf("\nFILES CREATED\n\n");
    chdir("../");
}

int main()
{
    const char *path = "/home/daniil/Desktop/The Witcher";
    create(path);
    printf("");
    int c = getchar();
    remove_dir(path);
    return 0;
}
