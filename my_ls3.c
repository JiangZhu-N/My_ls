#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

struct comp
{
    int year;
    int mouth;
    int day;
    int hour;
    int min;
    char path[100];
};

void ls(int oh, int infor, int ino, int sum, char *path);
void Get_mode(struct dirent *file, int sum, char *path);
void fino(struct dirent *file);
void fsum(int sum);
void sort(struct dirent *file, char *path, int sum);
struct comp Comp(struct dirent *file);

int main(int argc, char *argv[])
{
    char *p;
    int openhide = 0;
    int linformation = 0;
    int ino = 0;
    int sum = 0;
    int sort = 0;
    char path[100] = "./";
    if (argc > 1 || argv[1] == "ls")
    {
        for (int i = 2; i < argc; i++)
        {
            p = argv[i];

            if (*p == '-')
            {
                for (int j = 0; j <= sizeof(argv[i]) / 2; j++)
                {
                    if (*p == 'a')
                    {
                        openhide = 1;
                    }
                    if (*p == 'l')
                    {
                        linformation = 1;
                    }
                    if (*p == 'i')
                    {
                        ino = 1;
                    }
                    if (*p == 's')
                    {
                        sum = 1;
                    }
                    if (*p == 't')
                    {
                        sort = 1;
                    }
                    p++;
                }
            }
            if (*p == '/')
            {
                for (int j = 0; j <= strlen(argv[i]); j++)
                {
                    path[j] = *p;
                    p++;
                    // printf("\n@@@@@@@@@@@%d@@@@@@@@@@\n",strlen(argv[i]));
                }
            }
        }

        // printf("\n###########%s#########\n",path);
        ls(openhide, linformation, ino, sum, path);
        // sort()
    }

    return 0;
}

