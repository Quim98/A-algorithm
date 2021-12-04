int load_data ()
{
    typedef struct{
        unsigned long int id;
        char *name;
        double lat,lon;
        unsigned short int nsucc;
        unsigned long int *successors;
    }node;

    void ExitError(const char *miss, int errcode)
    {
        fprintf (stderr, "nnERROR: %s.nnStopping...nnnn", miss); 
        exit(errcode);
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

    void put_successor ( node nodes_inf[], unsigned long int source, unsigned long int destination )
    {
        unsigned long int vector;
        unsigned int k=0;
        unsigned long int size_nodes = 23895681;
        // Search the element of the source node

        // Search if the destination node is already in the vector
        for (k=0;k<nodes_inf[source].nsucc;k++)
        {
            vector = nodes_inf[source].successors[k];
            if (vector==destination)
            {
                return;
            }
        }

        // Include memory if needed
        nodes_inf[source].nsucc += 1;
        if ( nodes_inf[source].nsucc > 2 )
        {
            nodes_inf[source].successors = (unsigned long int*)realloc(nodes_inf[source].successors,nodes_inf[source].nsucc*sizeof(unsigned long int));
        }
        // Add the successor
        nodes_inf[source].successors[nodes_inf[source].nsucc - 1] = destination;
        return;
    }

    int isnode, isway, isoneway, contador;
    char *trash;
    char *first;
    unsigned long int node_count = 0;
    unsigned long int edges[10000], current_edge;
    unsigned long int i,h,p;
    unsigned int j,m,n;
    size_t size = 79857;
    char *buffer = (char*) malloc (size*sizeof(char));
    if (buffer == NULL)
    {
        printf("Memory of buffer could not be reserved. Exiting the program. \n");
        return 1;
    }
    FILE *myfile;
    node *node_inf = (node*)malloc(23895681*sizeof(node));
    if (node_inf == NULL)
    {
        printf("Memory of nodes could not be reserved. Exiting the program. \n");
        return 1;
    }

    myfile = fopen("spain.csv","r");
    if (myfile == NULL)
    {
        printf ("File was not found. \n");
        return 1;
    }
    else
    {
        printf ("Success at loading the file. \n");
    }

    getline (&buffer, &size, myfile);
    getline (&buffer, &size, myfile); // Discard first 3 rows
    getline (&buffer, &size, myfile);
    i=3;
    for (i=4;i<25353790;i++)
    {
        getline (&buffer, &size, myfile);
        first = strsep (&buffer,"|");
        isnode = strcmp(first,"node"); // If the line that we are reading is a node the variable isnode = 0
        isway = strcmp(first,"way"); // If the line that we are reading is a way the variable isway = 0
        if (isnode==0)
        {
            // Save the id and name of the node
            node_inf[node_count].id = strtoul(strsep (&buffer,"|"),&trash,10);
            node_inf[node_count].name = strsep(&buffer,"|");

            for (j=0;j<6;j++)
            {
                trash = strsep(&buffer,"|"); // Columns that we don't want
            }

            node_inf[node_count].lat = atof(strsep (&buffer,"|"));
            node_inf[node_count].lon = atof(strsep (&buffer," "));
            node_inf[node_count].nsucc = 0;
            // We assign a default vector size of 2 by reserving 2 times the memory needed for our data type
            node_inf[node_count].successors = (unsigned long int*)malloc(2*sizeof(unsigned long int));
            if ( node_inf[node_count].successors == NULL )
            {
                printf("Error at assigning memory in a successors vector. Exiting the program. \n");
                return 1;
            }
            node_count += 1;
        }
        else if (isway==0)
        {
            for (j=0;j<6;j++)
            {
                trash = strsep (&buffer,"|"); // Lines that we don't want
            }
            first = strsep (&buffer,"|");
            isoneway = strcmp(first,"oneway");
            if (isoneway==0)
            {
                trash = strsep (&buffer,"|");
                first = strsep (&buffer,"|");
                contador = 0;
                while (first != NULL)
                {          
                    current_edge = binary_search ( node_inf, 23895681, strtoul(first,&trash,10));
                    if (current_edge != 4294967295)
                    {
                        edges[contador]=current_edge;
                        contador = contador + 1;
                    }
                    first = strsep (&buffer,"|");
                }    
                if (contador > 1)
                {
                    for (n=0;n<(contador-1);n=n+1)
                    {
                        put_successor ( node_inf, edges[n] , edges[n+1] );
                    }
                }
            }
            else
            {
                trash = strsep (&buffer,"|");
                first = strsep (&buffer,"|");
                contador = 0;
                while (first != NULL)
                {        
                    current_edge = binary_search ( node_inf, 23895681, strtoul(first,&trash,10));
                    if (current_edge != 4294967295)
                    {
                        edges[contador]=current_edge;
                        contador = contador + 1;
                    }
                    first = strsep (&buffer,"|");
                }   
                if (contador > 1)
                {
                    for (n=0;n<(contador-1);n=n+1)
                    {
                        put_successor ( node_inf, edges[n] , edges[n+1] );
                        put_successor ( node_inf, edges[n+1] , edges[n] );
                    }
                }
            }
        }
        else
        {
            break;
        }
    }
    printf("File reading finished. Generating binary file. \n"); 
    fclose(myfile);
    
    FILE *fin;
    unsigned long nnodes = 23895681UL;
    char name[257];
    
    unsigned long int n0 = 0UL;// Borrar
    unsigned long int n1,n2,n3,n4,n5,n6,n7,n8,n9 = 0UL;// Borrar
    

    // Computing the total number of successors
    unsigned long ntotnsucc=0UL;
    for(i=0; i < nnodes; i++)
    {
        ntotnsucc += node_inf[i].nsucc;
    }

    // Setting the name of the binary file 
    strcpy(name, "spain.csv");
    strcpy(strrchr(name, '.'), ".bin");

    if ((fin = fopen (name, "wb")) == NULL)
    {
        ExitError("the output binary data file cannot be opened", 31);
    }

    // Global data --- header 
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
    {
        ExitError("when initializing the output binary data file", 32);
    }
    // Writing all nodes
    if( fwrite(node_inf, sizeof(node), nnodes, fin) != nnodes )
    {
        ExitError("when writing nodes to the output binary data file", 32);
    }

    // Writing sucessors in blocks
    for(i=0; i < nnodes; i++) 
    {
        if(node_inf[i].nsucc)
        {
            if( fwrite(node_inf[i].successors, sizeof(unsigned long), node_inf[i].nsucc, fin) != node_inf[i].nsucc)
            {
                ExitError("when writing edges to the output binary data file", 32);
            }
        
        }
    }
    fclose(fin);
    printf("Binary file generated. \n"); 
    
    return 0;
}