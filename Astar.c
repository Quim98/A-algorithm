#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    unsigned long int id;
    char *name;
    double lat,lon;
    unsigned short int nsucc;
    unsigned long int *successors;
}node;

typedef char Queue;
enum whichQueue {NONE, OPEN, CLOSED};

typedef struct{
    double g, h;
    unsigned long int parent;
    Queue whq;
}AStarStatus;

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "nnERROR: %s.nnStopping...nnnn", miss); exit(errcode);
}

double distance (double lat_i, double lon_i, double lat_f, double lon_f)
{
    double a,c;

    a = sin(M_PI/180*(lat_f-lat_i)/2)*sin(M_PI/180*(lat_f-lat_i)/2)+cos(M_PI/180*lat_i)*cos(M_PI/180*lat_f)*sin(M_PI/180*(lon_f-lon_i)/2)*sin(M_PI/180*(lon_f-lon_i)/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    return 6371000*c;
}

unsigned long int binary_search ( node nodes_inf[], unsigned long int size_nodes, unsigned long int searched_node )
{
    unsigned long int high = size_nodes-1;
    unsigned long int low = 0;
    unsigned long int middle = high/2;
    while ( nodes_inf[middle].id != searched_node )
    {
        if ( searched_node > nodes_inf[middle].id)
        {
            low = middle + 1;
            middle = low + (high - low)/2;
        }
        else
        {
            high = middle -1;
            middle = low + (high - low)/2;
            
        }
        if (low > high) 
        {
            return 4294967295; 
        }
        else if (high == 0)
        {
            return 4294967295;
        }
        else if (low == (size_nodes-1))
        {
            return 4294967295;
        }
    }
    return middle;
}

int main(int argc, char** argv)
{
    FILE *fin;
    unsigned long nnodes;
    node *node_inf;
    unsigned long int ntotnsucc=46181625;
    unsigned long int *allsuccessors;
    unsigned long int i;
    
    //LOADING DATA FROM BINARY FILE

    if ((fin = fopen ("spain.bin", "r")) == NULL)
    {
        ExitError("the data file does not exist or cannot be opened", 11);
    }

    /* Global data 􀀀􀀀􀀀 header */
    if( fread(&nnodes, sizeof(unsigned long), 1, fin) + fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
    {
        ExitError("when reading the header of the binary data file", 12);
    }
    
    /* getting memory for all data */
    if((node_inf = (node *) malloc(nnodes*sizeof(node))) == NULL)
    {
        ExitError("when allocating memory for the nodes vector", 13);
    }
    
    if( (allsuccessors = (unsigned long *) malloc(ntotnsucc*sizeof(unsigned long))) == NULL)
    {
        ExitError("when allocating memory for the edges vector", 15);
    }
    
    /* Reading all data from file */
    if( fread(node_inf, sizeof(node), nnodes, fin) != nnodes )
    {
        ExitError("when reading nodes from the binary data file", 17);
    }
    
    if(fread(allsuccessors, sizeof(unsigned long), ntotnsucc, fin) != ntotnsucc)
    {
        ExitError("when reading sucessors from the binary data file", 18);
    }
    
    fclose(fin);

    AStarStatus *status = (AStarStatus*)malloc(nnodes*sizeof(AStarStatus));
    unsigned long int *open_queue = (unsigned long int*)malloc(nnodes*sizeof(unsigned long int));
    if (open_queue == NULL || status == NULL)
    {
        printf("Memory of Astar status and/or open queue could not be reserved. Exiting the program. \n");
        return 1;
    }

    /* Setting pointers to successors and putting infinite to all nodes in linked list */
    for(i=0; i < nnodes; i++)
    {
        if(node_inf[i].nsucc)
        {
            node_inf[i].successors = allsuccessors;
            allsuccessors += node_inf[i].nsucc;
        }
        open_queue[i] = 4294967295;
        status[i].whq == NONE;
    }

    // A* ALGORITHM
    unsigned long int open_n_elem = 0;
    unsigned long int current_node, final_node, parent_node, open_head, list_iterator, list_iterator_old;

    // Add initial node to closed list
    current_node = binary_search ( node_inf, nnodes , 240949599);
    final_node = binary_search ( node_inf, nnodes , 195977239);
    status[current_node].g = 0;
    status[current_node].h = distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
    status[current_node].whq = CLOSED;
    parent_node = current_node;   
    
    // While we don't find the final node
    while (parent_node != final_node)
    {
        // For the successors in the parent node
        for (i=0;i<node_inf[parent_node].nsucc;i++)
        {
            // Save the location of the successor node in the vectors: node_inf, status, open_queue (same for all 3)
            current_node = binary_search ( node_inf, nnodes , node_inf[parent_node].successors[i]); 
            
            // If the node is already in the open list (OPEN) vs it's not (NONE). If the successor
            // node is in the closed list we do nothing
            if (status[current_node].whq == OPEN)
            {
                // We only do something if the new cost for the node is lower
                if (status[current_node].g > (status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon)))
                {
                    // Update cost and parents
                    status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                    status[current_node].h = 1.1552 * distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
                    status[current_node].parent = parent_node;

                    // Dequeue and enqueue of the node
                    if (current_node == open_head)
                    {
                        // If the successor node is the head of the queue and now it has 
                        // a better cost we update the cost above and do nothing
                    }
                    else
                    {
                        // Save the node that goes after the successor node in the
                        // open queue and dequeue the successor node
                        list_iterator_old = open_queue[current_node];
                        open_queue[current_node] = 4294967295;

                        // Begin iterating the open queue from the beggining until we reach
                        // the successor node that we want to dequeue
                        list_iterator = open_head;
                        while (open_queue[list_iterator] != current_node)
                        {
                            list_iterator = open_queue[list_iterator];
                            // If in the end of the linked list
                            if (list_iterator == 4294967295)
                            {
                                break;
                            }
                        }

                        // Finally fully dequeue the node
                        if (list_iterator != 4294967295)
                        {
                            open_queue[list_iterator] = list_iterator_old;
                        }
                        
                        // Enqueue
                        // If we need to change the linked list head
                        if ((status[open_head].g + status[open_head].h)>(status[current_node].g + status[current_node].h))
                        {
                            open_queue[current_node] = open_head;
                            open_head = current_node;
                        }
                        else
                        {
                            // Begin iterating the open queue from the beggining until we reach
                            // avnode that has more cost than the current successor node
                            list_iterator = open_head;
                            while((status[list_iterator].g + status[list_iterator].h)<(status[current_node].g + status[current_node].h))
                            {
                                list_iterator_old = list_iterator;
                                list_iterator = open_queue[list_iterator_old];
                                // If in the end of the linked list
                                if (list_iterator == 4294967295)
                                {
                                    break;
                                }
                            }
                            // Include the node in the queue in the current position
                            open_queue[list_iterator_old] = current_node;
                            open_queue[current_node] = list_iterator;
                        }
                    }
                }
            }
            else if (status[current_node].whq == NONE)
            {
                // Update cost and parents
                status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                status[current_node].h = 1.1552 * distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
                status[current_node].parent = parent_node;
                status[current_node].whq = OPEN;

                // Enqueue

                // If there are no elements in the open queue the successor will be the head
                if (open_n_elem == 0)
                {
                    open_head = current_node;
                    open_n_elem = 1;
                }
                else
                {

                    open_n_elem += 1;
                    // If we need to change the linked list head
                    if ((status[open_head].g + status[open_head].h)>(status[current_node].g + status[current_node].h))
                    {

                        open_queue[current_node] = open_head;
                        open_head = current_node;
                    }
                    else
                    {
                        // Begin iterating the open queue from the beggining until we reach
                        // a node that has more cost than the current successor node
                        list_iterator = open_head;
                        while((status[list_iterator].g + status[list_iterator].h)<(status[current_node].g + status[current_node].h))
                        {
                            list_iterator_old = list_iterator;
                            list_iterator = open_queue[list_iterator_old];
                            // If in the end of the linked list
                            if (list_iterator == 4294967295)
                            {
                                break;
                            }
                        }
                        // Include the node in the queue in the current position
                        open_queue[list_iterator_old] = current_node;
                        open_queue[current_node] = list_iterator;
                    }
                }
            }  
        }
        // Take the best node from open list to closed list and update open_list
        parent_node = open_head;
        open_head = open_queue[parent_node];
        open_queue[parent_node] = 4294967295;
        status[parent_node].whq = CLOSED;
    }
    printf("Final distance is: %lf \n",status[parent_node].g);
    return 0;
}