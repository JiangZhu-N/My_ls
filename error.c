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

void ls(int oh, int infor, int allfile, char *path);
int Get_mode(struct dirent *file, int infor,char* npath);
void sort(struct dirent *file);

int main(int argc, char *argv[])
{
    char path[100] = "./";
    char *p;
    int openhide = 0;
    int linformation = 0;
    int allfile = 0;
    // printf("\n%d\n%s\n\n", argc, argv[1]);
    if (argc > 1 || argv[1] == "ls")
    {

        for (int i = 2; i < argc; i++)
        {
            p = argv[i];

            if (*p == '-')
            {
                for (int j = 0; j <= sizeof(argv[i]) / 4; j++)
                {
                    p++;
                    if (*p == 'a')
                    {
                        openhide = 1;
                    }
                    if (*p == 'l')
                    {
                        linformation = 1;
                    }
                    if (*p == 'R')
                    {
                        allfile = 1;
                    }
                }
            }
        }
        ls(openhide, linformation, allfile, path);
    }

    return 0;
}

void ls(int oh, int infor, int allfile, char *path)
{

    DIR *dir;
    struct dirent *ptr;
    char hide = '.';
    int openhide = oh; /////////判断-a

    dir = opendir(path); ////查询的路径
    if (openhide)
    {
        while ((ptr = readdir(dir)) != NULL)
        {

            if (Get_mode(ptr, infor,path) && allfile == 1 && *(ptr->d_name) != '.')
            {
                
                strcat(path, ptr->d_name);
                strcat(path, "/");
                //DIR *ndir=opendir(path);
                //struct dirent *nptr=readdir(ndir);
                //ptr=nptr;
                ls(oh, infor, allfile, path);
            }
        }
    }
    else
    {
        while ((ptr = readdir(dir)) != NULL)
        {

            if (*(ptr->d_name) != hide)
            {
                if (Get_mode(ptr, infor,path) && allfile == 1 && *(ptr->d_name) != '.')
                {
                    ls(oh, infor, allfile, strcat(path, ptr->d_name));
                }
            }
        }
    }
    closedir(dir);

    return;
}
int Get_mode(struct dirent *file, int infor,char* npath)
{
    int menu = 0;

    struct stat buf;
    char inf[11] = "----------";
    //获取工作路径
    char path[100];
    getcwd(path, sizeof(path));

    //合成文件绝对路径
    strcat(path, "/");
    strcat(path, npath);
    // printf("%s\n", path);

    stat(path, &buf);

    struct passwd *user;
    struct group *group;

    if (S_ISLNK(buf.st_mode))
    { //符号链接
        inf[0] = 'l';
    }
    else if (S_ISDIR(buf.st_mode))
    { //目录文件
        inf[0] = 'd';
        menu = 1;
    }
    else if (S_ISCHR(buf.st_mode))
    { //字符设备文件
        inf[0] = 'c';
    }
    else if (S_ISBLK(buf.st_mode))
    { //块设备文件
        inf[0] = 'b';
    }
    else if (S_ISFIFO(buf.st_mode))
    { //先进先出文件
        inf[0] = 'f';
    }
    else if (S_ISSOCK(buf.st_mode))
    { // socket
        inf[0] = 's';
    }

    //拥有者权限
    if (buf.st_mode & S_IRUSR)
        inf[1] = 'r';
    if (buf.st_mode & S_IWUSR)
        inf[2] = 'w';
    if (buf.st_mode & S_IXUSR)
        inf[3] = 'x';
    //组权限
    if (buf.st_mode & S_IRGRP)
        inf[4] = 'r';
    if (buf.st_mode & S_IWGRP)
        inf[5] = 'w';
    if (buf.st_mode & S_IXGRP)
        inf[6] = 'x';
    //其他用户权限
    if (buf.st_mode & S_IROTH)
        inf[7] = 'r';
    if (buf.st_mode & S_IWOTH)
        inf[8] = 'w';
    if (buf.st_mode & S_IXOTH)
        inf[9] = 'x';

    ////////////

    user = getpwuid(buf.st_uid);
    group = getgrgid(buf.st_gid);
    if (infor)
    {
        printf("%s  ", inf);
        printf("%2d  ", buf.st_nlink);   //硬链接数
        printf("%8s  ", user->pw_name);  //用户名字
        printf("%8s  ", group->gr_name); //用户组名字
        printf("%6d  ", buf.st_size);    //文件大小

        struct tm *t = localtime(&buf.st_ctim.tv_sec);

        printf("%2d月 %2d  %02d:%02d  ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    }
    printf("%s\n", file->d_name);
    return menu;
}
void sort(struct dirent *file)
{
}
