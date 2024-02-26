#include <stdio.h>
#include <stdlib.h>
#include "iniparser.h"


int main(void)
{
    /* Get variables from config file */
    settings_t *settings = init_settings("config.ini");
    if (settings == NULL) {
        fprintf(stderr, "config file error\n");
        exit(EXIT_FAILURE);
    }
    
    char *ip = get_value(settings, "ip");
    printf("ip: %s\n", ip);

    clean_settings(settings);
}