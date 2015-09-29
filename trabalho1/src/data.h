#define MAX_HASH 10

typedef enum {false, true} bool;
typedef char * String;
typedef void * Table;

typedef struct Data {
    String name;
    // In case of label:
    // >0: left
    // <0: right
    int value;
} Data; 

Table HashInit();
bool insert_instance (Table p, String name, int value);
void print_table(Table p);
int get_value(Table p, String name);
void free_table(Table p);

