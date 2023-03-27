// iniparser.c

#include <stdio.h>
#include <string.h>
#include "iniparser.h"
#include "unix_wrappers.h"

struct Section {
    unsigned int num_values;
    char* section_name;
    char** keys;
    char** values;
};
typedef struct Section section;


struct settings {
    unsigned int num_sections;
    section** sections;
};


typedef struct {
    char* key;
    char* value;
} key_value_t;


static void set_section_name(section* s, char* name)
{
    name++;
    char* delim = strchr(name, ']');
    *delim = '\0';
    s->section_name = Malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(s->section_name, name);
}


static void initialize_section(struct settings* settings, char* buffer)
{
    section* s = Malloc(sizeof(struct Section));
    settings->sections = Realloc(settings->sections, sizeof(section*) * (settings->num_sections + 1));
    settings->sections[(settings->num_sections)++] = s;
    settings->sections[settings->num_sections - 1]->num_values = 0;
    set_section_name(settings->sections[settings->num_sections - 1], buffer);
    settings->sections[settings->num_sections - 1]->keys = Malloc(sizeof(char*));
    settings->sections[settings->num_sections - 1]->values = Malloc(sizeof(char*));
}


static void get_key_value_pair(char* str, key_value_t* key_value_pair)
{
    char* delim = strchr(str, '=');
    *delim = '\0';
    char* key = str;
    char* value = delim + 1;
    
    size_t length = strlen(value);
    if (value[length - 1] == '\n')
        value[length - 1] = '\0';
    else
        value[length] = '\0';
    key_value_pair->key = key;
    key_value_pair->value = value;
}


static void set_key_value(section* s, key_value_t* key_value_pair)
{
    s->keys = Realloc(s->keys, sizeof(char*) * (s->num_values + 1));
    s->keys[s->num_values] = Malloc(sizeof(char)*strlen(key_value_pair->key) + 1);
    strcpy(s->keys[s->num_values], key_value_pair->key);

    s->values = Realloc(s->values, sizeof(char*) * (s->num_values + 1));
    s->values[s->num_values] = Malloc(sizeof(char)*strlen(key_value_pair->value) + 1);
    strcpy(s->values[s->num_values], key_value_pair->value);   
}


static int parse_file(struct settings* settings, const char* name)
{
    char buffer[128];
    FILE* fptr = fopen(name, "r");

    if (fptr == NULL)
        return -1;
    
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        if (buffer[0] == '[') {
            initialize_section(settings, buffer);
            continue; 
        }
        if (strchr(buffer, '=') != NULL) {
            key_value_t key_value_pair;
            get_key_value_pair(buffer, &key_value_pair);
            set_key_value(settings->sections[settings->num_sections - 1], &key_value_pair);
            (settings->sections[settings->num_sections - 1]->num_values)++;
        }
    }
    fclose(fptr);
    return 0;
}


static void free_settings(struct settings* settings)
{
    for (unsigned int i=0; i<settings->num_sections; i++) {
        for (unsigned int j = 0; j<settings->sections[i]->num_values; j++) {
            Free(settings->sections[i]->keys[j]);
            settings->sections[i]->keys[j] = NULL;
            Free(settings->sections[i]->values[j]);
            settings->sections[i]->values[j] = NULL;
        }
        Free(settings->sections[i]->section_name);
        settings->sections[i]->section_name = NULL;
        Free(settings->sections[i]->keys);
        settings->sections[i]->keys = NULL;
        Free(settings->sections[i]->values);
        settings->sections[i]->values = NULL;
        Free(settings->sections[i]);
        settings->sections[i] = NULL;
    }
    Free(settings->sections);
    settings->sections = NULL;
    settings->num_sections = 0;
    Free(settings);
    settings = NULL;
}


static char* _set_value(struct settings* settings, char* key, char* value)
{
    for (unsigned int i=0; i<settings->num_sections; i++) {
        for (unsigned int j=0; j<settings->sections[i]->num_values; j++) {
            if (!strcmp(key, settings->sections[i]->keys[j])) {
                settings->sections[i]->values[j] = Realloc(settings->sections[i]->values[j], sizeof(char)*strlen(value) + 1);
                strcpy(settings->sections[i]->values[j], value);
                return settings->sections[i]->values[j];
            }
        }
    }
    printf("key not found\n");
    return NULL;
}


static char* _get_value(struct settings* settings, char* key)
{
    for (unsigned int i=0; i<settings->num_sections; i++) {
        for (unsigned int j=0; j<settings->sections[i]->num_values; j++) {
            if (!strcmp(key, settings->sections[i]->keys[j])) {
                return settings->sections[i]->values[j];
            }
        }
    }
    printf("key not found: %s\n", key);
    return NULL;
}


static struct settings* initialize_inisettings(struct settings* settings, char* filename)
{
    settings = Malloc(sizeof(struct settings));
    settings->num_sections = 0;
    settings->sections = Malloc(sizeof (section*));
    if (parse_file(settings, filename) < 0) {
        fprintf(stderr, "settings error: %s\n", strerror(errno));
        Free(settings->sections);
        Free(settings);
        return NULL;
    }
    else
        return settings;
}

struct settings* (*init_settings) (struct settings*, char*) = initialize_inisettings;
char* (*set_value) (struct settings*, char*, char*) = _set_value;
char* (*get_value) (struct settings*, char*) = _get_value;
void (*clear_settings) (struct settings*) = free_settings;