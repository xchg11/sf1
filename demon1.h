// fire1024 (c) 2014
// sf1 include 
#ifndef DEMON1_H
#define DEMON1_H
///
#define FD_LIMIT			1024*10
// константы для кодов завершения процесса
#define CHILD_NEED_WORK			1
#define CHILD_NEED_TERMINATE	2
#define PID_FILE "/var/run/sf1.pid"
#define file_status1 "/var/sf1/sf1_status.lock"
//
////////////////////////
static char *iface_conf1;
static char *status_failover;
int status_failover_int;
static char *mcast_ipaddr_conf1;
static char *bind_ipaddr_conf1;
static char *ping_count_packet_conf1;
static char *failover_network_iface;
static char *bind_iface_daemon_failover_conf1;
static char *failover_ipaddr;
static char *failover_ipaddr_slave;
static char *failover_macaddr;
static char *failover_macaddr_2;
static char *failover_macaddr_slave;
static char *failover_uplink_iface;
static char *failover_uplink_iface_slave;
static char *failover_uplink_ipaddr;
static char *failover_uplink_ipaddr_slave;
static char *failover_uplink_ipaddr_mask;
static char *failover_uplink_ipaddr_slave_mask;
static char *failover_ipaddr_mask;
static char *failover_ipaddr_slave_mask;
const char *str;
config_t cfg,*cfg0;
void init_con()
{
//
config_init(&cfg);
cfg0=&cfg;
if(!config_read_file(cfg0,"/etc/sf1/sf1.conf"))
{
fprintf(stderr, "%s:%d - %s\n",config_error_file(cfg0),
config_error_line(cfg0), config_error_text(cfg0));
config_destroy(cfg0);
exit(1);
}
setpriority(PRIO_PROCESS, 0, -20);// самый (наивысший приоритет)
if(config_lookup_string(cfg0,"MODE",&str)) status_failover=strdup((char *)str);
if(config_lookup_string(cfg0,"GMULTICAST_IPADDR",&str)) mcast_ipaddr_conf1=strdup((char *)str);
if(config_lookup_string(cfg0,"PING_SEND_PACKETS",&str)) ping_count_packet_conf1=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_BIND_DEMON_IFACE",&str)) bind_iface_daemon_failover_conf1=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_NETWORK_IFACE",&str)) failover_network_iface=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_IPADDR",&str)) failover_ipaddr=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_IPADDR_SLAVE",&str)) failover_ipaddr_slave=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_MACADDR",&str)) failover_macaddr=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_MACADDR_SLAVE",&str)) failover_macaddr_slave=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IFACE",&str)) failover_uplink_iface=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IFACE_SLAVE",&str)) failover_uplink_iface_slave=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IPADDR",&str)) failover_uplink_ipaddr=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IPADDR_SLAVE",&str)) failover_uplink_ipaddr_slave=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IPADDR_MASK",&str)) failover_uplink_ipaddr_mask=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_UPLINK_IPADDR_SLAVE_MASK",&str)) failover_uplink_ipaddr_slave_mask=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_IPADDR_MASK",&str)) failover_ipaddr_mask=strdup((char *)str);
if(config_lookup_string(cfg0,"FAILOVER_IPADDR_SLAVE_MASK",&str)) failover_ipaddr_slave_mask=strdup((char *)str);
if(config_lookup_string(cfg0,"TIMEOUT_PING",&str)) failover_macaddr_slave=strdup((char *)str);
}
///
////////////////////////////

