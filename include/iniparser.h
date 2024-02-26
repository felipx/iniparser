// iniparser.h

#ifndef INIPARSER_
#define INIPARSER_


/*************
 * Structures
 *************/

/**
 * @defgroup struct Structures
 * structures definitions
 * @{
 */

/**
 * @struct section_t
 * @brief structure containing key-value pairs
*/
typedef struct {
    unsigned int num_values;
    char *section_name;
    char **keys;
    char **values;
} section_t;

/**
 * @struct settings_t
 * @brief structure containing the settings of the ini configuration file
*/
typedef struct {
    unsigned int num_sections;
    section_t **sections;
} settings_t;

/**
 * @}
 */

/*************************
 * Functions declarations
 *************************/

/**
 * @defgroup settings_t instance
 * settings allocation functions
 * @{
 */

/**
 * @brief free the data allocated by settings functions
 * @param settings settings_t to free
*/
void clean_settings(settings_t *settings);

/**
 * @brief initialize settings from configuration ini file
 * @param filename name of configuration ini file
 * @return pointer to settings_t to store configurations
*/
settings_t* init_settings(char *filename);

/**
 * @}
 */

/**
 * @defgroup key-value
 * key-value pairs management functions
 * @{
 */

char* set_value(settings_t *, char *, char *);

/**
 * @brief gets the value of a given key
 * @param settings pointer to settings_t instance
 * @param key key of the value to get
 * @return value of the given key
*/

/**
 * @brief set a new value for a key-value pair
 * @param settings pointer to settings_t instance
 * @param key key name
 * @param value new value
 * @return pointer to new value
*/
char* get_value(settings_t *, char *);

/**
 * @}
 */

#endif /* INIPARSER_ */