#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct Proc{
    int procid;
    int fileid1;
    int fileid2;
    int visited;
    int revisited;
    int complete;
} Proc_t;

//Task 1
int num_comp(const void *a, const void *b);
int count_unique(int files[], int n);
//Task 2
int count_same(int files[], int n); //Calculates the execution time via highest number of same fileid2
//Tasks 3-5
void make_graph(Proc_t graph[], int graph_len, Proc_t procs[], int procs_len);
int find_deadlock(int deadlock_found, int fileid, int procs_len, Proc_t graph[], int graph_len);
int depth_search(int fileid1, Proc_t graph[], int graph_len, Proc_t revisited_procs[], int* revisited_procs_len);  //Using a dfs, find if a deadlock exists with the current fileid
int find_cycle(int dead_proc, int deadlocked_procs[], Proc_t revisited_procs[], int* revisited_procs_len); //Find the cycle within the array of recently visited processes
int min(int procs[], int procs_len);
void clear_revisited(Proc_t revisited_procs[], int* revisited_procs_len, Proc_t graph[]);
//Task 6
int find_match(int num, int array[], int array_len);
void print_proc_details(Proc_t procs, int time);

#endif
