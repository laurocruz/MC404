#define MAX_HASH 20

typedef enum {false, true} Boolean;
typedef char * String;
typedef void * Base;

typedef struct Rotulo {
    String name;
    unsigned short int mem_line;
} 

Base HashInit();
Boolean InsereBase (Base p, String name, unsigned short int mem_line);
void ImprimeBase(Base p);
unsigned short int ConsultaBase(Base p, String name);
