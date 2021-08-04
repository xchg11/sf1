// fire1024 (c) 2014
// sf1 program  failover....
////////////////////////
#define _GNU_SOURCE 2
#include "sf1.h"
#include "demon1.h"
//#define file_status1 "/var/run/sf1_status.lock"
// проверяем запущен ли fork reciver1000 сервер приема пакетов 
int check_daemon()
{
int status_pid=0;
status_pid=check_daemon_run();
if(status_pid==10)
{
FILE *f2;
char arr[10];
f2=fopen("/var/run/sf1.pid","r");while (fgets(arr,9, f2)!=NULL) printf("[ %s ] \033[1;33msf1 daemon running pid = %s\033[1;0m\n",gettime_tt(),arr);
fclose(f2);
}
return status_pid;
}
//
int main(int argc, char** argv)
{
int desc1;
int status;
int pid;
char *li_ip_addr;
int cnt_resync_status=0;
int status1,status_sync=0;
int status_sf1=0;
int good_packet_ping=0;
long msec1,max_msec=0,msec_sync=0;
int bad_packet=0,cnt_packet=0;
//// назначем переменным параметры которые вытаскиваем из конфига /etc/sf1.conf 
int inc_port_slave_master=0;
int status_pid;
////
init_con();
config_destroy(cfg0);
// создаем потомка
if(check_daemon()==20)
{
pid = fork();
setpriority(PRIO_PROCESS, 0, -20);// самый (наивысший приоритет) процесса //пока тестим...
if (pid == -1) // если не удалось запустить потомка
{
// выведем на экран ошибку и её описание
writelog_daemon("Start Daemon Error: %s\n", strerror(errno));
return -1;
}
else if (!pid) // если это потомок
{
// данный код уже выполняется в процессе потомка
// разрешаем выставлять все биты прав на создаваемые файлы,
// иначе у нас могут быть проблемы с правами доступа
umask(0);
// создаём новый сеанс, чтобы не зависеть от родителя
setsid();
// переходим в корень диска, если мы этого не сделаем, то могут быть проблемы.
// к примеру с размантированием дисков
chdir("/");
// закрываем дискрипторы ввода/вывода/ошибок, так как нам они больше не понадобятся
//close(STDIN_FILENO);
//close(STDOUT_FILENO);
//close(STDERR_FILENO);
// Данная функция будет осуществлять слежение за процессом
status = MonitorProc(argv[1]);
return status;
}
else // если это родитель
{
}
//if((strcmp(argv[1],"--debug")==0)|| (strcmp(argv[1],"-d")==0)) check_sw(1);
}
return 0;
}