////////
// функция загрузки конфига
int LoadConfig(char* FileName)
{
	// тут должен быть код для загрузки конфига
	return 1;
}
// функция которая загрузит конфиг заново
// и внесет нужные поправки в работу
int signusr10(char *ch1)
{
char *li_ip_addr;
config_t cfg00,*cfg0;
const char *str;
//// назначем переменным параметры которые вытаскиваем из конфига /etc/sf1.conf.
static char *iface_conf1;
static char *status_failover;
int status_failover_int;
static char *mcast_ipaddr_conf1;
static char *bind_ipaddr_conf1;
static char *ping_count_packet_conf1;
static char *iface_failover_conf1;
static char *bind_iface_daemon_failover_conf1;
int status1=0;
//читаем конфиг.
config_init(&cfg00);
cfg0=&cfg00;
if(!config_read_file(cfg0,"/etc/sf1/sf1.conf"))
{
fprintf(stderr, "%s:%d - %s\n",config_error_file(cfg0),
config_error_line(cfg0), config_error_text(cfg0));
config_destroy(cfg0);
exit(1);
}
if(config_lookup_string(cfg0,"MODE",&str)) status_failover=(char *)str;
if(config_lookup_string(cfg0,"GMULTICAST_IPADDR",&str)) mcast_ipaddr_conf1=(char *)str;
if(config_lookup_string(cfg0,"PING_SEND_PACKETS",&str)) ping_count_packet_conf1=(char *)str;
if(config_lookup_string(cfg0,"FAILOVER_BIND_DEMON_IFACE",&str)) bind_iface_daemon_failover_conf1=(char *)str;
if(config_lookup_string(cfg0,"FAILOVER_NETWORK_IFACE",&str)) iface_failover_conf1=(char *)str;
//проверяем параметры из конфига на корректность...
li_ip_addr=getipaddr(bind_iface_daemon_failover_conf1);
//// дальше идет код 
if(strcmp(status_failover,"MASTER")==0)
{
// посылаем статус 28 что нужно слейву прекратить мониторитть сеть и  случае падение чтобы он непереключал интерфейсы так как мастер идет в даун ))
//status1=pinger1(bind_iface_daemon_failover_conf1,li_ip_addr,GROUP_MCAST_PORT1+0/*failover_slave_master port inc */,GROUP_MCAST_ADDR1,28,5,40000);
//
}
config_destroy(cfg0); //закрываем конфиг 
	// код функции
	return 1;
}
// функция для остановки потоков и освобождения ресурсов
void DestroyWorkThread()
{
writelog_daemon("%s dsds test \n",gettime_tt());
	// тут должен быть код который остановит все потоки и
	// корректно освободит ресурсы
}
//
int check_sw(int debug1)
{
int desc1;
int status;
int pid;
char *li_ip_addr;
int cnt_resync_status=0;
int status1,status_sync=0;
int status_sf1=0;
int good_packet_ping=0;
int status_debug=0;
long msec1,max_msec=0,msec_sync=0;
int bad_packet=0,cnt_packet=0;
//// назначем переменным параметры которые вытаскиваем из конфига /etc/sf1.conf.
int inc_port_slave_master=0;
int status_pid;
// начало кода программы.
// проверяем режим дебага///
//
li_ip_addr=getipaddr(bind_iface_daemon_failover_conf1);
if(strcmp(status_failover,"MASTER")==0){inc_port_slave_master=1;status_sf1=100;}
if(strcmp(status_failover,"SLAVE")==0) {inc_port_slave_master=0;status_sf1=200;}
//устанавливем рукапожатие и выясняем кто мастер кто слейв для дольнейщей работы.
// делаем рукопожатие sync.
//запускаем функцию pinger1 со статутом sync status_sf+20.
//if(debug1==1)
for(;;)
{
//debug1=1;
if(debug1==1) {printf("[ %s ] \033[1;33mTESTING SYNC MASTER/SLAVE.\033[0m",gettime_tt());}
time_start();// засекаем время выполение в микросекундах функции pinger1
status_sync=pinger1(bind_iface_daemon_failover_conf1,li_ip_addr,GROUP_MCAST_PORT1+inc_port_slave_master,GROUP_MCAST_ADDR1,status_sf1+20,5,40000);
msec_sync=time_stop();//
if(status_sync!=50 && status_sync>1)
{
if(debug1==1) {printf("\n[ %s ] packet recv %d resync service...",gettime_tt(),status_sync);}
}
if (status_sync==50) {if(debug1==1){printf("\n[ %s ] status sync = %d OK.  time elapsed = %lumsec STATUS-FAILOVER = %s\n",gettime_tt(),status_sync,msec_sync,status_failover);}break;}
else
{
if(debug1==1) {printf("\n[ %s ] \033[1;31mstatus sync faild...\033[0m\n[ %s ] \033[1;31merrorlevel=1 time  time elapsed = %lumsec\033[0m\n",gettime_tt(),gettime_tt(),msec_sync);}
if(cnt_resync_status==20)
{
sleep(10);
if (debug1==1) {printf("[ %s ] Fix the remote receiver\n",gettime_tt());writelog("[ %s ] Fix the remote receiver\n",gettime_tt());}
}
cnt_resync_status++;
}
}
// если status_sync=1 то связь установлена.
// если status_sync=0 то согласовать соединение и дальнейшую работу не удалось.
///
//
int st1=4;
if(strcmp(status_failover,"SLAVE")==0) // если слайв
{
for(;;)
{
for(int i10=0;i10<st1;i10++)
{
time_start();
status1=pinger1(bind_iface_daemon_failover_conf1,li_ip_addr,GROUP_MCAST_PORT1+inc_port_slave_master,GROUP_MCAST_ADDR1,status_sf1,1,10000);
if(status1==1) good_packet_ping++;// если отправили/получилил хороший пакет считаем.
cnt_packet+=status1;
msec1=time_stop();
if(msec1>max_msec) max_msec=msec1;
if(status1==0) bad_packet++;
//printf("[ %s ] packet send OK = %d  ms  %lu bad_packet %d (max_ping msecond) =  %lu  packet_good = %d\n",gettime_tt(),status1,msec1,bad_packet,max_msec,cnt_packet-bad_packet);
usleep(20000); //send 1 packet 0.050msec.
}
if(status_readfile(file_status1)==0x30 && good_packet_ping==0) //мастер упал переключаем со слейва на мастер
{
// меняем внутренний ип и мак шлюза на мастера.
changemac_iface(failover_network_iface,failover_macaddr);
changeip_addr(failover_network_iface,failover_ipaddr,failover_ipaddr_mask);
// меняем внешний ип и мак шлюза.
//changemac_iface(failover_uplink_iface,"NONE"); // временно отключено дописать!!!! позже.
changeip_addr(failover_uplink_iface,failover_uplink_ipaddr,failover_uplink_ipaddr_mask);
if(debug1==1) {printf("[ %s ] \033[1;33mchange slave to master IP=%s\033[0m -> \033[1;32mIP=%s and MAC=%s -> MAC=%s\033[0m\n",gettime_tt(),failover_ipaddr_slave,failover_ipaddr,failover_macaddr_slave,failover_macaddr);}
writelog("[ %s ] \033[1;33mchange slave to master IP=%s\033[0m -> \033[1;32mIP=%s and MAC=%s -> MAC=%s\033[0m\n",gettime_tt(),failover_ipaddr_slave,failover_ipaddr,failover_macaddr_slave,failover_macaddr);
// пишем в лог что меняем ip-mac на master.
status_writefile(file_status1,0x31);
continue;
}
if((status_readfile(file_status1)==0x31  && good_packet_ping==4))// мастер появился снимаем ип адреса обратно на slave //
{
// меняем внутренний ип и мак шлюза.
changemac_iface(failover_network_iface,failover_macaddr_slave);
changeip_addr(failover_network_iface,failover_ipaddr_slave,failover_ipaddr_slave_mask);
// меняем внешний ип и мак шлюза.
//changemac_iface(failover_uplink_iface,"NONE"); //если мак адрес none то не меняем мак адрес.
changeip_addr(failover_uplink_iface,failover_uplink_ipaddr_slave,failover_uplink_ipaddr_slave_mask);
if(debug1==1) {printf("[ %s ] \033[1;32mchange master to slave  IP=%s\033[0m -> \033[1;33mIP=%s and MAC=%s -> MAC=%s\033[0m\n",gettime_tt(),failover_ipaddr,failover_ipaddr_slave,failover_macaddr,failover_macaddr_slave);}
// пишем в лог что меняем ip-mac на master.
writelog("[ %s ] \033[1;32mchange master to slave  IP=%s\033[0m -> \033[1;33mIP=%s and MAC=%s -> MAC=%s\033[0m\n",gettime_tt(),failover_ipaddr,failover_ipaddr_slave,failover_macaddr,failover_macaddr_slave);
status_writefile(file_status1,0x30);
}
good_packet_ping=0;
}
}
free(mcast_ipaddr_conf1);
free(bind_ipaddr_conf1);
free(ping_count_packet_conf1);
free(failover_network_iface);
free(bind_iface_daemon_failover_conf1);
free(failover_ipaddr);
free(failover_ipaddr_slave);
free(failover_macaddr);
free(failover_macaddr_2);
free(failover_macaddr_slave);
}
// завершим процес, т.к. основную свою задачу (запуск демона) мы выполнили
//

