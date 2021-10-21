#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    unsigned long int id;
    char *name;
    double lat,lon;
    unsigned short int nsucc = 0;
    unsigned long int *successors;
}node;


unsigned long int binary_search ( node nodes_inf, int size_nodes, int searched_node )
{
    int middle = (size_nodes-1)/2;
    int search_size = size_nodes-1;
    while ( node_inf[middle].id != searched_node )
    {
        if ( searched_node > node_inf[middle].id)
        {
            search_size=search_size/2;
            middle = middle + search_size/2;
        }
        else if ( searched_node < node_inf[middle].id)
        {
            search_size=search_size/2;
            middle = middle - search_size/2;
        }
    }
    return middle;
}

void put_successor ( node nodes_inf, unsigned long int source, unsigned long int destination )
{
    unsigned long int source_element;
    int k;

    // Search the element of the source node
    source_element = binary_search ( nodes_inf, 23895681, source );

    // Search if the destination node is already in the vector
    for (k=0;k<nodes_inf[source_element].nsucc;k++)
    {
        if (*(nodes_inf[source_element].successors+k) == destination)
        {
            return;
        }
    }

    // Include memory if needed
    nodes_inf[source_element].nsucc += 1;
    if ( nodes_inf[source_element].nsucc > 2 )
    {
        node_inf[source_element].successors = (unsigned long int*)realloc(successors,nsucc*sizeof(unsigned long int));
    }

    // Add the successor
    *(nodes_inf[source_element].successors + (nodes_inf[source_element].nsucc - 1)) = destination;

}

int main()
{
    FILE *myfile;
    size_t size;
    char *buffer;
    int bytes_read;
    unsigned long int row_num = 50707580; 
    unsigned long int node_count = 0;
    unsigned long int node_num = 23895681;
    unsigned long int i;
    unsigned int j;
    node node_inf[node_num];
    char *trash;
    char *first;
    char *last;

    
    myfile = fopen("spain.csv","r");
    if (myfile == NULL)
    {
        printf ("File was not found. \n");
        return 0;
    }
    else
    {
        printf ("Success at loading the file. \n");
    }

    getline (&buffer, &size, myfile);
    getline (&buffer, &size, myfile); /* Discard first 3 rows */
    getline (&buffer, &size, myfile);

    for (i=3;i<row_num;i++)
    {
        getline (&buffer, &size, myfile);
        if (*strsep (&buffer,"|") == "node" )
        {
            node_inf[node_count].id = strtoul(*strsep (&buffer,"|"),&trash,10);
            node_inf[node_count].name = strsep (&buffer,"|");
            for (j=0;j<7;j++)
            {
                strsep (&buffer,"|"); // Lines that we don't want
            }
            node_inf[node_count].lat = atof(*strsep (&buffer,"|"));
            node_inf[node_count].lon = atof(*strsep (&buffer,"|"));
            node_inf[node_count].successors = (unsigned long int*)malloc(2*sizeof(unsigned long int));
            // We assign a default vector size of 2 by reserving 2 times the memory needed for our data type
            if ( node_inf[node_count].successors == NULL )
            {
                printf("Memory could not be reserved. Exiting the program. \n");
                return 0;
            }
            node_count += 1;
            continue;
        }
        else if ((*strsep (&buffer,"|")) == "way" )
        {
            for (j=0;j<6;j++)
            {
                strsep (&buffer,"|"); // Lines that we don't want
            }
            if ((*strsep (&buffer,"|")) == "oneway" )
            {
                while ( (first = strsep (&buffer,"|")) != NULL )
                {
                    last = strsep (&buffer,"|");
                    put_successor ( nodes_inf, strtoul(*first,&trash,10) , strtoul(*last,&trash,10) );
                }
            }
            else
            {
                while ( (first = strsep (&buffer,"|")) != NULL )
                {
                    last = strsep (&buffer,"|");
                    put_successor ( nodes_inf, strtoul(*first,&trash,10) , strtoul(*last,&trash,10) );
                    put_successor ( nodes_inf, strtoul(*last,&trash,10) , strtoul(*first,&trash,10) );
                }
            }
        }
    }
    fclose(myfile);
    return 0;
}