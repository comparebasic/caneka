#define MAX_CONN 256
#define SOCK_PATH_MAX 512
#define BACKLOG 128
#define BUFF_SLOTS 64;

typedef struct simplestr {
    uint16_t typeOf; 
    uint16_t status; 
    int length;
    unsigned char *content;
} SimpleStr;

typedef struct simplebuff {
    uint16_t typeOf; 
    uint16_t status; 
    SimpleStr *slots[BUFF_SLOTS];
} SimpleBuff;

typedef struct simplevisor {
    uint16_t typeOf; 
    uint16_t status; 
    char *dir;
    char *name;
    char **cmd;
    int code;
    struct pollfd sock_pfd;
    int next_slot;
    int max_slot;
    struct pollfd poll_pfds[MAXX_CONN];
    SimpleBuff buffs[MAXX_CONN];
} SimpleVisor;

extern SimpleVisor _globalVisor;

void Spawn(SimpleVisor *visor);
SimpleVisor *Spawn_GetGlobal();
