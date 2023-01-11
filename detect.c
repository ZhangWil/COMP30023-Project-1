#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char const *argv[]) {
    int procs_len = 0;
    int procs_size = 2;
    Proc_t* procs = malloc(sizeof(Proc_t) * procs_size);
    if (procs == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }

    char* filename;
    int exec_time_required = 0;
    int challenge_accepted = 0;
    int i;
    for (i=0; i<argc; i++){
        if (!strcmp(argv[i], "-e")){
            exec_time_required = 1;
        }
        if (!strcmp(argv[i], "-f")){
            filename = (char*)argv[i+1];
        }
        if (!strcmp(argv[i], "-c")){
            challenge_accepted = 1;
        }
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        if (!challenge_accepted){
            printf("Processes 0\n");
            printf("Files 0\n");
            if (exec_time_required){
                printf("Execution time 0\n");
            }
            else{
                printf("No deadlocks\n");
            }
        }
        else {
            printf("Simulation time 0\n");
        }
        exit(EXIT_FAILURE);
    }

    //Read the file and dynamically allocate memory to populate the array
    int procid, fileid1, fileid2;
    while (fscanf(fp, "%d %d %d", &procid, &fileid1, &fileid2) == 3) {
        if (procs_len == procs_size) {
            procs_size *= 2;
            procs = realloc(procs, sizeof(Proc_t) * procs_size);
            if (procs == NULL) {
            fprintf(stderr, "Realloc failure\n");
            exit(EXIT_FAILURE);
            }
        }
        procs[procs_len].procid = procid;
        procs[procs_len].fileid1 = fileid1;
        procs[procs_len].fileid2 = fileid2;
        procs[procs_len].visited = 0;
        procs[procs_len].revisited = 0;
        procs[procs_len].complete = 0;
        procs_len++;
    }

    //Process the information into separate arrays and sort them
    int locked_files[procs_len];
    int required_files[procs_len];
    int all_files[2*procs_len];
    for (i=0; i<procs_len; i++){
        locked_files[i] = procs[i].fileid1;
        required_files[i] = procs[i].fileid2;
        all_files[2*i] = procs[i].fileid1;
        all_files[2*i+1] = procs[i].fileid2;
    }
    qsort(locked_files, procs_len, sizeof(int),num_comp);
    qsort(required_files, procs_len, sizeof(int),num_comp);
    qsort(all_files, procs_len*2, sizeof(int),num_comp);

    //Task 1: Systems statistics: Processes and files
    if (!challenge_accepted){
        printf("Processes %d\n", procs_len);
        printf("Files %d\n", count_unique(all_files, procs_len*2));
    }
    //Task 2: Execution time
    if (exec_time_required){
        if (procs_len == 0){
            printf("Execution time 0\n");
        }
        else {
            printf("Execution time %d\n", count_same(required_files, procs_len)+1);
        }
    }
    //Task 6: Challenge
    else if (challenge_accepted){
        int time = 0;
        int procs_remaining = procs_len;
        while (procs_remaining > 0){
            int unavailable_files[procs_len];
            int unavailable_files_len = 0;
            for (i=0; i<procs_len; i++){
                if (!procs[i].complete){
                    if (!find_match(procs[i].fileid1, unavailable_files, unavailable_files_len) &&
                    !find_match(procs[i].fileid2, unavailable_files, unavailable_files_len)){
                        unavailable_files[unavailable_files_len] = procs[i].fileid1;
                        unavailable_files[unavailable_files_len+1] = procs[i].fileid2;
                        unavailable_files_len += 2;

                        print_proc_details(procs[i], time);
                        procs[i].complete = 1;
                        procs_remaining--;
                    }
                }
            }
            unavailable_files_len = 0;
            time++;
        }
        printf("Simulation time %d\n", time);
    }
    else {
        //Task 3-5: Detect presence of deadlocks and terminate process(es)
        if (procs_len){
            int graph_len = locked_files[procs_len-1]; //Length of graph is the highest fileid1
            Proc_t graph[graph_len];
            make_graph(graph, graph_len, procs, procs_len);
            int deadlock_found = 0;
            //Go through each process to see if they are part of a deadlock
            for (i=0; i<procs_len; i++){
                deadlock_found = find_deadlock(deadlock_found, procs[i].fileid1, procs_len, graph, graph_len);
            }
            if (!deadlock_found){
                printf("No deadlocks\n");
            }
        }
        else{
            printf("No deadlocks\n");
        }

    }
    free(procs);
    fclose(fp);
    return 0;
}
