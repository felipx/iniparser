// iniparser.h

#ifndef INIPARSER_
#define INIPARSER_

typedef struct settings* settings_t;

extern settings_t (*init_settings) (settings_t, char*);
extern char* (*set_value) (struct settings*, char*, char*);
extern char* (*get_value) (struct settings*, char*);
extern void (*clear_settings) (struct settings*);

#endif /* INIPARSER_ */