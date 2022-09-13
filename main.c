/*
    [TASK 1]

    MAKE DIRECTORY "The Witcher"
        MAKE DIRECTORY "Geralt"
            MAKE "Geralt.txt" FILE OF SIZE 1 KB FILLED WITH ZEROS
        MAKE DIRECTORY "Zoltan & Dandelion"
            MAKE "Zoltan.txt" FILE WITH ZOLTAN'S QUOTE
            MAKE "Dandelion.txt" FILE WITH DANDELIONS'S QUOTE
        MAKE DIRECTORY "Triss & Yennifer"
            MAKE "Yennifer.txt" AS HARD LINK ON "Geralt.txt"
            MAKE "Triss.txt" AS SYMBOLIC LINK ON "Geralt.txt"
    
    [TASK 2]
    
    DELETE DIRECTORY "The Witcher"
*/

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define KBYTE 1024

int write_file(const char* filename, const char* buf) // create filename and write buf
{
    int fd = open(filename, O_CREAT | O_RDWR, S_IRWXU);
    if(fd == -1)
    {
        fprintf(stderr, "[!] Failed to create file [%s] : %s\n", filename, strerror(errno));
        exit(-1);
    }
    write(fd, buf, strlen(buf));
    close(fd);
    printf("[%s] created\n", filename);
    return 0;
}

int truncate_file(const char* filename, int size) // create filename of size bytes
{
    int fd = open(filename, O_CREAT | O_RDWR, S_IRWXU);
    if(fd == -1)
    {
        fprintf(stderr, "[!] Failed to create file [%s] : %s\n", filename, strerror(errno));
        exit(-1);   
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
    {
        fprintf(stderr, "[!] Failed to create hard link [%s] on file [%s] : %s\n", filename, path, strerror(errno));
        exit(-1);
    }
    printf("Hard link [%s] on file [%s] created \n", filename, path);
    return 0;
}

int create_symlink(const char* path, const char* filename) // create symbolic link filaname on file path
{
    int n = symlink(path, filename);
    if(n == -1)
    {
        fprintf(stderr, "[!] Failed to create symbolic link [%s] on file [%s] : %s\n", filename, path, strerror(errno));
        exit(-1);
    }
    printf("Symbolic link [%s] on file [%s] created \n", filename, path);
    return 0;
}

int ifdir(const struct dirent *info) // filter for directories (besides for "../" & "./")
{
    struct stat buf;
    int str = stat(info->d_name, &buf);
    if (str == -1)
    {   
        fprintf(stderr, "[!] Failed to get file [%s] attributes : %s\n", info->d_name, strerror(errno));
        exit(-1);
    }
    if(strcmp(info->d_name, "..") && strcmp(info->d_name, "."))
    {
        if (buf.st_mode & S_IFDIR)
            return 1;
    }
    return 0;
}

int iffile(const struct dirent *info) // filter for filess
{
    struct stat buf;
    int str = stat(info->d_name, &buf);
    if (str == -1)
    {   
        fprintf(stderr, "[!] Failed to get file [%s] attributes : %s\n", info->d_name, strerror(errno));
        exit(-1);
    }
    if (buf.st_mode & S_IFREG)
        return 1;
    return 0;
}

int remove_dir(const char *path) // delete directory
{
    chdir(path);
    struct dirent **namelist;
    int n = scandir(".", &namelist, ifdir, alphasort);
    if (n == -1)
    {
        fprintf(stderr, "[!] Failed scan directory [%s] : %s\n", path, strerror(errno));
        exit(-1);
    }
    while (n--)
    {   
        remove_dir(namelist[n]->d_name);
        free(namelist[n]);
    }
    free(namelist);
    
    chdir(path);
    n = scandir(".", &namelist, iffile, alphasort);
    if (n == -1)
    {
        fprintf(stderr, "[!] Failed scan directory [%s] : %s\n", path, strerror(errno));
        exit(-1);
    }
    while (n--)
    {   
        if(unlink(namelist[n]->d_name) == -1)
        {
            fprintf(stderr, "[!] Failed to delete file [%s] : %s\n", namelist[n]->d_name, strerror(errno));
            exit(-1);
        }
        printf("Erased file [%s]\n", namelist[n]->d_name);
        free(namelist[n]);
    }
    free(namelist);
    
    chdir("../");
    if(rmdir(path) == -1)
    {
        fprintf(stderr, "[!] Failed to delete folder [%s] : %s\n", path, strerror(errno));
        exit(-1);
    }
    printf("Erased folder [%s]\n", path);
    return 0;
}

void create(const char *path)
{
    mkdir(path, S_IRWXU);
    chdir(path);
    mkdir("Geralt", S_IRWXU);
    mkdir("Zoltan & Dandelion", S_IRWXU);
    mkdir("Triss & Yennifer", S_IRWXU);

    chdir("Geralt");
    truncate_file("Geralt.txt", KBYTE);
    char geralt_path[PATH_MAX];
    getcwd(geralt_path, PATH_MAX);
    strcat(geralt_path, "/Geralt.txt");

    chdir("../Zoltan & Dandelion");
    write_file("Zoltan.txt", "Я думал, тут ждут меня: горячий окорок и холодное пиво, a тут жопа!");
    write_file("Dandelion.txt", "Людям для жизни необходимы три вещи: еда, питье и сплетни.");

    chdir("../Triss & Yennifer");
    create_hardlink(geralt_path, "Yennifer.txt");
    create_symlink(geralt_path, "Triss.txt");
    
    chdir("../");
}

int main(int argc, char *argv[])
{
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    if(argc > 1)
    {
        if(opendir(argv[1]) == NULL)
        {
            fprintf(stderr, "[!] Failed to open directory [%s] : %s\n", argv[1], strerror(errno));
            exit(-1);
        }
        strcpy(path, argv[1]);
    }
    strcat(path, "/The Witcher");
    printf("CREATING DIRECTORY on [%s]\n\n", path);
    create(path);
    puts("\nPRESS ANY BUTTON TO CONTINUE");
    getchar();
    printf("REMOVING DIRECTORY [%s]\n\n", path);
    remove_dir(path);
    return 0;
}