// функция которая инициализирует рабочие потоки
int InitWorkThread(char *arg1)
{
int st1;
char *li_ip_addr;
const char *str;
int inc_port_slave_master=0;
int status_sf1=0;
//
if(strcmp(status_failover,"MASTER")==0) {inc_port_slave_master=0;status_sf1=100;}
if(strcmp(status_failover,"SLAVE")==0) {inc_port_slave_master=1;status_sf1=200;}
li_ip_addr=getipaddr(bind_iface_daemon_failover_conf1);
if(fork()==0)
{
st1=reciver_1000(bind_iface_daemon_failover_conf1,li_ip_addr,status_sf1,GROUP_MCAST_PORT1+inc_port_slave_master,GROUP_MCAST_ADDR1,100000);
}
if(fork()==0)
{
if((strcmp(arg1,"-d")==0) || (strcmp(arg1,"--debug")==0)) check_sw(1);
if((strcmp(arg1,"-d")!=0) || (strcmp(arg1,"--debug")!=0)) check_sw(0);
}
return 0;
}
// функция обработки сигналов
static void signal_error(int sig, siginfo_t *si, void *ptr)
{
	void*  ErrorAddr;
	void*  Trace[16];
	int    x;
	int    TraceSize;
	char** Messages;

	// запишем в лог что за сигнал пришел
writelog_daemon("%s [DAEMON] Signal: %s, Addr: 0x%0.16X\n",gettime_tt(), strsignal(sig), si->si_addr);


	#if __WORDSIZE == 64 // если дело имеем с 64 битной ОС
		// получим адрес инструкции которая вызвала ошибку
		ErrorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_RIP];
	#else
		// получим адрес инструкции которая вызвала ошибку
		ErrorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_EIP];
	#endif

	// произведем backtrace чтобы получить весь стек вызовов
	TraceSize = backtrace(Trace, 16);
	Trace[1] = ErrorAddr;

	// получим расшифровку трасировки
	Messages = backtrace_symbols(Trace, TraceSize);
	if (Messages)
	{
		writelog_daemon("%s == Backtrace ==\n",gettime_tt());

		// запишем в лог
		for (x = 1; x < TraceSize; x++)
		{
			writelog_daemon(Messages[x]);
		}

		writelog_daemon("%s == End Backtrace ==\n",gettime_tt());
		free(Messages);
	}

	writelog_daemon("%s [DAEMON] Stopped\n",gettime_tt());

	// остановим все рабочие потоки и корректно закроем всё что надо
	DestroyWorkThread();

	// завершим процесс с кодом требующим перезапуска
	exit(CHILD_NEED_WORK);
}

