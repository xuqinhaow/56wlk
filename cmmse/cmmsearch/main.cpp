/***************************************************************************
 * 
 * Copyright (c) 2008 Baidu.com, Inc. All Rights Reserved
 * $Id: main.cpp,v 1.3 2009/09/24 10:00:11 yanghy2 Exp $ 
 * 
 **************************************************************************/



/**
 * @file main.cpp
 *
 * @date 2008/04/07 22:44:51
 * @version $Revision: 1.3 $ 
 * @brief 主函数
 *  
 **/
#include "pub.h"
STATIC const char BuildDate[] = __DATE__ " " __TIME__;

pthread_mutex_t g_ts_lock = PTHREAD_MUTEX_INITIALIZER;
cfg_data_t g_cfg;
ts_runtime_t g_runtime;
extern ts_runtime_t *pruntime ;

STATIC void print_help(void);
STATIC void print_version(void);
STATIC void parse_options(int argc, char **argv);

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    parse_options(argc, argv);

    ///初始化所有数据
    if(data_init() < 0)
    {
        printf("data init failed.");
    }


    int opret = 0;


    opret =
        init_data_accessor(g_cfg.svr_query.thread_num +
                           g_cfg.svr_update.thread_num,g_runtime);
    if(opret < 0)
    {
        printf("init_data_accessor failed.");
    }
	pruntime = &g_runtime;

    ///初始化更新线程
    ub_server_t *serv_update = ub_server_create();
    if(NULL == serv_update)
    {
        printf("ub_server_create failed.");
    }
    opret = ub_server_set_callback(serv_update, op_update);
    if(opret != 0)
    {
        printf("ub_server_set_callback failed.");
    }
    opret = ub_server_load(serv_update, &g_cfg.svr_update);
    if(opret != 0)
    {
        printf("ub_server_load failed.");
    }
    ASSERT_I(g_cfg.svr_update.thread_num, 1);
    //usr_data
    opret = ub_server_set_user_data(serv_update, g_data_map,
                                    sizeof(ansx::hash_map < sign64_t,
                                           pointer_t >));
    if(opret != 0)
    {
        printf("ub_server_set_user_data failed.");
    }
    opret = ub_server_set_buffer(serv_update,
                                 NULL, g_cfg.update_readbuf_size,
                                 NULL, g_cfg.update_writebuf_size);
    if(opret != 0)
    {
        printf("ub_server_set_buffer failed.");
    }

    ub_auth_t *auth = ub_auth_create();
    if(NULL == auth)
    {
        printf("ub_auth_create failed");
    }
    opret = ub_auth_load_ip(auth, g_cfg.iplist_path);
    if(opret != 0)
    {
        printf("ub_auth_load_ip failed.");
    }
    opret = ub_auth_compile(auth);
    if(opret != 0)
    {
        printf("ub_auth_compile failed.");
    }
    opret = ub_server_set_ip_auth(serv_update, auth);
    if(opret != 0)
    {
        printf("ub_server_set_ip_auth failed.");
    }
    //设置线程启动时处理
    opret = ub_server_set_threadstartfun(serv_update, update_thread_init);
    if(opret != 0)
    {
        printf("ub_server_set_threadstartfun failed.");
    }



    opret = ub_server_run(serv_update);
    if(opret != 0)
    {
        printf("ub_server_run failed.");
    }


    ///初始化查询线程
    ub_server_t *serv_query = ub_server_create();
    if(NULL == serv_query)
    {
        printf("ub_server_create failed.");
    }
    opret = ub_server_set_callback(serv_query, op_query);
    if(opret != 0)
    {
        printf("ub_server_set_callback failed.");
    }
    opret = ub_server_load(serv_query, &g_cfg.svr_query);
    if(opret != 0)
    {
        printf("ub_server_load failed.");
    }
    //usr_data
    opret =
        ub_server_set_user_data(serv_query,
                                g_data_map + g_cfg.svr_update.thread_num,
                                sizeof(ansx::hash_map < sign64_t,
                                       pointer_t >));
    if(opret != 0)
    {
        printf("ub_server_set_user_data failed.");
    }


    //buffer
    opret = ub_server_set_buffer(serv_query,
                                 NULL, g_cfg.query_readbuf_size,
                                 NULL, g_cfg.query_writebuf_size);
    if(opret != 0)
    {
        printf("ub_server_set_buffer failed.");
    }

    opret = ub_server_setoptsock(serv_query, UBSVR_NODELAY);
    if(opret != 0)
    {
        printf("ub_server_setoptsock failed.");
    }
    //设置线程启动时处理
    opret = ub_server_set_threadstartfun(serv_query, query_thread_init);
    if(opret != 0)
    {
        printf("ub_server_set_threadstartfun failed.");
    }



    opret = ub_server_run(serv_query);
    if(opret != 0)
    {
        printf("ub_server_run failed.");
    }
    ///定时处理线程
    UB_LOG_NOTICE("start ontime thread ...");
    pthread_t thr_ontime;
    ul_pthread_create(&thr_ontime, NULL, ontime_thread, NULL);

    ///线程销毁
    ub_server_join(serv_update);
    ub_server_join(serv_query);
    ub_server_destroy(serv_update);
    ub_server_destroy(serv_query);
    ub_auth_destroy(auth);


    return 0;
}


