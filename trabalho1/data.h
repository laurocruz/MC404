#define MAX_HASH 20

typedef enum {false, true} Boolean;
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
Boolean insert_instance (Table p, String name, int value);
void print_table(Table p);
int get_value(Table p, String name);
