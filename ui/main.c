#define _XOPEN_SOURCE 600
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>

#include <sys/wait.h>
#include <unistd.h>
#define DIRSEP '/'
#define abs_path(rel,abs,abs_size) realpath((rel),(abs))

#include "mongoose.h"

#define MAX_OPTIONS 100
#define MAX_CONF_FILE_LINE_SIZE (8*1024)

static int exit_flag;
static char server_name[40];
static char config_file[PATH_MAX];
static struct mg_server *server;

#if !defined(CONFIG_FILE)
#define CONFIG_FILE "mongoose.conf"
#endif

static void signal_handler(int sig_num)
{
    // Reinstantiate signal handler
    signal(sig_num,signal_handler);
    exit_flag = sig_num;
}

static void init_server_name(void)
{
    snprintf(server_name,sizeof(server_name),"Mongoose web server v.%s",MONGOOSE_VERSION);
}

static void die(const char *fmt,...)
{
    va_list ap;
    char msg[200];
    va_start(ap,fmt);
    vsnprintf(msg,sizeof(msg),fmt,ap);
    va_end(ap);
    fprintf(stderr,"%s\n" ,msg);
    exit(EXIT_FAILURE);
}

static char *sdup(const char *str)
{
    char *p;
    if ((p = (char *)malloc(strlen(str)+1)) != NULL)
    {
        strcpy(p,str);
    }
    return p;
}

static int is_path_absolute(const char *path)
{
    return path != NULL && path[0] == '/';
}

static char *get_option(char **options,const char *option_name)
{
    int i;
    for (i = 0; options[i] != NULL; i++)
        if (!strcmp(options[i],option_name))
            return options[i+1];
    return NULL;
}
                

static void set_option(char **options,const char *name,const char *value)
{
    int i;

    for (i = 0;i < MAX_OPTIONS - 3; i++)
    {
        if (options[i] == NULL)
        {
            options[i] = sdup(name);
            options[i+1] = sdup(value);
            options[i+2] = NULL;
            break;
        }
        else if (!strcmp(options[i],name))
        {
            free(options[i+1]);
            options[i+1] = sdup(value);
            break;
        }
    }

    if (i == MAX_OPTIONS - 3)
    {
        die("%s","Too many options specified");
    }
}

static void verify_existence(char **options,const char *option_name,int must_be_dir)
{
    struct stat st;
    const char *path = get_option(options,option_name);

    if (path != NULL && (stat(path,&st) != 0 || ((S_ISDIR(st.st_mode) ? 1 : 0) != must_be_dir)))
    {
        die("Invalid path for %s: [%s]: (%s). Make sure that path is either"
            "absolute, or it is relative to mongoose executable.",
            option_name,path,strerror(errno));
    }
}

static void set_absolute_path(char *options[],const char *option_name,const char *path_to_exe)
{
    char path[PATH_MAX],abs[PATH_MAX],*option_value;
    const char *p;

    // Check whether option is already set
    option_value = get_option(options,option_name);

    // If option is already set and it is an absolute path,
    // leave it as it is -- it's already absolute.
    if (option_value != NULL && !is_path_absolute(option_value))
    {
        // Not absolute. Use the directory where mongoose executable lives
        // be the relative directory for everything.
        // Extract mongoose executable directory into path.
        if ((p = strrchr(path_to_exe,DIRSEP)) == NULL)
        {
            getcwd(path,sizeof(path));
        }
        else
        {
            snprintf(path,sizeof(path),"%.*s",(int)(p-path_to_exe),path_to_exe);
        }

        strncat(path,"/",sizeof(path)-1);
        strncat(path,option_value,sizeof(path)-1);

        // Absolutize the path,and set the option
        abs_path(path,abs,sizeof(abs));
        set_option(options,option_name,abs);
    }
}
static void show_usage_and_exit(void) 
{ 
    const char **names;
    int i;

    fprintf(stderr, "Mongoose version %s (c) Sergey Lyubka, built on %s\n",
            MONGOOSE_VERSION, __DATE__);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  mongoose -A <htpasswd_file> <realm> <user> <passwd>\n");
    fprintf(stderr, "  mongoose [config_file]\n");
    fprintf(stderr, "  mongoose [-option value ...]\n");
    fprintf(stderr, "\nOPTIONS:\n");

    names = mg_get_valid_option_names();
    for (i = 0; names[i] != NULL; i += 2) 
    {
        fprintf(stderr, "  -%s %s\n",
        names[i], names[i + 1] == NULL ? "<empty>" : names[i + 1]);
    }

    exit(EXIT_FAILURE);
}

