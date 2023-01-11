#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

/*
num_comp is the compare function used for qsort
inputs: two integer values
outputs: higher value
*/
int
num_comp(const void *a, const void *b){
    int *left = (int*)a;
    int *right = (int*)b;
    return *left-*right;
}

/*
count_unique counts the number of unique files
inputs: files[] = sorted array of all files
        n = length of the sorted array of all files
outputs: the number of unique files
*/
int
count_unique(int files[], int n){
    int num_unique = 0;
    for (int i=0; i<n; i++){
        while (i < n-1){
            if (files[i] == files[i+1]){
                i++;
            }
            else{
                break;
            }
        }
        num_unique++;
    }
    return num_unique;
}

/*
count_same is used to find the number of duplicate fileids in requested_files
inputs: files[] = sorted requested_files
        n = length of array of sorted requested_files
outpus: number of duplicate fileids
*/
int
count_same(int files[], int n){
    int max_same_num = 1;
    for (int i=0; i<n; i++){
        int current_same_num = 1;
        while (i < n-1){
            if (files[i] == files[i+1]){
                current_same_num++;
                i++;
            }
            else{
                break;
            }
        }
        if (current_same_num > max_same_num){
            max_same_num = current_same_num;
        }
    }
    return max_same_num;
}

/*
find_deadlock uses depth_search to find if a deadlock exists on the current
process. It also creates and manages recently visited processes (revisited_procs[])
inputs: deadlock_found = boolean
        fileid = fileid1 of the current process
        procs_len = length of procs[]
        graph[] = adjacency list representation of procs[]
        graph_len = length of the graph
outputs: deadlock_found = 1 if deadlock has been found
                          0 if not
*/
int
find_deadlock(int deadlock_found, int fileid, int procs_len, Proc_t graph[], int graph_len){
    Proc_t revisited_procs[procs_len]; //Keep track of which nodes have been recently visited
    int *revisited_procs_len = (int*)malloc(sizeof(int));
    if (revisited_procs_len == NULL){
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }
    *revisited_procs_len = 0;
    if (!graph[fileid].visited){
        int dead_proc; //dfs the process to see if it is part of a deadlock
        dead_proc = depth_search(fileid, graph, graph_len, revisited_procs, revisited_procs_len);
        //Revisited_procs now contains the deadlock cycle
        if (dead_proc > -1){
            if (!deadlock_found){
                deadlock_found = 1;
                printf("Deadlock detected\n");
                printf("Terminate");
            }
            //Task 4: Terminate process to unlock deadlock
            //Task 5: Terminate processes to unlock deadlocks
            int deadlocked_procs[*revisited_procs_len];
            int deadlocked_procs_len = find_cycle(dead_proc, deadlocked_procs, revisited_procs, revisited_procs_len);
            printf(" %d", min(deadlocked_procs, deadlocked_procs_len));
        }
        clear_revisited(revisited_procs, revisited_procs_len, graph);
    }
    free(revisited_procs_len);
    return deadlock_found;
}

/*
depth_search recursively calls itself to find if the current process has been
recently visited (.revisited = 1), resulting in a deadlock. It also preemptively
returns if the next process has already been visited, or if the fileid is out
of bounds or irrelevant.
We mark each visited process as both visited and recently visited in graph[].
Revisited_procs[] is updated so we know which processes have been recently
visited, and so that we can remove the revisited marker later on as well.
inputs: fileid = fileid1 of the current process
        graph[] = adjacency list representation of procs[]
        graph_len = length of the graph
        revisited_procs[] = processes that have been recently visited
        revisited_procs_len = pointer to length of revisited_procs
outputs: -1 for no deadlock found, preemptive return
         procid`of the process which started the deadlock
*/
int
depth_search(int fileid1, Proc_t graph[], int graph_len, Proc_t revisited_procs[], int* revisited_procs_len){
    graph[fileid1].visited = 1;
    graph[fileid1].revisited = 1;

    revisited_procs[*revisited_procs_len] = graph[fileid1];
    *revisited_procs_len += 1;

    int next_node = graph[fileid1].fileid2;
    if (graph[next_node].procid < 0 || next_node > graph_len){
        return -1;
    }
    if (graph[next_node].revisited){ //Deadlock has been found
        return graph[next_node].procid; //Return the process that begins the deadlock
    }
    if (graph[next_node].visited){
        return -1;
    }
    return depth_search(next_node, graph, graph_len, revisited_procs, revisited_procs_len);
}

/*
make_graph simply creates and populates graph[] using procs[] and filling the
rest with a null process.
inputs: graph[] = adjacency list representation of procs[]
        graph_len = length of the graph
        procs[] = array of all processes
        procs_len = length of procs[]
outputs: void
*/
void
make_graph(Proc_t graph[], int graph_len, Proc_t procs[], int procs_len){
    Proc_t null_proc = {
        .procid = -1,
        .fileid1 = -1,
        .fileid2 = -1,
        .visited = 0,
        .revisited = 0,
        .complete = 0,
    };
    for (int i=0; i<graph_len; i++){
        graph[i] = null_proc;
    }
    for (int i=0; i<procs_len; i++){
        graph[procs[i].fileid1] = procs[i];
    }
}

/*
find_cycle goes through revisited_procs to see where the cycle started and
puts it into deadlocked_procs[]
inputs: dead_proc = procid of the first process in the deadlock
        deadlocked_procs = array containing a subset of revisited_procs[]
        revisited_procs[] = array of recently visited processes
        revisited_procs_len = length of revisited_procs
outputs: deadlocked_procs_len = length of deadlocked_procs
*/
int
find_cycle(int dead_proc, int deadlocked_procs[], Proc_t revisited_procs[], int* revisited_procs_len){
    int deadlocked_procs_len = 0;
    for (int i=0; i<*revisited_procs_len; i++){
        if (revisited_procs[i].procid == dead_proc){
            for (int j=i; j<*revisited_procs_len; j++){
                deadlocked_procs[j-i] = revisited_procs[j].procid;
                deadlocked_procs_len++;
            }
            break;
        }
    }
    return deadlocked_procs_len;
}

/*
min returns the smallest value in an array
inputs: procs[] = array of all processes
        procs_len = length of procs[]
outputs: min = minimum value in the array
*/
int
min(int procs[], int procs_len){
    int min = procs[0];
    for (int i=0; i<procs_len; i++){
        if (procs[i] < min){
            min = procs[i];
        }
    }
    return min;
}

/*
clear_revisited removes all the revisited markers in the graph.
inputs: revisited_procs[] = recently visited processes
        revisited_procs_len = length of revisited_procs[]
        graph[] = adjacency list representation of procs[]
outputs: void
*/
void
clear_revisited(Proc_t revisited_procs[], int* revisited_procs_len, Proc_t graph[]){
    for (int i=0; i<*revisited_procs_len; i++){
        graph[revisited_procs[i].fileid1].revisited = 0;
    }
}

/* find_match goes through an array to see if num is in the array
inputs: num = number to search for
        array[] = array to sift through
        array_len = length of array[]
outputs: 0 if num not found in array
         1 if num found in array
*/
int
find_match(int num, int array[], int array_len){
    for (int i=0; i<array_len; i++){
        if (array[i] == num){
            return 1;
        }
    }
    return 0;
}

/*
print_proc_details prints out the procid, fileid1 and fileid2 at each time interval
inputs: proc = the current process
        time = the current time
outputs: void
*/
void
print_proc_details(Proc_t proc, int time){
    printf("%d %d %d,%d\n", time, proc.procid, proc.fileid1, proc.fileid2);
}