void ls(int oh, int infor, int ino, int sum, char *path)
{
    DIR *dir;
    struct dirent *ptr;
    char hide = '.';
    int openhide = oh; /////////??????-a

    dir = opendir(path); ////???????????????
    if (openhide)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            if (sort)
            {
            }
            else
            {
                if (ino)
                    fino(ptr);
                if (infor)
                    Get_mode(ptr, sum, path);
                printf("%s\n", ptr->d_name);
            }
        }
    }
    else
    {
        while ((ptr = readdir(dir)) != NULL)
        {

            if (*(ptr->d_name) != hide)
            {
                if (ino)
                    fino(ptr);
                if (infor)
                    Get_mode(ptr, sum, path);
                printf("%s\n", ptr->d_name);
            }
        }
    }
    closedir(dir);

    return;
}
void Get_mode(struct dirent *file, int sum, char *path)
{
    char path2[100] = {0};
    for (int i = 0; i < strlen(path); i++)
    {
        path2[i] = path[i];
    }
    // struct tm *t;
    struct stat buf;
    char inf[11] = "----------";
    //??????????????????

    getcwd(path, sizeof(path));
    // printf("\n\n##%s##\n",path);
    // printf("##%s##\n\n",path2);

    //????????????????????????
    if (path2 != "./")
        strcat(path2, "/");
    strcat(path2, file->d_name);
    // printf("%s\n", path);

    stat(path2, &buf);

    // printf("\n\n@@%s@@\n\n",path2);

    struct passwd *user;
    struct group *group;

    if (S_ISLNK(buf.st_mode))
    { //????????????
        inf[0] = 'l';
    }
    else if (S_ISDIR(buf.st_mode))
    { //????????????
        inf[0] = 'd';
    }
    else if (S_ISCHR(buf.st_mode))
    { //??????????????????
        inf[0] = 'c';
    }
    else if (S_ISBLK(buf.st_mode))
    { //???????????????
        inf[0] = 'b';
    }
    else if (S_ISFIFO(buf.st_mode))
    { //??????????????????
        inf[0] = 'f';
    }
    else if (S_ISSOCK(buf.st_mode))
    { // socket
        inf[0] = 's';
    }

    //???????????????
    if (buf.st_mode & S_IRUSR)
        inf[1] = 'r';
    if (buf.st_mode & S_IWUSR)
        inf[2] = 'w';
    if (buf.st_mode & S_IXUSR)
        inf[3] = 'x';
    //?????????
    if (buf.st_mode & S_IRGRP)
        inf[4] = 'r';
    if (buf.st_mode & S_IWGRP)
        inf[5] = 'w';
    if (buf.st_mode & S_IXGRP)
        inf[6] = 'x';
    //??????????????????
    if (buf.st_mode & S_IROTH)
        inf[7] = 'r';
    if (buf.st_mode & S_IWOTH)
        inf[8] = 'w';
    if (buf.st_mode & S_IXOTH)
        inf[9] = 'x';

    ////////////

    if (sum)
        printf("%4ld ", buf.st_blocks / 2);

    printf("%s  ", inf);

    user = getpwuid(buf.st_uid);
    group = getgrgid(buf.st_gid);

    printf("%4ld ", buf.st_nlink);   //????????????
    printf("%-8s  ", user->pw_name); //????????????
    printf("%-8s", group->gr_name);  //???????????????
    printf("%6ld", buf.st_size);     //????????????

    struct tm *t = localtime(&buf.st_ctim.tv_sec);

    printf("%2d???%2d %02d:%02d  ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

    return;
}
void fino(struct dirent *file)
{
    // printf("!!!!!!!!!!!!!!!!!!");
    struct stat buf;

    //??????????????????
    char path[100];
    getcwd(path, sizeof(path));

    //????????????????????????
    strcat(path, "/");
    strcat(path, file->d_name);
    // printf("%s\n", path);

    stat(path, &buf);
    printf("%8ld  ", buf.st_ino);
    return;
}
void sort(struct dirent *file, char *path, int sum)
{

    DIR *dir;
    struct dirent *ptr;
    char hide = '.';
    int openhide = 1; /////////??????-a
    struct comp cp[100];
    int i = 0;
    struct comp temp;

    dir = opendir(path); ////???????????????
    if (openhide)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            cp[i] = Comp(ptr);
            i++;
        }
    }
    else
    {
        while ((ptr = readdir(dir)) != NULL)
        {

            if (*(ptr->d_name) != hide)
            {
                cp[i] = Comp(ptr);
                i++;
            }
        }
    }
    closedir(dir);

    for (int j = 0; j < i; j++)
    {
        for (int k = i; k < i - j - 1; k++)
        {
            if (cp[k].year * 365 * 24 * 60 + cp[k].mouth * 30 * 24 * 60 + cp[k].day * 24 * 60 + cp[k].hour * 60 + cp[k].min > cp[k + 1].year * 365 * 24 * 60 + cp[k + 1].mouth * 30 * 24 * 60 + cp[k + 1].day * 24 * 60 + cp[k + 1].hour * 60 + cp[k + 1].min)
            {
                temp = cp[k];
                cp[k] = cp[k + 1];
                cp[k + 1] = temp;
            }
        }
    }
    for (int n = 0; n < i; n++)
    {
        Get_mode(file, sum, cp[n].path);
    }
    return;
}
struct comp Comp(struct dirent *file)
{

    struct stat buf;
    struct comp cp;
    //??????????????????
    char path[100];
    getcwd(path, sizeof(path));

    //????????????????????????
    strcat(path, "/");
    strcat(path, file->d_name);

    stat(path, &buf);
    struct tm *t = localtime(&buf.st_ctim.tv_sec);
    cp.year = t->tm_year;
    cp.mouth = t->tm_mon;
    cp.day = t->tm_mday;
    cp.hour = t->tm_hour;
    cp.min = t->tm_min;
    for (int i = 0; i < strlen(path); i++)
        cp.path[i] = path[i];
    return cp;
}