static void process_command_line_arguments(char *argv[], char **options) 
{
    char line[MAX_CONF_FILE_LINE_SIZE], opt[sizeof(line)], val[sizeof(line)], *p;
    FILE *fp = NULL;
    size_t i, cmd_line_opts_start = 1, line_no = 0;

    // Should we use a config file ?
    if (argv[1] != NULL && argv[1][0] != '-') 
    {
        snprintf(config_file, sizeof(config_file), "%s", argv[1]);
        cmd_line_opts_start = 2;
    } 
    else if ((p = strrchr(argv[0], DIRSEP)) == NULL) 
    {
        // No command line flags specified. Look where binary lives
        snprintf(config_file, sizeof(config_file), "%s", CONFIG_FILE);
    } 
    else 
    {
        snprintf(config_file, sizeof(config_file), "%.*s%c%s",
                (int) (p - argv[0]), argv[0], DIRSEP, CONFIG_FILE);
    }
    
    fp = fopen(config_file, "r");
  
    // If config file was set in command line and open failed, die
    if (cmd_line_opts_start == 2 && fp == NULL)
    {
       die("Cannot open config file %s: %s",config_file,strerror(errno));
    }
   
    // Load config file settings first
    if (fp != NULL)
    {
        fprintf(stderr, "Loading config file %s\n", config_file);

        // Loop over the lines in config file
        while (fgets(line, sizeof(line), fp) != NULL) 
        {
            line_no++;
           
            // Ignore empty lines and comments
            for (i = 0; isspace(* (unsigned char *) &line[i]); ) i++;
            if (line[i] == '#' || line[i] == '\0') 
                continue;
            if (sscanf(line, "%s %[^\r\n#]", opt, val) != 2) 
            {
                printf("%s: line %d is invalid, ignoring it:\n %s",
                        config_file, (int) line_no, line);
            } 
            else 
            {
                set_option(options, opt, val);
            }
        }
      
        fclose(fp);
    }
    
    // If we're under MacOS and started by launchd, then the second
    // argument is process serial number, -psn_....
    // In this case, don't process arguments at all
    if ( argv[1] == NULL || memcmp(argv[1],"-psn_",5) != 0)
    {
        // Handle command line flags.
        // They override config file and default settings.
        for (i = cmd_line_opts_start; argv[i] != NULL; i += 2) 
        {
            if (argv[i][0] != '-' || argv[i + 1] == NULL) 
            {
                show_usage_and_exit();
            }
            set_option(options, &argv[i][1], argv[i + 1]);
        }
    }
}


static void start_mongoose(int argc,char *argv[])
{
    char *options[MAX_OPTIONS];
    int i;

    if ((server = mg_create_server(NULL)) == NULL)
        die("%s","Failed to start Mongoose.");

    /*
    // Edit password file if -A option is specified
    if (argc > 1 && !strcmp(argv[1],"-A"))
    {
        if (argc != 6)
            show_usage_and_exit();
        exit(modify_passwords_file(argv[2],argv[3],argv[4],argv[5])?EXIT_SUCCESS:EXIT_FAILURE);

    }
    */

    if (argc == 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))
    {
        show_usage_and_exit();
    }

    options[0] = NULL;
    set_option(options,"document_root","./web");
    set_option(options,"listening_port","80");

    // Update config based on command line arguments
    process_command_line_arguments(argv,options);

    // Make sure we have absolute paths for files and directories 
    set_absolute_path(options,"document_root",argv[0]);
    set_absolute_path(options,"put_delete_auth_file",argv[0]);
    set_absolute_path(options,"cgi_interpreter",argv[0]);
    set_absolute_path(options,"access_log_file",argv[0]);
    set_absolute_path(options,"error_log_file",argv[0]);
    set_absolute_path(options,"global_auth_file",argv[0]);
    set_absolute_path(options,"ssl_certificate",argv[0]);

    // Make extra verification for certain options
    verify_existence(options,"document_root",1);
    verify_existence(options,"cgi_interpreter",0);
    verify_existence(options,"ssl_certificate",0);
    
    for (i = 0; options[i] != NULL; i += 2)
    {
        const char *msg = mg_set_option(server,options[i],options[i+1]);
        if (msg != NULL) die("Failed to set option [%s]: %s",options[i],msg);
        free(options[i]);
        free(options[i+1]);
    }
        
    // Setup handler : quit on Ctrl-C
    signal(SIGTERM,signal_handler);
    signal(SIGINT,signal_handler);
}

int main(int argc,char *argv[])
{
    init_server_name();
    start_mongoose(argc,argv);
    printf("%s serving [%s] on port %s\n",
            server_name,mg_get_option(server,"document_root"),
            mg_get_option(server,"listening_port"));
    fflush(stdout);
    while(exit_flag == 0)
        mg_poll_server(server,1000);
    printf("Exiting on signal %d ...",exit_flag);
    fflush(stdout);
    mg_destroy_server(&server);
    printf("%s\n"," done. ");

    return EXIT_SUCCESS;
}
