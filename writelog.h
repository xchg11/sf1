// взято с http://www.cs.virginia.edu/~jws9c/temple/307Fall2002/stdarg/
///
///
#include <fcntl.h>
#ifndef WRITELOG_H
#define WRITELOG_H
#define STRING_LENGTH 240                    /* something 'safe'             */
//

int writelog(char *fmt, ...)
{
va_list ap;                                /* special type for variable    */
char format[STRING_LENGTH];                /* argument lists               */
int count = 0;
int i, j;                                  /* Need all these to store      */
char c;                                    /* values below in switch       */
double d;
unsigned u;
char *s;
void *v;
FILE *log_file;
char error[STRING_LENGTH];
if (!(log_file = fopen("/var/log/sf1.log","a+w"))) {     /* open file for mode           */
sprintf(error, "init_log() failed to open\n");
perror(error);
exit(-1);                               /* message and exit on fail     */
}
va_start(ap, fmt);                         /* must be called before work   */
while (*fmt) {
for (j = 0; fmt[j] && fmt[j] != '%'; j++)	format[j] = fmt[j];                    /* not a format string          */
if (j) {
format[j] = '\0';
count += fprintf(log_file, format);    /* log it verbatim              */
fmt += j;
} 
else 
{
for (j = 0; !isalpha(fmt[j]); j++) {   /* find end of format specifier */
format[j] = fmt[j];
if (j && fmt[j] == '%')              /* special case printing '%'    */
break;
}
format[j] = fmt[j];                    /* finish writing specifier     */
format[j + 1] = '\0';                  /* don't forget NULL terminator */
fmt += j + 1;
switch (format[j]) {                   /* cases for all specifiers     */
case 'd':
case 'i':                              /* many use identical actions   */
i = va_arg(ap, int);                 /* process the argument         */
count += fprintf(log_file, format, i); /* and log it                 */
break;
case 'o':
case 'x':
case 'X':
case 'u':
u = va_arg(ap, unsigned);
count += fprintf(log_file, format, u);
break;
case 'c':
c = (char) va_arg(ap, int);          /* must cast!                   */
count += fprintf(log_file, format, c);
break;
case 's':
s = va_arg(ap, char *);
count += fprintf(log_file, format, s);
break;
case 'f':
case 'e':
case 'E':
case 'g':
case 'G':
d = va_arg(ap, double);
count += fprintf(log_file, format, d);
break;
case 'p':
v = va_arg(ap, void *);
count += fprintf(log_file, format, v);
break;
case 'n':
count += fprintf(log_file, "%d", count);
break;
case '%':
count += fprintf(log_file, "%%");
break;
default:
fprintf(stderr, "Invalid format specifier in log().\n");
}
}
}
va_end(ap);                                /* clean up                     */
fclose(log_file);
return count;
}
////
int writelog_daemon(char *fmt, ...)
{
FILE *log_file1;
char error[STRING_LENGTH];
va_list ap;                                /* special type for variable    */
char format[STRING_LENGTH];                /* argument lists               */
int count = 0;
int i, j;                                  /* Need all these to store      */
char c;                                    /* values below in switch       */
double d;
unsigned u;
char *s;
void *v;
if (!(log_file1 = fopen("/var/log/sf1.log","a+w"))) {     /* open file for mode           */
sprintf(error, "init_log() failed to open\n");
perror(error);
exit(-1);                               /* message and exit on fail     */
}

va_start(ap, fmt);                         /* must be called before work   */
while (*fmt) {
for (j = 0; fmt[j] && fmt[j] != '%'; j++)	format[j] = fmt[j];                    /* not a format string          */
if (j) {
format[j] = '\0';
count += fprintf(log_file1, format);    /* log it verbatim              */
fmt += j;
} 
else 
{
for (j = 0; !isalpha(fmt[j]); j++) {   /* find end of format specifier */
format[j] = fmt[j];
if (j && fmt[j] == '%')              /* special case printing '%'    */
break;
}
format[j] = fmt[j];                    /* finish writing specifier     */
format[j + 1] = '\0';                  /* don't forget NULL terminator */
fmt += j + 1;
switch (format[j]) {                   /* cases for all specifiers     */
case 'd':
case 'i':                              /* many use identical actions   */
i = va_arg(ap, int);                 /* process the argument         */
count += fprintf(log_file1, format, i); /* and log it                 */
break;
case 'o':
case 'x':
case 'X':
case 'u':
u = va_arg(ap, unsigned);
count += fprintf(log_file1, format, u);
break;
case 'c':
c = (char) va_arg(ap, int);          /* must cast!                   */
count += fprintf(log_file1, format, c);
break;
case 's':
s = va_arg(ap, char *);
count += fprintf(log_file1, format, s);
break;
case 'f':
case 'e':
case 'E':
case 'g':
case 'G':
d = va_arg(ap, double);
count += fprintf(log_file1, format, d);
break;
case 'p':
v = va_arg(ap, void *);
count += fprintf(log_file1, format, v);
break;
case 'n':
count += fprintf(log_file1, "%d", count);
break;
case '%':
count += fprintf(log_file1, "%%");
break;
default:
fprintf(stderr, "Invalid format specifier in log().\n");
}
}
}
va_end(ap);                                /* clean up                     */
fclose(log_file1);
return count;
}
static char *gettime_tt(void )
{
static char buff[20];
struct tm *stm,*local;
time_t t;
t=time(0);
stm = localtime(&t);
strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S",stm);
return (char*)buff;
}
#endif