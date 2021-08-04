// Coding fire1024 (c) 2014 ////////
// Header functions sf1 prog ///////
////////////////////////////////////
#ifndef SF1_H
#define SF1_H
//#include <features.h>
#include <libconfig.h>
#include <sys/resource.h>
#include <execinfo.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
//#include <ucontext.h>
//#include <sys/ucontext.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/times.h>
#include <time.h>
#include <sys/time.h>
//#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
//#include <wait.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <net/if_arp.h>
#include <termios.h>
#include <net/if.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdint.h>
#include <signal.h>
#include <linux/fs.h>
#include "writelog.h"
////
#define GROUP_MCAST_PORT1 4444 // дефолтные параметры будут читаться из конфига sf1.conf
#define GROUP_MCAST_ADDR1 "224.0.0.200"// дефолтные параметры будут читаться из конфига sf1.conf 
#define logfile "/var/log/sf1.log" // путь к лог файлу от работы демона.
///////////////////////////////////
//
// создаем свою структуру данных для обменна данными между сервисами
//  типо структура своего пакета...
//
struct sfr_pkt
{
uint8_t status;//STATUS MASTER|SLAVE
uint8_t derr;// error status
uint16_t cnt; //count packet
uint8_t ping;// ping.
uint8_t chk1;//check status
uint8_t other1;//
uint8_t other2;//
};
// размер пакета sfr_pkt получился sizeof(sfr_pkt).
/// функция записи в файл.
///////
/// функции для замера времени выполнение участка кода в миллисекундах.
struct timeval tv1,tv2,dtv;
struct timezone tz;
void time_start()
{
gettimeofday(&tv1, &tz);
}
long time_stop()
{
gettimeofday(&tv2, &tz);
dtv.tv_sec= tv2.tv_sec -tv1.tv_sec;
dtv.tv_usec=tv2.tv_usec-tv1.tv_usec;
if(dtv.tv_usec<0)
{
dtv.tv_sec--; dtv.tv_usec+=1000000;
}
return dtv.tv_sec*1000+dtv.tv_usec/1000;
}
// позже описать более детально сделать номальный камент....
// пишем функцию для смены мак адреса на сетевом интерфейсе...
int changemac_iface(char *iface_name,char *mac_addr)
{
struct ifreq ifr;
int s1;
int derr=0;
s1=socket(AF_INET,SOCK_DGRAM,0);
if(s1<0) derr=+10;
strcpy(ifr.ifr_name,iface_name);
sscanf(mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",&ifr.ifr_hwaddr.sa_data[0],&ifr.ifr_hwaddr.sa_data[1],&ifr.ifr_hwaddr.sa_data[2],
&ifr.ifr_hwaddr.sa_data[3],&ifr.ifr_hwaddr.sa_data[4],&ifr.ifr_hwaddr.sa_data[5]);
s1 = socket(AF_INET, SOCK_DGRAM, 0);
if(s1<0) derr=+10;
strcpy(ifr.ifr_name,iface_name);
ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
if(ioctl(s1, SIOCSIFHWADDR, &ifr) < 0) derr=+10;
// возврат статуса смены мак адреса.
return derr;
}
// пишем функцию для чтение IP адреса установленного на интерфейсе.
char* getipaddr(char *iface_name)
{
int fd;
struct ifreq ifr;
fd = socket(AF_INET, SOCK_DGRAM, 0);
if (fd<0)
{
perror("error socket creat");
exit(1);
}
ifr.ifr_addr.sa_family = AF_INET;
strncpy(ifr.ifr_name, iface_name, IFNAMSIZ-1);
ioctl(fd, SIOCGIFADDR, &ifr);
if(fd<0)
{
perror("error ioctl");
exit(1);
}
close(fd);
// возвращаем полученый адрес
return (inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}
// пишем функцию для смены IP адреса на сетевом интерфейсе...
//
int changeip_addr(char *iface_name,char *ip_addr, char *ip_mask)
{
struct ifreq ifr;
int derr=0;
int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
if(fd<0) derr=+10; // ошибка сокета derr=10 
strncpy(ifr.ifr_name, iface_name, IFNAMSIZ);
ifr.ifr_addr.sa_family = AF_INET;
inet_pton(AF_INET, ip_addr, ifr.ifr_addr.sa_data + 2);
ioctl(fd, SIOCSIFADDR, &ifr);
if(fd<0) derr=+10; // ошибка установки мак адреса derr=20
inet_pton(AF_INET, ip_mask, ifr.ifr_addr.sa_data + 2);
ioctl(fd, SIOCSIFNETMASK, &ifr);
if(fd<0) derr=+10;
ioctl(fd, SIOCGIFFLAGS, &ifr);
if(fd<0) derr=+10;
strncpy(ifr.ifr_name,iface_name, IFNAMSIZ);
ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
ioctl(fd, SIOCSIFFLAGS, &ifr);
if(fd<0) derr=+10;
}
int ponger1(char *iface_name,char *iface_localipaddr,int GROUP_MCAST_PORT,char *GROUP_MCAST_ADDR, int status_sf)
{
int status_failover=0;
struct in_addr lint;
struct sockaddr_in gsock;
int sd1;
int inc_port_slave_master=0;
char *li_ip_addr;
int derr=0;
int recv_packet=0;
ssize_t datlen;
char buffer[1000];
socklen_t opt_length;
int rc=0;
int i10,i20;
// инициализируем структуру данных пакета //
struct sfr_pkt buf_packet1;
struct sfr_pkt buf_packet2;
//
buf_packet1.status=status_sf;
buf_packet1.derr=0xff;
buf_packet1.cnt=0xff;
buf_packet1.ping=0xff;// pong.
buf_packet1.chk1=0xff;
buf_packet1.other1=0xff;
buf_packet1.other2=0xff;
//
sd1=socket(AF_INET,SOCK_DGRAM,0);
//fcntl(sd1, F_SETFL, O_NONBLOCK);// не блокирующий сокет.
if (sd1<0)
{
perror("Open datagram socket error");
exit(1);
}
memset((char *)&gsock,0,sizeof(gsock));
gsock.sin_family=AF_INET;
//мультикаст групповой адрес
gsock.sin_addr.s_addr=inet_addr(GROUP_MCAST_ADDR);
// порт udp.
gsock.sin_port=htons(GROUP_MCAST_PORT+1);
//bind интерфейс от адреса
memset((char *)&lint,0,sizeof(lint));
lint.s_addr=inet_addr(iface_localipaddr);
// задаем параметры нового сокета...
if(setsockopt(sd1, IPPROTO_IP, IP_MULTICAST_IF, (char *)&lint, sizeof(lint)) < 0)
{
perror("setting local interface error");
exit(1);
}
// рабочий кусок биндинга на интерфейс
/*struct ifreq ifr;
memset(&ifr, 0, sizeof(struct ifreq));
snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "ens224");
ioctl(sd1, SIOCGIFINDEX, &ifr);
setsockopt(sd1, SOL_SOCKET, SO_BINDTODEVICE,  (void*)&ifr, sizeof(struct ifreq));
*/
fd_set writeset;
FD_ZERO(&writeset);
FD_SET(sd1,&writeset);
fflush(stdout);
struct timeval timeout;
timeout.tv_sec=0;
timeout.tv_usec=100000;//timeout_p;// time out select socket
//
if((select(sd1+1,0,&writeset,(fd_set*)0,&timeout)==-1))
{
perror("select:");
//exit(1);
}
if(FD_ISSET(sd1,&writeset))
{
//
//
if(sendto(sd1,&buf_packet1,sizeof(buf_packet1),0,(struct sockaddr*)&gsock,sizeof(gsock))<0)
{
perror("send dat msg error");
exit(0);
}
}
close(sd1);
return 0;
}
//
//отдельная функция которая форкается для (ответа/приема ping/pong)
//////////////////////////////////////////////////////////////
int reciver_1000(char *iface_name,char  *iface_localipaddr, int status_sf,int GROUP_MCAST_PORT,char *GROUP_MCAST_ADDR,int timeout_p)
{
pid_t child;
struct sockaddr_in lsock;
struct ip_mreq grp1;
int sd1,sd2,i,n;
int ready;
char *li_ip_addr;
int recv_packet=0;
ssize_t datlen=0;
struct sfr_pkt buf_packet1;
struct sfr_pkt buf_packet2;
//
sd1=socket(AF_INET,SOCK_DGRAM,0);// создаем блокирующий сокет будем ждать до последнего ) без таймаута...
//fcntl(sd1, F_SETFL, O_NONBLOCK); // закоменчено неблокирующий сокет...
if(sd1<0)
{
perror("open datgr socket error");
close(sd1);
exit(1);
}
//printf("sock open ok.\n");
int reuse=1;
if(setsockopt(sd1, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
{
perror("Setting SO_REUSEADDR error");
close(sd1);
exit(1);
}
//printf("Setting SO_REUSEADDR...OK.\n");
// bind sock listen multicast group
memset((char *)&lsock,0,sizeof(lsock));
lsock.sin_family=AF_INET;
lsock.sin_port=htons(GROUP_MCAST_PORT);
lsock.sin_addr.s_addr=INADDR_ANY;
if(bind(sd1,(struct sockaddr*)&lsock,sizeof(lsock)))
{
perror("bind datgrm socket error");
close(sd1);
exit(1);
}
grp1.imr_multiaddr.s_addr=inet_addr(GROUP_MCAST_ADDR);
grp1.imr_interface.s_addr=inet_addr(iface_localipaddr);
if(setsockopt(sd1, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&grp1, sizeof(grp1)) < 0)
{
perror("set error \n");
close(sd1);
exit(1);
}
memset(&buf_packet1,sizeof(buf_packet1),0);
memset(&buf_packet2,sizeof(buf_packet2),0);
for(;;)
{
//
datlen=recv(sd1,&buf_packet1,sizeof(buf_packet1),0);
if(datlen==sizeof(buf_packet1) && buf_packet1.ping==0xf0)
{
recv_packet++;
// считываем статус и и если master_sync то отправляем уже другой статус status-sf (master_sync|slave_sync)
if(buf_packet1.status==120) {status_sf=120;}
if(buf_packet1.status==220) {status_sf=220;}
//отправляем в ответ пакет... pong = 0xff.
//int ponger1(char *iface_name,char *iface_localipaddr,int GROUP_MCAST_PORT,char *GROUP_MCAST_ADDR, int status_sf)
ponger1(iface_name,iface_localipaddr,GROUP_MCAST_PORT,GROUP_MCAST_ADDR,status_sf);
}
}
close(sd1);
return recv_packet;
}
//
int pinger1(char *iface_name,char *iface_localipaddr,int GROUP_MCAST_PORT,char *GROUP_MCAST_ADDR, int status_sf,int col_packet, int timeout_p)
{
//
fd_set readset,writeset;
struct in_addr lint;
struct sockaddr_in gsock,lsock;
struct ip_mreq grp1;
int reuse=1;
int status_sync=0;
int sd1,sd2,sd3;
int m_status=0;
int derr=0;
int recv_packet=0;
int recv_packet_sync=0;
ssize_t datlen;
int i10,i20;
// инициализируем структуру данных пакета //
struct sfr_pkt buf_packet1;
struct sfr_pkt buf_packet2;
//
//
sd1=socket(AF_INET,SOCK_DGRAM,0);
fcntl(sd1, F_SETFL, O_NONBLOCK);// делаем не блокирующий сокет.//
if (sd1<0)
{
perror("Open datagram socket error");
close(sd1);
exit(1);
}
memset((char *)&gsock,0,sizeof(gsock));
gsock.sin_family=AF_INET;
//мультикаст групповой адрес
gsock.sin_addr.s_addr=inet_addr(GROUP_MCAST_ADDR);
// порт udp. куда отправляем данные ping 
gsock.sin_port=htons(GROUP_MCAST_PORT);
//bind интерфейс от адреса iface_localipaddr
lint.s_addr=inet_addr(iface_localipaddr);
// задаем параметры нового сокета...
if(setsockopt(sd1, IPPROTO_IP, IP_MULTICAST_IF, (char *)&lint, sizeof(lint)) < 0)
{
perror("setting local interface error");
close(sd1);
exit(1);
}
////////////
//биндим интерфес для приема ответа pong 4444 port 
sd2=socket(AF_INET,SOCK_DGRAM,0);
fcntl(sd2, F_SETFL, O_NONBLOCK);
if(sd2<0)
{
perror("open datgr socket error");
close(sd2);
exit(1);
}
//printf("sock open ok.\n");
if(setsockopt(sd2, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
{
perror("Setting SO_REUSEADDR error");
close(sd2);
exit(1);
}
//printf("Setting SO_REUSEADDR...OK.\n");
// bind sock listen multicast group
memset((char *)&lsock,0,sizeof(lsock));
lsock.sin_family=AF_INET;
lsock.sin_port=htons(GROUP_MCAST_PORT+1);
lsock.sin_addr.s_addr=INADDR_ANY;
if(bind(sd2,(struct sockaddr*)&lsock,sizeof(lsock)))
{
perror("bind datgrm socket error");
close(sd2);
exit(1);
}
grp1.imr_multiaddr.s_addr=inet_addr(GROUP_MCAST_ADDR);
grp1.imr_interface.s_addr=inet_addr(iface_localipaddr);
if(setsockopt(sd2, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&grp1, sizeof(grp1)) < 0)
{
perror("setsockopt error... \n");
close(sd2);
exit(1);
}
//clear packet 
memset(&buf_packet2,sizeof(buf_packet2),0);
//////////////////////////////
recv_packet=0;
// количество отправляемых пакетов...
/// после того как заработает переписать более оптимально!!!!....
int status1=0;
//
memset(&buf_packet1,sizeof(buf_packet1),0);
buf_packet1.status=status_sf; // статус сервера кто он MASTER? ИЛИ SLAVE? или MASTER_SYNC? SLAVE_SYNC?
buf_packet1.derr=0xff;
buf_packet1.cnt=0xff;
buf_packet1.ping=0xf0;// флаг PING 
buf_packet1.chk1=0xff;
buf_packet1.other1=0xff;
buf_packet1.other2=0xff;
if(status_sf==120 || status_sf==220) // если статус sync делаем цикл больше для отправки 100 пакетов...тестируем типо
{
col_packet=50;
} 
//
for(int i0=0;i0<col_packet;i0++)
{
if(sendto(sd1,&buf_packet1,sizeof(buf_packet1),0,(struct sockaddr*)&gsock,sizeof(gsock))<0)
{
perror("send dat msg error");
close(sd1);
exit(0);
}
//memset(&buf_packet2,sizeof(buf_packet2),0);
FD_ZERO(&readset);
FD_SET(fileno(stdin), &readset);
FD_SET(sd2,&readset);
fflush(stdout);
struct timeval timeout;
timeout.tv_sec=0;
timeout.tv_usec=timeout_p;// time out select socket
//
if((select(sd2+1,&readset,0,(fd_set*)0,&timeout)==-1))
{
perror("select:");
//exit(1);
}
if(FD_ISSET(sd2,&readset))
{
datlen=recv(sd2,&buf_packet2,sizeof(buf_packet2),0);
// принимаем если статус идет status_sync проверка связи и синхронизация... от мастера
if(datlen==sizeof(buf_packet2) && buf_packet2.ping==0xff && buf_packet2.status==120)
{
recv_packet_sync++; // считаем принятые пакеты sync 
// обрабатываем действия....
//
}
// принимаем если статус идет status_sync проверка связи и синхронизация... от слейва
if(datlen==sizeof(buf_packet2) && buf_packet2.ping==0xff && buf_packet2.status==220)
{
recv_packet_sync++; // считаем принятые пакеты sync 
// обрабатываем действия....
//
}
//принимаем обычный пинг
if(datlen==sizeof(buf_packet2) && buf_packet2.ping==0xff)
{
/// выполняем действия если получили пакет pong 
if(buf_packet2.chk1!=0xfa) {recv_packet++;} // счетчик  принятых пакетов 
if(buf_packet2.chk1==0xfa) { recv_packet=0xDEDA;}//  флаг что мастер система ушла в легитимный ребут не производить ни каких действий на слейве!!!
}
//FD_ZERO(&readset);
}
//printf(".");
}
close(sd1);
close(sd2);
return recv_packet;
}
// status write sf1 /var/log/sf1/sf1.stat
//
int status_write(char *filename1,char *status)
{
FILE *f1;
f1=fopen(filename1,"r+");
chmod(filename1,0644);
if(f1)
{
fprintf(f1,"%s",status);
fclose(f1);
}
else {
writelog_daemon("error write status file %s\n",filename1);
}
}
//
char *gettime_str(time_t mytime)
{
return ctime(&mytime);
}
////
// пишем функции для чтение-записи статуса мастера в файл sf1_status.lock.
///
int status_readfile(char *fname)
{
int b1;
FILE *f1;
f1=fopen(fname,"r");
if(!f1) // если не можем считать то беда!!! ) 
{
time_t mytime;mytime=time(NULL);
writelog("error open file status system sf1_status.lock %s",ctime(&mytime));
}
else
{
fread(&b1,sizeof(unsigned char),1,f1);
fclose(f1);
}
return b1;
}
void status_writefile(char *fname,unsigned char writebyte)
{
int status_w=0;
FILE *f11;
f11=fopen(fname,"w");
if(!f11) // если не можем считать то беда!!! )
{
time_t mytime;mytime=time(NULL);
writelog("error open file status system sf1_status.lock %s",ctime(&mytime));
fclose(f11);
}
else
{
status_w=fwrite(&writebyte,1,1,f11); //
if(status_w!=1)
{
time_t mytime;mytime=time(NULL);
writelog("error write file  status system sf1_status.lock %s",ctime(&mytime));
fclose(f11);
}
}
fclose(f11);
return;
}
// возвращает 10 если файл sf1.pid существует в противном случае 20 
int check_daemon_run()
{
int status_w=0;
FILE *f1;
f1=fopen("/var/run/sf1.pid","r");
if(!f1)
{
status_w=20;
//fclose(f1);
}
else 
{
fclose(f1);
status_w=10;
}
return status_w;
}
///////////////////////////////////////////////////////////////////////////////////////
#endif