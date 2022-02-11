/************************************************************************
 * ECE7610 Final Project
 * Developer: Mostafa AlNaimi
 * Project: Prim's MST - sequential.
 *
 *************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>
 
 /* Define number of vertices in the mst_graph */
#define V 6
  
 /* Find the vertex with minimum weight_pick value */
int min_weight(int weight_pick[], bool mst_Set[])
{
    /* Initialize min value */
    int min = INT_MAX, min_index;
  
    for (int v = 0; v < V; v++)
        if (mst_Set[v] == false && weight_pick[v] < min)
            min = weight_pick[v], min_index = v;
  
    return min_index;
}

int print_MST(int parent[], int mst_graph[V][V], double time_diff)
{
    printf ("Execution time = %lf \n", time_diff);
    printf("Edge \tWeight\n");
    for (int i = 1; i < V; i++)
        printf("%d - %d \t%d \n", parent[i], i, mst_graph[i][parent[i]]);
}
  
 /* Construct MST using Prim's method. Print the Final matrix */
void prim_MST_seq(int mst_graph[V][V])
{
    struct timeval tv;		/* Time spec */
    double t1, t2, time_diff;
    
    /* An array to store constructed MST
     * Values to pick minimum weight edge
     * Represent set of vertices in MST
     */
    int parent[V];
    int weight_pick[V];
    bool mst_Set[V];
  
    gettimeofday(&tv, NULL);
    t1 = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
    
    /* Initialize all weights as INFINITE */
    for (int i = 0; i < V; i++){
        weight_pick[i] = INT_MAX;
        mst_Set[i] = false;
    }
    
    /* Always include first 1st vertex in MST.
    /* First node is always root of MST */
    weight_pick[0] = 0;
    parent[0] = -1;  
  
    for (int i = 0; i < V - 1; i++) {
        
        /* Pick the minimum weight_pick vertex */
        int u = min_weight(weight_pick, mst_Set);
  
        /* Add the picked vertex to the MST Set */
        mst_Set[u] = true;
  
        /* Update weight_pick value and parent index of
           the adjacent vertices of the picked vertex*/
         
        for (int v = 0; v < V; v++)
  
            if (mst_graph[u][v] && mst_Set[v] == false && mst_graph[u][v] < weight_pick[v])
                parent[v] = u, weight_pick[v] = mst_graph[u][v];
    }
    
    gettimeofday(&tv, NULL);
    t2 = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;
    
    time_diff = t2 - t1;
  
    /* print the MST */
    print_MST(parent, mst_graph, time_diff);
}


int main()
{
    /* Use the Graph 10.5 in the book */
    int mst_graph[V][V] = { { 0, 1, 3, 0, 0, 3 },
                            { 1, 0, 5, 1, 0, 0 },
                            { 3, 5, 0, 2, 1, 0 },
                            { 0, 1, 2, 0, 4, 0 },
                            { 0, 0, 1, 4, 0, 5 },
                            { 2, 0, 0, 0, 5, 0 } };
  
    /* Analyze the Minimum Spanning Tree graph */
    prim_MST_seq(mst_graph);
  
    return 0;
}