// функция установки максимального кол-во дескрипторов которое может быть открыто 
int SetFdLimit(int MaxFd)
{
	struct rlimit lim;
	int           status;

	// зададим текущий лимит на кол-во открытых дискриптеров
	lim.rlim_cur = MaxFd;
	// зададим максимальный лимит на кол-во открытых дискриптеров
	lim.rlim_max = MaxFd;

	// установим указанное кол-во
	status = setrlimit(RLIMIT_NOFILE, &lim);

	return status;
}


int WorkProc(char *arg1)
{
	struct sigaction sigact;
	sigset_t         sigset;
	int              signo;
	int              status;
	// сигналы об ошибках в программе будут обрататывать более тщательно
	// указываем что хотим получать расширенную информацию об ошибках
	sigact.sa_flags = SA_SIGINFO;
	// задаем функцию обработчик сигналов
	sigact.sa_sigaction = signal_error;

	sigemptyset(&sigact.sa_mask);

	// установим наш обработчик на сигналы

	sigaction(SIGFPE, &sigact, 0); // ошибка FPU
	sigaction(SIGILL, &sigact, 0); // ошибочная инструкция
	sigaction(SIGSEGV, &sigact, 0); // ошибка доступа к памяти
	sigaction(SIGBUS, &sigact, 0); // ошибка шины, при обращении к физической памяти

	sigemptyset(&sigset);

	// блокируем сигналы которые будем ожидать
	// сигнал остановки процесса пользователем
	sigaddset(&sigset, SIGQUIT);

	// сигнал для остановки процесса пользователем с терминала
	sigaddset(&sigset, SIGINT);

	// сигнал запроса завершения процесса
	sigaddset(&sigset, SIGTERM);

	// пользовательский сигнал который мы будем использовать для обновления конфига
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	// Установим максимальное кол-во дискрипторов которое можно открыть
	SetFdLimit(FD_LIMIT);

	// запишем в лог, что наш демон стартовал
	writelog_daemon("[ %s ] [DAEMON] Started\n",gettime_tt());

	// запускаем все рабочие потоки
	status = InitWorkThread(arg1);
	if (!status)
		{
		// цикл ожижания сообщений
		for (;;)
		{
			// ждем указанных сообщений
			sigwait(&sigset, &signo);
// сигнал USR1 сигнал чтобы мастер послал слейву чтобы он перестал мониторниг сети...
if (signo == SIGUSR1)
{
status = signusr10("test");
if(status == 0)
{
writelog_daemon("[ %s ] [DAEMON] signal send packet slave to stop master failed\n",gettime_tt());
}
else
{
writelog_daemon("[ %s ] [DAEMON] signal send packet slave to stop master OK.\n",gettime_tt());
}
}
else // если какой-либо другой сигнал, то выйдим из цикла
{
break;
}
}
// остановим все рабочеи потоки и корректно закроем всё что надо
DestroyWorkThread();
}
else
{
writelog_daemon("[ %s ] [DAEMON] Create work thread failed\n",gettime_tt());
}
writelog_daemon("[ %s ] [DAEMON] Stopped\n",gettime_tt());
	// вернем код не требующим перезапуска
	return CHILD_NEED_WORK;
}
void SetPidFile(char* Filename)
{
FILE* f;
f = fopen(Filename, "w+");
chmod(Filename,0644);// chmod logfile (read+write | read | read )
if (f)
{
fprintf(f, "%u", getpid());
fclose(f);
}
}
///
int MonitorProc(char *arg1)
{
	int       pid;
	int       status;
	int       need_start = 1;
	sigset_t  sigset;
	siginfo_t siginfo;
	// настраиваем сигналы которые будем обрабатывать
	sigemptyset(&sigset);

	// сигнал остановки процесса пользователем
	sigaddset(&sigset, SIGQUIT);

	// сигнал для остановки процесса пользователем с терминала
	sigaddset(&sigset, SIGINT);

	// сигнал запроса завершения процесса
	sigaddset(&sigset, SIGTERM);

	// сигнал посылаемый при изменении статуса дочернего процесс
	sigaddset(&sigset, SIGCHLD);

	// сигнал посылаемый при изменении статуса дочернего процесс
	sigaddset(&sigset, SIGCHLD);

	// пользовательский сигнал который мы будем использовать для обновления конфига
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	// данная функция создат файл с нашим PID'ом
	SetPidFile(PID_FILE);

	// бесконечный цикл работы
	for (;;)
	{
		// если необходимо создать потомка
		if (need_start)
		{
			// создаём потомка
			pid = fork();
		}

		need_start = 1;

		if (pid == -1) // если произошла ошибка
		{
			// запишем в лог сообщение об этом
			writelog_daemon("[ %s ] [MONITOR] Fork failed (%s)\n",gettime_tt(), strerror(errno));
		}
		else if (!pid) // если мы потомок
		{
			// данный код выполняется в потомке

			// запустим функцию отвечающую за работу демона
			status = WorkProc(arg1);

			// завершим процесс
			exit(status);
		}
		else // если мы родитель
		{
			// данный код выполняется в родителе

			// ожидаем поступление сигнала
			sigwaitinfo(&sigset, &siginfo);

			// если пришел сигнал от потомка
			if (siginfo.si_signo == SIGCHLD)
			{
				// получаем статус завершение
				wait(&status);

				// преобразуем статус в нормальный вид
				status = WEXITSTATUS(status);

				 // если потомок завершил работу с кодом говорящем о том, что нет нужны дальше работать
				if (status == CHILD_NEED_TERMINATE)
				{
					// запишем в лог сообщени об этом
					writelog_daemon("[ %s ] [MONITOR] Childer stopped\n",gettime_tt());

					// прервем цикл
					//break;
				}
				else if (status == CHILD_NEED_WORK) // если требуется перезапустить потомка
				{
					// запишем в лог данное событие
					writelog_daemon("[ %s ] [MONITOR] Childer restart\n",gettime_tt());
				}
			}
			else if (siginfo.si_signo == SIGUSR1) // если пришел сигнал что необходимо перезагрузить конфиг
			{
				kill(pid, SIGUSR1); // перешлем его потомку
				need_start = 0; // установим флаг что нам не надо запускать потомка заново
			}
			else // если пришел какой-либо другой ожидаемый сигнал
			{
				// запишем в лог информацию о пришедшем сигнале
				writelog_daemon("[ %s ] [MONITOR] Signal stop\n",gettime_tt());

				// убьем потомка
				kill(pid, SIGTERM);
				status = 1;
				break;
			}
		}
	}

	// запишем в лог, что мы остановились
	writelog_daemon("[ %s ] [MONITOR] Stopped\n",gettime_tt());
	printf("%d \n",pid);
kill(pid+1,9);
	// удалим файл с PID'ом
	unlink(PID_FILE);

	return 1;
}
#endif