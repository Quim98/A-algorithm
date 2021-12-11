int Astar(unsigned long int initial_id, unsigned long int final_id)
{
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

    struct node_l{
        unsigned long int element;
        double cost;
        struct node_l* next;
    };
    typedef struct node_l node_list;

    node_list *create_new_node(unsigned long int elem, double cost_node)
    {
        node_list *new = malloc(sizeof(node_list));
        new -> element = elem;
        new -> next = NULL;
        new -> cost = cost_node;
        return new;
    }

    node_list *insert_at_head(node_list *head, node_list *elem)
    {
        elem -> next = head;
        return elem;
    }

    void insert_at_middle(node_list *head, node_list *elem, AStarStatus stat[])
    {
        node_list *temp = head;
        node_list *temp_old;
        while ((temp -> cost) <= (elem -> cost))
        {
            temp_old = temp;
            temp = temp_old -> next;
            if (temp == NULL)
            {
                break;
            }
        }
        temp_old -> next = elem;
        elem -> next = temp;
    }

    node_list *eliminate_head(node_list *head)
    {
        node_list *new_head;
        new_head = head -> next;
        free(head);
        return new_head;
    }

    void eliminate_at_middle(node_list *head, unsigned long int elem)
    {
        node_list *temp = head;
        node_list *temp_old;
        while ((temp -> element) != elem)
        {
            temp_old = temp;
            temp = temp -> next;
        }
        temp_old -> next = temp -> next;
        temp -> next = NULL;
        free(temp);
    }

    void ExitError(const char *miss, int errcode)
    {
        fprintf (stderr, "nnERROR: %s.nnStopping...nnnn", miss); exit(errcode);
    }
    
    double distance (double lat_i, double lon_i, double lat_f, double lon_f)
    {
        double x,y;
        x = M_PI/180*(lon_f - lon_i) * cos(M_PI/180*(lat_f + lat_i)/2);
        y = M_PI/180*(lat_f - lat_i);
        return sqrt(x*x + y*y)*6371007.1810;
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
            else if (nodes_inf[middle].id != searched_node)
            {    
                if (high == 0)
                {
                    return 4294967295;
                }
                else if (low == (size_nodes-1))
                {
                    return 4294967295;
                }
            }
        }
        return middle;
    }

    printf("Loading data from binary file. \n");
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
    if (status == NULL)
    {
        printf("Memory of Astar status could not be reserved. Exiting the program. \n");
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
        status[i].whq == NONE;
    }

    // A* ALGORITHM
    printf("Binary file data loading finished. Astar algorithm begins. \n");
    unsigned long int open_n_elem = 1;
    unsigned long int current_node, final_node, parent_node;
    node_list *tmp;
    node_list *open_head;

    // Add initial node to closed list
    current_node = binary_search ( node_inf, nnodes , initial_id);
    final_node = binary_search ( node_inf, nnodes , final_id);
    status[current_node].g = 0;
    status[current_node].h = distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
    status[current_node].whq = OPEN;
    tmp = create_new_node(current_node, (status[current_node].g+status[current_node].h));
    open_head = tmp;
    
    // While we don't find the final node
    while (open_n_elem != 0)
    {
        // Take the best node from open list to closed list and update open list
        open_n_elem -= 1;
        parent_node = open_head -> element;
        open_head = eliminate_head(open_head);
        status[parent_node].whq = CLOSED;

        // If final node in closed list, break
        if (parent_node == final_node)
        {
            printf("Final distance is: %lf \n",status[parent_node].g);
            break;
        }

        // For the successors in the parent node
        for (i=0;i<node_inf[parent_node].nsucc;i++)
        {
            // Save the location of the successor node in the vectors: node_inf, status, open_queue (same for all 3)
            current_node = node_inf[parent_node].successors[i];
            // If the node is already in the open list (OPEN) vs it's not (NONE)
            // If the successor node is in the closed list we do nothing
            if (status[current_node].whq == OPEN)
            {
                // We only do something if the new cost for the node is lower
                if (status[current_node].g > (status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon)))
                {
                    // Update cost and parents
                    status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                    status[current_node].parent = parent_node;

                    // Dequeue and enqueue of the node

                    if (current_node != (open_head -> element))
                    {
                        eliminate_at_middle(open_head, current_node);

                        // Enqueue
                        tmp = create_new_node(current_node, (status[current_node].g+status[current_node].h));
                        // If we need to change the linked list head
                        if ((open_head -> cost) >= (tmp -> cost))
                        {
                            open_head = insert_at_head(open_head, tmp);
                        }
                        else
                        {
                            insert_at_middle(open_head, tmp, status);
                        }
                    }
                    else
                    {
                        // If the successor node is the head of the queue and now it has 
                        // a better cost we update the cost and do nothing
                        open_head -> cost = status[current_node].g + status[current_node].h;
                    }
                }
            }
            else if (status[current_node].whq == NONE)
            {
                // Update cost and parents
                status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                status[current_node].h = distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
                status[current_node].parent = parent_node;
                status[current_node].whq = OPEN;

                tmp = create_new_node(current_node, (status[current_node].g+status[current_node].h));

                // Enqueue

                // If there are no elements in the open queue the successor will be the head
                if (open_n_elem == 0)
                {
                    open_head = tmp;
                    open_n_elem = 1;
                }
                else
                {
                    open_n_elem += 1;
                    // If we need to change the linked list head
                    if ((open_head -> cost) >= (tmp -> cost))
                    {
                        open_head = insert_at_head(open_head, tmp);
                    }
                    else
                    {
                        insert_at_middle(open_head, tmp, status);
                    }
                }
            }
        }
    }
    if (parent_node == final_node)
    {
        // If the search is successful we create the output file with the path
        unsigned long int list_iterator;
        unsigned long int path_number = 1;
        unsigned long int *path = malloc(sizeof(unsigned long int));
        FILE *final_file;
        char name[257];
        strcpy(name, "path.csv");
        final_file = fopen(name,"wb");
        if (final_file == NULL)
        {
            printf("Failure at creating file path.csv created. Exiting the program. \n");
            return 0;
        }

        fprintf(final_file,"Node ID,Distance from beggining,Latitude,Longitude \n");
        
        current_node = binary_search ( node_inf, nnodes , initial_id); //Initial node
        list_iterator = final_node;
        path[path_number-1] = list_iterator;
        while (list_iterator != current_node)
        {
            list_iterator = status[list_iterator].parent;
            path_number += 1;
            path = (unsigned long int*)realloc(path,path_number*sizeof(unsigned long int));
            path[path_number-1] = list_iterator;
        }

        for(i=path_number; i>0;i-=1)
        {
            fprintf(final_file,"%lu,%lf,%lf,%lf\n", node_inf[path[i-1]].id, status[path[i-1]].g, node_inf[path[i-1]].lat, node_inf[path[i-1]].lon);
        }
        fclose(final_file);
        printf("File path.csv created. Exiting the program. \n");
        return 0;
    }
    else
    {
        printf("Final node not found. Exiting the program. \n");
        return 1;
    }
}