#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int sum = 0;
void print(struct dirent *file, char *path, int l, int s, int in2)
{
        int color = 37;

        char path2[100] = {0};
        for (int i = 0; i < strlen(path); i++)
        {
                path2[i] = path[i];
        }

        struct stat buf;
        char inf[11] = "----------";
        //获取工作路径

        getcwd(path, sizeof(path));

        //合成文件绝对路径
        if (path2 != "./")
                strcat(path2, "/");
        strcat(path2, file->d_name);

        stat(path2, &buf);
        struct passwd *user;
        struct group *group;

        if (S_ISLNK(buf.st_mode))
        { //符号链接
                inf[0] = 'l';
                color = 31;
        }
        else if (S_ISDIR(buf.st_mode))
        { //目录文件
                inf[0] = 'd';
                color = 35;
        }
        else if (S_ISCHR(buf.st_mode))
        { //字符设备文件
                inf[0] = 'c';
                color = 32;
        }
        else if (S_ISBLK(buf.st_mode))
        { //块设备文件
                inf[0] = 'b';
                color = 33;
        }
        else if (S_ISFIFO(buf.st_mode))
        { //先进先出文件
                inf[0] = 'f';
                color = 31;
        }
        else if (S_ISSOCK(buf.st_mode))
        { // socket
                inf[0] = 's';
                color = 32;
        }

        char *p = file->d_name;
        for (int i = 0; i <= strlen(file->d_name); i++)
        {
                if (*p == '.')
                {
                        p++;
                        if (*p == 'c')
                        {
                                color = 34;
                                break;
                        }
                        if (*p == 't')
                        {
                                p++;
                                if (*p == 'x')
                                {
                                        p++;
                                        if (*p == 't')
                                        {
                                                color = 36;
                                                break;
                                        }
                                        p--;
                                }
                                p--;
                        }
                }
                p++;
        }

        if (in2)
        {
                printf("%ld  ", buf.st_ino);
        }
        if (s)
        {
                sum += buf.st_blocks / 2;
                printf("%-4d ", buf.st_blocks / 2);
        }
        if (l)
        {

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

                printf("%s  ", inf);

                user = getpwuid(buf.st_uid);
                group = getgrgid(buf.st_gid);

                printf("%4ld ", buf.st_nlink);   //硬链接数
                printf("%-8s  ", user->pw_name); //用户名字
                printf("%-8s", group->gr_name);  //用户组名字
                printf("%6ld", buf.st_size);     //文件大小

                struct tm *t = localtime(&buf.st_ctim.tv_sec);

                printf("%2d月%2d %02d:%02d  ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);

                printf("\033[%dm%s\n\033[0m", color, file->d_name);
        }
        else
        {

                printf("\033[%dm%s    \033[0m", color, file->d_name);
        }
        return;
}
void ls(char *path, int R, int a, int l, int s, int inf)
{

        DIR *dir;
        struct dirent *p;
        struct dirent *file[50];
        dir = opendir(path);

        int n = 0; //文件总数

        struct stat buf;

        char *ptr;

        for (int i = 0; (p = readdir(dir)) != NULL; i++)
        {
                file[i] = p;
                n++;
                if (R)
                {
                        char path2[100] = {0};
                        for (int i = 0; i < strlen(path); i++)
                        {
                                path2[i] = path[i];
                        }

                        //获取工作路径

                        getcwd(path, sizeof(path));

                        //合成文件绝对路径
                        if (path2 != "./" && strlen(path2) > 3)
                        {
                                strcat(path2, "/");
                        }
                        strcat(path2, p->d_name);

                        stat(path2, &buf);

                        if (S_ISDIR(buf.st_mode) && *p->d_name != '.')
                        {
                                ls(path2, R, a, l, s, inf);
                        }
                }
        }

        //排序_正常
        for (int i = 0; i < n - 1; i++)
        {

                for (int j = i; j < n - i - 1; j++)
                {

                        if (*file[j]->d_name > *file[j + 1]->d_name)
                        {
                                struct dirent *t = file[j];
                                file[j] = file[j + 1];
                                file[j + 1] = t;
                        }
                }
        }

        if (R)
        {
                printf("%s:\n", path);
        }
        for (int i = 0; i < n; i++)
        {
                if (a == 0 && *file[i]->d_name == '.')
                {
                        continue;
                }
                print(file[i], path, l, s, inf);
        }

        if (s)
        {
                printf("\n总用量：%d", sum);
        }
        printf("\n");
        closedir(dir);
        return;
}
int main(int argc, char *argv[])
{

        char path[100] = "./\0";
        char *p;

        int R = 0;
        int a = 0;
        int l = 0;
        int s = 0;
        int inf = 0;

        printf("\n");

        //判断ls
        if (argc > 1 || argv[1] == "ls")
        {

                //判断每项参数
                for (int i = 2; i < argc; i++)
                {
                        p = argv[i];
                        // ls 功能
                        if (*p == '-')
                        {
                                for (int j = 0; j <= strlen(argv[i]); j++)
                                {
                                        if (*p == 'R')
                                        {
                                                R = 1;
                                        }
                                        if (*p == 'a')
                                        {
                                                a = 1;
                                        }
                                        if (*p == 'l')
                                        {
                                                l = 1;
                                        }
                                        if (*p == 's')
                                        {
                                                s = 1;
                                        }
                                        if (*p == 'i')
                                        {
                                                inf = 1;
                                        }
                                        p++;
                                }
                        }
                        // ls 路径
                        if ((*p == '.' && *(p + 1) == '/') || *p == '/')
                        {
                                for (int j = 0; j <= strlen(argv[i]); j++)
                                {
                                        path[j] = *p;
                                        p++;
                                }
                        }
                }
                ls(path, R, a, l, s, inf);
        }

        printf("\n");

        return 0;
}