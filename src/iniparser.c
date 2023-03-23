// iniparser.c

#include <stdio.h>
#include <string.h>
#include "fileparser.h"
#include "unix_wrappers.h"

struct Section {
    unsigned int num_values;
    char* section_name;
    char** keys;
    char** values;
};
typedef struct Section section;


static struct Settings {
    int initialized;
    unsigned int num_sections;
    section** sections;
} settings = {.initialized = 0, .num_sections = 0, .sections = NULL};


static struct Key_Value {
    char* key;
    char* value;
} key_value_pair;


static void set_section_name(section* s, char* name)
{
    name++;
    char* delim = strchr(name, ']');
    *delim = '\0';
    s->section_name = Malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(s->section_name, name);
}


static void initialize_section(char* buffer)
{
    section* s = Malloc(sizeof(struct Section));
    settings.sections = Realloc(settings.sections, sizeof(section*) * (settings.num_sections + 1));
    settings.sections[(settings.num_sections)++] = s;
    settings.sections[settings.num_sections - 1]->num_values = 0;
    set_section_name(settings.sections[settings.num_sections - 1], buffer);
    settings.sections[settings.num_sections - 1]->keys = Malloc(sizeof(char*));
    settings.sections[settings.num_sections - 1]->values = Malloc(sizeof(char*));
}


static void get_key_value_pair(char* str)
{
    char* delim = strchr(str, '=');
    *delim = '\0';
    char* key = str;
    char* value = delim + 1;
    
    int length = strlen(value);
    if (value[length - 1] == '\n')
        value[length - 1] = '\0';
    else
        value[length] = '\0';
    key_value_pair.key = key;
    key_value_pair.value = value;
}


static void set_key_value(section* s)
{
    s->keys = Realloc(s->keys, sizeof(char*) * (s->num_values + 1));
    s->keys[s->num_values] = Malloc(sizeof(char)*strlen(key_value_pair.key) + 1);
    strcpy(s->keys[s->num_values], key_value_pair.key);

    s->values = Realloc(s->values, sizeof(char*) * (s->num_values + 1));
    s->values[s->num_values] = Malloc(sizeof(char)*strlen(key_value_pair.value) + 1);
    strcpy(s->values[s->num_values], key_value_pair.value);   
}


static void parse_file(const char* name)
{
    char buffer[128];
    FILE* fptr = fopen(name, "r");
    
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        if (buffer[0] == '[') {
            initialize_section(buffer);
            continue; 
        }
        if (strchr(buffer, '=') != NULL) {
            get_key_value_pair(buffer);
            set_key_value(settings.sections[settings.num_sections - 1]);
            (settings.sections[settings.num_sections - 1]->num_values)++;
        }
    }
    fclose(fptr);  
}


static void free_settings()
{
    for (unsigned int i=0; i<settings.num_sections; i++) {
        for (unsigned int j = 0; j<settings.sections[i]->num_values; j++) {
            Free(settings.sections[i]->keys[j]);
            settings.sections[i]->keys[j] = NULL;
            Free(settings.sections[i]->values[j]);
            settings.sections[i]->values[j] = NULL;
        }
        Free(settings.sections[i]->section_name);
        settings.sections[i]->section_name = NULL;
        Free(settings.sections[i]->keys);
        settings.sections[i]->keys = NULL;
        Free(settings.sections[i]->values);
        settings.sections[i]->values = NULL;
        Free(settings.sections[i]);
        settings.sections[i] = NULL;
    }
    Free(settings.sections);
    settings.sections = NULL;
    settings.num_sections = 0;
    settings.initialized = 0;
}


static void initialize_settings(char* filename)
{
    if (settings.initialized)
        free_settings();
    settings.initialized = 1;
    settings.sections = Malloc(sizeof (section*));
    parse_file(filename);
}


void set_value(char* value)
{
    value++;
    return;
}


int main (void)
{
    initialize_settings("/home/felipe/repos/so2/2023---laboratorio-i-felipx/build/config.ini");
    
    printf("num_sections: %d\n", settings.num_sections);
    for (unsigned int j=0; j<settings.num_sections; j++) {
        printf("section name: %s\n", settings.sections[j]->section_name);
        printf("size of section: %ld\n", sizeof(*(settings.sections[j])));
        printf("seccion values: %d\n", settings.sections[j]->num_values);
        for (unsigned int i = 0; i<settings.sections[j]->num_values; i++) {
            printf("key[%d]: %s\n", i, settings.sections[j]->keys[i]);
            printf("value[%d]: %s\n", i, settings.sections[j]->values[i]);
        }
    }

    initialize_settings("/home/felipe/repos/so2/2023---laboratorio-i-felipx/build/config.ini");
    
    printf("num_sections: %d\n", settings.num_sections);
    for (unsigned int j=0; j<settings.num_sections; j++) {
        printf("section name: %s\n", settings.sections[j]->section_name);
        printf("size of section: %ld\n", sizeof(*(settings.sections[j])));
        printf("seccion values: %d\n", settings.sections[j]->num_values);
        for (unsigned int i = 0; i<settings.sections[j]->num_values; i++) {
            printf("key[%d]: %s\n", i, settings.sections[j]->keys[i]);
            printf("value[%d]: %s\n", i, settings.sections[j]->values[i]);
        }
    }

    free_settings();

}
