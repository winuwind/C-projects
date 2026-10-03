extern void *addr;
extern unsigned count_pages;
extern unsigned heap_size;
extern char initial_flag;
extern unsigned page_size;

int init();

int finalize();

void *my_malloc(unsigned size);

int my_free(void *pointer);