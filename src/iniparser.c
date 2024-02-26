// iniparser.c

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iniparser.h"


/**
 * @struct key_value_t
 * @brief internal structure to handle key-value pairs
*/
typedef struct {
    char* key;
    char* value;
} key_value_t;


/**
 * private internal functions
 */


/**
 * @brief Unix-style error
 * @param msg error message
*/
static void unix_error(char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}


/**
 * @brief wrapper function for malloc
 * @param size number of bytes to allocate
*/
static void* Malloc(size_t size) 
{
    void* p;

    if ((p = malloc(size)) == NULL)
	    unix_error("Malloc error");
    return p;
}


/**
 * @brief wrapper function for realloc
 * @param ptr pointer to allocated memory
 * @param size new size to allocate
*/
static void* Realloc(void* ptr, size_t size) 
{
    void* p;

    if ((p  = realloc(ptr, size)) == NULL)
	    unix_error("Realloc error");
    return p;
}


/**
 * @brief saves the name of a section. Allocates memory to store the name
 * @param s section to be named
 * @param name name of the section
*/
static void set_section_name(section_t *s, char *name)
{
    s->section_name = Malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(s->section_name, name);    
}


/**
 * @brief creates a new section
 * @param settings settings to store the new section
 * @param name name of the section
*/
static void initialize_section(settings_t *settings, char *name)
{
    section_t *s = Malloc(sizeof(section_t));
    settings->sections = Realloc(settings->sections, sizeof(section_t *) * (settings->num_sections + 1));
    settings->sections[(settings->num_sections)++] = s;
    settings->sections[settings->num_sections - 1]->num_values = 0;
    set_section_name(settings->sections[settings->num_sections - 1], name);
    settings->sections[settings->num_sections - 1]->keys = Malloc(sizeof(char*));
    settings->sections[settings->num_sections - 1]->values = Malloc(sizeof(char*));
}


/**
 * @brief creates a new key-value pair from a string containing the key and value
 * @param str string with the key-value pair
 * @return key-value pair
*/
static key_value_t new_key_value_pair(char* str)
{
    key_value_t kv;

    char* delim = strchr(str, '=');
    *delim = '\0';
    char* key = str;
    char* value = delim + 1;
    
    size_t length = strlen(value);
    if (value[length - 1] == '\n')
        value[length - 1] = '\0';
    else
        value[length] = '\0';
    kv.key = key;
    kv.value = value;

    return kv;
}


/**
 * @brief saves a new key-value pair in a section
 * @param s section to store the new key-value pair
 * @param key_value key-value pair to store
*/
static void set_key_value(section_t *s, key_value_t *key_value)
{
    s->keys = Realloc(s->keys, sizeof(char*) * (s->num_values + 1));
    s->keys[s->num_values] = Malloc(sizeof(char)*strlen(key_value->key) + 1);
    strcpy(s->keys[s->num_values], key_value->key);

    s->values = Realloc(s->values, sizeof(char*) * (s->num_values + 1));
    s->values[s->num_values] = Malloc(sizeof(char)*strlen(key_value->value) + 1);
    strcpy(s->values[s->num_values], key_value->value);   
}


/**
 * @brief parse a section name
 * @param name name of the section
 * @return 0 if section name format is correct or -1 on error
*/
static int parse_section_name(char *name)
{
    name++;
    char* delim = strchr(name, ']');
    if (delim == NULL)
        return -1;
    *delim = '\0';
    return 0;
}


/**
 * @brief parse a configuration ini file and store the key-value pairs in settings
 * @param settings settings_t to store the key-value pairs
 * @param file_name name of the ini file
*/
static int parse_file(settings_t *settings, const char *file_name)
{
    char buffer[128];
    FILE* fptr = fopen(file_name, "r");

    if (fptr == NULL)
        return -1;
    
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
        /* If line starts with ; or # is a comment*/
        if (buffer[0] == ';' || buffer[0] == '#')
            continue;
        /* Skip empty lines */
        else if (buffer[0] == '\n' || buffer[0] == '\r') {
            continue;
        }
        /* If line starts with is a new section */
        else if (buffer[0] == '[') {
            if (!parse_section_name(buffer)) {
                char *section_name = buffer + 1;
                initialize_section(settings, section_name);
                continue;
            }
            else {
                fprintf(stderr, "Invalid section name: %s\n", buffer);
                return -1;
            }
             
        }
        /* If line starts with '=' is invalid*/
        else if (buffer[0] == '=') {
            fprintf(stderr, "Invalid format: %s\n", buffer);
            return -1;
        }
        /* If line has a '=' char is a key-value pair */
        else if (strchr(buffer, '=') != NULL) {
            key_value_t key_value_pair = new_key_value_pair(buffer);
            set_key_value(settings->sections[settings->num_sections - 1], &key_value_pair);
            (settings->sections[settings->num_sections - 1]->num_values)++;
        }
        else {
            fprintf(stderr, "Invalid format: %s\n", buffer);
            return -1; 
        }
    }
    fclose(fptr);
    return 0;
}

/**
 * end of private functions
 */


/**
 * @brief free the data allocated by settings functions
 * @param settings settings_t to free
*/
void clean_settings(settings_t *settings)
{
    for (unsigned int i=0; i<settings->num_sections; i++) {
        for (unsigned int j = 0; j<settings->sections[i]->num_values; j++) {
            free(settings->sections[i]->keys[j]);
            settings->sections[i]->keys[j] = NULL;
            free(settings->sections[i]->values[j]);
            settings->sections[i]->values[j] = NULL;
        }
        free(settings->sections[i]->section_name);
        settings->sections[i]->section_name = NULL;
        free(settings->sections[i]->keys);
        settings->sections[i]->keys = NULL;
        free(settings->sections[i]->values);
        settings->sections[i]->values = NULL;
        free(settings->sections[i]);
        settings->sections[i] = NULL;
    }
    free(settings->sections);
    settings->sections = NULL;
    settings->num_sections = 0;
    free(settings);
    settings = NULL;
}


/**
 * @brief initialize settings from configuration ini file
 * @param filename name of configuration ini file
 * @return pointer to settings_t to store configurations
*/
settings_t* init_settings(char *filename)
{
    settings_t *settings = Malloc(sizeof(settings_t));
    settings->num_sections = 0;
    settings->sections = Malloc(sizeof (section_t *));
    if (parse_file(settings, filename) < 0) {
        fprintf(stderr, "init_settings error\n");
        free(settings->sections);
        free(settings);
        return NULL;
    }
    else
        return settings;
}


/**
 * @brief set a new value for a key-value pair
 * @param settings pointer to settings_t instance
 * @param key key name
 * @param value new value
 * @return pointer to new value
*/
char* set_value(settings_t *settings, char *key, char *value)
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


/**
 * @brief gets the value of a given key
 * @param settings pointer to settings_t instance
 * @param key key of the value to get
 * @return value of the given key
*/
char* get_value(settings_t *settings, char *key)
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