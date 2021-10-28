#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node{
    unsigned long int id;
    char *name;
    double lat,lon;
    unsigned short int nsucc;
    unsigned long int *successors;
}node;


unsigned long int binary_search ( node nodes_inf[], int size_nodes, int searched_node )
{
    int middle = (size_nodes-1)/2;
    int search_size = size_nodes-1;
    while ( nodes_inf[middle].id != searched_node )
    {
        if ( searched_node > nodes_inf[middle].id)
        {
            search_size=search_size/2;
            middle = middle + search_size/2;
        }
        else if ( searched_node < nodes_inf[middle].id)
        {
            search_size=search_size/2;
            middle = middle - search_size/2;
        }
    }
    return middle;
}

void put_successor ( node nodes_inf[], unsigned long int source, unsigned long int destination )
{
    unsigned long int source_element;
    unsigned short int k;

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
        nodes_inf[source_element].successors = (unsigned long int*)realloc(nodes_inf[source_element].successors,nodes_inf[source_element].nsucc*sizeof(unsigned long int));
    }

    // Add the successor
    nodes_inf[source_element].successors[nodes_inf[source_element].nsucc - 1] = destination;
    return;
}

int main(int argc, char** argv)
{
    FILE *myfile;
    size_t size = 79857;
    char *buffer;
    char *tmp;
    char **sp;
    unsigned long int node_count = 0;
    unsigned long int i;
    unsigned int j;
    node *node_inf;
    if((node_inf = (node*)malloc(23895681*sizeof(node))) == NULL)
    {
        printf("Memory of nodes could not be reserved. Exiting the program. \n");
        return 1;
    }
    char *trash;
    char *first;
    char *last;
    
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

    if((buffer = (char*) malloc (size*sizeof(char))) == NULL)
    {
        printf("Memory of buffer could not be reserved. Exiting the program. \n");
        return 1;
    }

    getline (&buffer, &size, myfile);
    getline (&buffer, &size, myfile); // Discard first 3 rows
    getline (&buffer, &size, myfile);

    printf("Reading line: '%s'\n",buffer); // Borrar!!!!!!!!!!!!!

    printf("Prova 1 \n"); // Borrar!!!!!!!!!!!!!
    
    for (i=3;i<25353791;i++)
    {
        getline (&buffer, &size, myfile);
        tmp = strdup(buffer);
        sp = &tmp;
        
        first = strsep (&buffer,"|");
        
        if ( first == "node" )
        {
            node_inf[node_count].id = strtoul(strsep (sp,"|"),&trash,10);
            node_inf[node_count].name = strsep (sp,"|");
            for (j=0;j<7;j++)
            {
                trash = strsep(&buffer,"|"); // Lines that we don't want
            }
            node_inf[node_count].lat = atof(strsep (sp,"|"));
            node_inf[node_count].lon = atof(strsep (sp,"|"));
            node_inf[node_count].nsucc = 0;
            ;
            // We assign a default vector size of 2 by reserving 2 times the memory needed for our data type
            if ( (node_inf[node_count].successors = (unsigned long int*)malloc(2*sizeof(unsigned long int))) == NULL )
            {
                printf("Memory of successors could not be reserved. Exiting the program. \n");
                return 1;
            }
            node_count += 1;
        }
        else if ( first == "way" )
        {
            for (j=0;j<6;j++)
            {
                trash = strsep (sp,"|"); // Lines that we don't want
            }
            if ( (first = strsep (sp,"|")) == "oneway" )
            {
                while ( (first = strsep (sp,"|")) != NULL )
                {
                    last = strsep (sp,"|");
                    put_successor ( node_inf, strtoul(first,&trash,10) , strtoul(last,&trash,10) );
                }
            }
            else
            {
                while ( (first = strsep (sp,"|")) != NULL )
                {
                    last = strsep (sp,"|");
                    put_successor ( node_inf, strtoul(first,&trash,10) , strtoul(last,&trash,10) );
                    put_successor ( node_inf, strtoul(last,&trash,10) , strtoul(first,&trash,10) );
                }
            }
        }
    }
    fclose(myfile);

    printf("Prova 2 \n"); // Borrar!!!!!!!!!!!!!

    /*
    for (i=0;i<23895681;i=i+1000000) // See if we have saved the nodes
    {
        printf("The id of the node is: %d \n",node_inf[i].id);
        printf("The latitude of the node is: %f \n",node_inf[i].lat);
        printf("The longitude of the node is: %f \n",node_inf[i].lon);
        printf("The number of successors of the node is: %d \n",node_inf[i].nsucc);
    }
    */


    return 0;
}