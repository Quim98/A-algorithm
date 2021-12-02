int Astar()
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
        struct node_l* next;
    };
    typedef struct node_l node_list;

    node_list *create_new_node(unsigned long int elem)
    {
        node_list *new = malloc(sizeof(node_list));
        new -> element = elem;
        new -> next = NULL;
        return new;
    }

    node_list *insert_at_head(node_list *head, node_list *elem)
    {
        elem -> next = head;
        return elem;
    }

    void insert_at_middle(node_list *head, node_list *elem, double cost_elem, AStarStatus stat[])
    {
        node_list *temp = head;
        node_list *temp_old;
        while ((stat[temp -> element].g + stat[temp -> element].h) <= cost_elem)
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
        double a,c;

        a = sin(M_PI/180*(lat_f-lat_i)/2)*sin(M_PI/180*(lat_f-lat_i)/2)+cos(M_PI/180*lat_i)*cos(M_PI/180*lat_f)*sin(M_PI/180*(lon_f-lon_i)/2)*sin(M_PI/180*(lon_f-lon_i)/2);
        c = 2 * atan2(sqrt(a), sqrt(1-a));
        return 6371007.1810*c;
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


    FILE *fin;
    unsigned long nnodes;
    node *node_inf;
    unsigned long int ntotnsucc=46181625;
    unsigned long int *allsuccessors;
    unsigned long int i,j;
    
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
        printf("Memory of Astar status  could not be reserved. Exiting the program. \n");
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
    unsigned long int open_n_elem = 1;
    unsigned long int current_node, final_node, parent_node, list_iterator, new_succ;
    node_list *tmp;
    node_list *open_head;
    node_list *problem; //BORRAR
    unsigned short int prob_happened = 0;

    // Add initial node to closed list
    current_node = binary_search ( node_inf, nnodes , 240949599);
    final_node = binary_search ( node_inf, nnodes , 195977239);
    status[current_node].g = 0;
    status[current_node].h = distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
    status[current_node].whq = OPEN;
    tmp = create_new_node(current_node);
    open_head = tmp;
    printf("Distance to objective: %lf \n",status[current_node].h);
    
    // While we don't find the final node
    while (open_n_elem != 0)
    {
        // Take the best node from open list to closed list and update open list
        open_n_elem -= 1;
        parent_node = open_head -> element;
        open_head = eliminate_head(open_head);
        status[parent_node].whq = CLOSED;
        //printf("Cost of parent node: %lf \n",(status[parent_node].g + status[parent_node].h));

        // If final node in closed list, break
        if (parent_node == final_node)
        {
            break;
        }

        new_succ = 0; //BORRAR

        // For the successors in the parent node
        for (i=0;i<node_inf[parent_node].nsucc;i++)
        {
            // Save the location of the successor node in the vectors: node_inf, status, open_queue (same for all 3)
            current_node = binary_search ( node_inf, nnodes , node_inf[parent_node].successors[i]);
            // If the node is already in the open list (OPEN) vs it's not (NONE). If the successor
            // node is in the closed list we do nothing
            //printf("Enter new successor in pos %lu \n",current_node);
            //printf("Number of elements in open: %lu \n", open_n_elem);
            if (status[current_node].whq == OPEN)
            {
                // We only do something if the new cost for the node is lower
                if (status[current_node].g > (status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon)))
                {
                    // Update cost and parents
                    //printf("Node already in open list and better cost \n");
                    status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                    status[current_node].parent = parent_node;

                    // Dequeue and enqueue of the node

                    // If the successor node is the head of the queue and now it has 
                    // a better cost we update the cost above and do nothing
                    if (current_node != (open_head -> element))
                    {
                        //printf("Before eliminated node in middle \n");
                        eliminate_at_middle(open_head, current_node);
                        //printf("Eliminated node in middle \n");

                        //printf("Eliminated node \n");
                        // Enqueue
                        tmp = create_new_node(current_node);
                        // If we need to change the linked list head
                        if ((status[open_head -> element].g + status[open_head -> element].h) >= (status[current_node].g + status[current_node].h))
                        {
                            //printf("Before eliminated node and put new head \n");
                            open_head = insert_at_head(open_head, tmp);
                            //printf("Eliminated node and put new head \n");
                        }
                        else
                        {
                            //printf("Before eliminated node and put node in open list \n");
                            insert_at_middle(open_head, tmp, (status[current_node].g + status[current_node].h), status);
                            //printf("Eliminated node and put node in open list \n");
                        }
                    }
                }
            }
            else if (status[current_node].whq == NONE)
            {
                //printf("Enter new node \n");
                new_succ += 1; //BORRAR
                // Update cost and parents
                status[current_node].g = status[parent_node].g + distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[parent_node].lat, node_inf[parent_node].lon);
                status[current_node].h = distance(node_inf[current_node].lat, node_inf[current_node].lon, node_inf[final_node].lat, node_inf[final_node].lon);
                status[current_node].parent = parent_node;
                status[current_node].whq = OPEN;

                tmp = create_new_node(current_node);
                //printf("Created new node \n");
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
                    //printf("Updated number of elements \n");
                    // If we need to change the linked list head
                    if ((status[open_head -> element].g + status[open_head -> element].h) >= (status[current_node].g + status[current_node].h))
                    {
                        //printf("Before put new head \n");
                        open_head = insert_at_head(open_head, tmp);
                        //printf("Put new head \n");
                    }
                    else
                    {
                        //printf("Before put node in open list \n"); // PETA AQUI (genera infinite loop perque el node 343686 s'apunta a si mateix amb mateixa memoria)
                        insert_at_middle(open_head, tmp, (status[current_node].g + status[current_node].h), status);
                        //printf("Put node in open list \n");
                    }
                }
            }
        }
    }
    printf("Final distance is: %lf \n",status[parent_node].g);
    
    return 0;
}