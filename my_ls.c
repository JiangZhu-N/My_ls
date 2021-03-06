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

void ls(int oh, int infor, int ino, int sum);
void Get_mode(struct dirent *file, int sum);
void fino(struct dirent *file);
void fsum(int sum);

int main(int argc, char *argv[])
{
    char *p;
    int openhide = 0;
    int linformation = 0;
    int ino = 0;
    int sum = 0;

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
                    p++;
                }
            }
        }
        ls(openhide, linformation, ino, sum);
    }

    return 0;
}

void ls(int oh, int infor, int ino, int sum)
{
    DIR *dir;
    struct dirent *ptr;
    char hide = '.';
    int openhide = oh; /////////判断-a

    dir = opendir("./"); ////查询的路径
    if (openhide)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            if (ino)
                fino(ptr);
            if (infor)
                Get_mode(ptr, sum);
            printf("%s\n", ptr->d_name);
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
                    Get_mode(ptr, sum);
                printf("%s\n", ptr->d_name);
            }
        }
    }
    closedir(dir);

    return;
}
void Get_mode(struct dirent *file, int sum)
{

    // struct tm *t;
    struct stat buf;
    char inf[11] = "----------";
    //获取工作路径
    char path[100];
    getcwd(path, sizeof(path));

    //合成文件绝对路径
    strcat(path, "/");
    strcat(path, file->d_name);
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

    if (sum)
        printf("%4ld ", buf.st_blocks / 2);

    printf("%s  ", inf);

    user = getpwuid(buf.st_uid);
    group = getgrgid(buf.st_gid);

    printf("%4ld ", buf.st_nlink);   //硬链接数
    printf("%-8s  ", user->pw_name); //用户名字
    printf("%-8s", group->gr_name);  //用户组名字
    printf("%6ld", buf.st_size);     //文件大小

    struct tm *t = localtime(&buf.st_ctim.tv_sec);

    printf("%2d月%2d %02d:%02d  ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

    return;
}
void fino(struct dirent *file)
{
    // printf("!!!!!!!!!!!!!!!!!!");
    struct stat buf;

    //获取工作路径
    char path[100];
    getcwd(path, sizeof(path));

    //合成文件绝对路径
    strcat(path, "/");
    strcat(path, file->d_name);
    // printf("%s\n", path);

    stat(path, &buf);
    printf("%8ld  ", buf.st_ino);
    return;
}