STATIC void print_logo(void)
{
	printf("\033[31m\033[1m"); 
	printf(" _____   _   __   _  __    __  _____   _____  \n");
	printf("|_   _| | | |  \\ | | \\ \\  / / /  ___/ | ____| \n");
	printf("  | |   | | |   \\| |  \\ \\/ /  | |___  | |__   \n");
	printf("  | |   | | | |\\   |   \\  /   \\___  \\ |  __| \n"); 
	printf("  | |   | | | | \\  |   / /     ___| | | |___  \n");
	printf("  |_|   |_| |_|  \\_|  /_/     /_____/ |_____|\n");
	printf("\033[0m");


}

STATIC void 
print_help(void)
{
	print_logo();
    printf("\nUsage:\n");
    printf("./%s <options>\n", PROJNAME);
    printf("  options:\n");
    printf("    -d:             配置文件路径\n");
    printf("    -f:             配置文件名\n");
    printf("    -g:             生成默认配置\n");
    printf("    -t:             检查配置\n");
    printf("    -v:             Print version information\n");
    printf("    -h:             This page\n");
    printf("\n\n");
}
STATIC void print_version(void)
{
	print_logo();
	printf("\n");
    printf("Project    :  %s\n", PROJNAME);
    printf("Version    :  %s\n", VERSION);
    //printf("Cvstag     :  %s\n", CVSTAG);
    //printf("Cvspath    :  %s\n", CVSPATH);
    printf("BuildDate  :  %s\n", BuildDate);
	printf("\n");
}

STATIC void parse_options(int argc, char **argv)
{
    int conf_build = 0;
    int conf_check = 0;
    int opret = 0;
    char c;

    memset(&g_cfg, 0, sizeof(g_cfg));
    strncpy(g_cfg.conf_dir, DEFAULT_CONF_DIR, TS_FILEPATH_MAXSIZE - 1);
    strncpy(g_cfg.conf_name, DEFAULT_CONF_NAME, TS_FILEPATH_MAXSIZE - 1);


    while((c = getopt(argc, argv, "f:tgd:c:hv?")) != -1)
    {
        switch (c)
        {
        case 'd':
            snprintf(g_cfg.conf_dir, TS_FILEPATH_MAXSIZE, "%s/", optarg);
            break;
        case 'f':
            strncpy(g_cfg.conf_name, optarg, sizeof(g_cfg.conf_name) - 1);
            if(!strstr(g_cfg.conf_name, ".conf"))   //如果没有.conf结尾，则表示参数错误
            {
                printf("config file name wrong. \n");
                print_help();
                exit(0);
            }
            break;
        case 'h':
        case '?':
            print_help();
            exit(0);
        case 'g':
            conf_build = 1;
            break;
        case 't':
            conf_check = 1;
            break;

        case 'v':
            print_version();
            exit(0);
        default:
            break;
        }
    }
    //读取配置文件
    if(conf_check)
    {
        //only check config
        opret = cfg_check();
        if(0 != opret)
        {
            fprintf(stderr,
                    "CHECK CONFIG FILE [dir:%s file:%s] FAILED!!\n",
                    g_cfg.conf_dir, g_cfg.conf_name);
        } else
        {
            fprintf(stderr,
                    "CHECK CONFIG FILE [dir:%s file:%s] SUCCESS!!\n",
                    g_cfg.conf_dir, g_cfg.conf_name);
        }
        goto out;
    } else
    {
        //load config
        opret = cfg_init(conf_build);
UB_LOG_FATAL("_______");
        if(0 == opret)
        {
            UB_LOG_TRACE("load config [dir:%s file:%s] success!",
                         g_cfg.conf_dir, g_cfg.conf_name);
        } else
        {
            UB_LOG_FATAL("load config [dir:%s file:%s] failed!",
                         g_cfg.conf_dir, g_cfg.conf_name);
            goto out;
        }
        if(1 == conf_build)
        {
            //生成配置文件,应该退出了
            goto out;
        }
    }

    return;
  out:
    exit(0);


}













/* vim: set ts=4 sw=4 sts=4 tw=100 */
