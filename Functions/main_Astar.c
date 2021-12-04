    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include "load_data.h"
    #include "Astar.h"
void main()
{
    // load_data needs the file spain.csv in the same folder as the file main_Astar.c
    if (load_data() == 0)
    {
        // Call Astar function if no error in the load_data() function appears.
        // Astar(initial_node_id, final_node_id)
        Astar(240949599,195977239);
    }
    else
    {
        printf("Astar was not executed due to an error in the load_data function. \n");
    }
    return;
}
