// fire1024 (c) 2014
// function config libconfig 
#ifndef SF1_CONFIG_H
#define SF1_CONFIG_H
/////
///
config_t cfg; // global var - end program necessarily flush config_destroy(&cfg)
int init_config(char *confpath)
{
//init config
config_init(&cfg);
if(!config_read_file(&cfg, confpath))
{
fprintf(stderr, "%s:%d - %s\n",config_error_file(&cfg),
config_error_line(&cfg), config_error_text(&cfg));
config_destroy(&cfg);
return 100;
}
return 0;
}
/// get param 
char *config_getparamstr(char *getparam_name)
{
const char *str;
char *msg0;
if(config_lookup_string(&cfg,getparam_name,&str)) 
msg0=(char *)str;
return  msg0;
}
//////
#endif