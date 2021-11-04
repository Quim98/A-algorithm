#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    unsigned long int id;
    char *name;
    double lat,lon;
    unsigned short int nsucc;
    unsigned long int *successors;
}node;

unsigned long int binary_search ( node nodes_inf[], unsigned long int size_nodes, unsigned long int searched_node )
{
    unsigned long int high = size_nodes-1;
    unsigned long int low = 0;
    unsigned long int middle = low + (high - low)/2;
    while ( nodes_inf[middle].id != searched_node )
    {
        if ( searched_node > nodes_inf[middle].id)
        {
            low = middle + 1;
            middle = low + (high - low)/2;
        }
        else if ( searched_node < nodes_inf[middle].id)
        {
            high = middle - 1;
            middle = low + (high - low)/2;
        }
    }
    return middle;
}

void put_successor ( node nodes_inf[], unsigned long int source, unsigned long int destination )
{
    unsigned long int source_element, vector;
    unsigned int k=0;
    unsigned long int size_nodes = 23895681;
    // Search the element of the source node
    source_element = binary_search ( nodes_inf, size_nodes, source );

    // Search if the destination node is already in the vector
    for (k=0;k<nodes_inf[source_element].nsucc;k++)
    {
        vector = nodes_inf[source_element].successors[k];
        if (vector==destination)
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
    int isnode, isway, isrelation, isoneway, contador, salir;
    char *trash;
    char *first;
    unsigned long int node_count = 0;
    unsigned long int edges[1000], no_existe[1000];
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
    for (i=3;i<25353790;i++)
    {
        getline (&buffer, &size, myfile);
        first = strsep (&buffer,"|");
        isnode = strcmp(first,"node");
        isway = strcmp(first,"way");
        isrelation = strcmp(first,"relation");
        if (isnode==0)
        {
            printf("Hello\n");
            node_inf[node_count].id = strtoul(strsep (&buffer,"|"),&trash,10);
            node_inf[node_count].name = strsep(&buffer,"|");
            j=0;
            for (j=0;j<6;j++)
            {
                trash = strsep(&buffer,"|"); // Lines that we don't want
            }
            node_inf[node_count].lat = atof(strsep (&buffer,"|"));
            node_inf[node_count].lon = atof(strsep (&buffer," "));
            node_inf[node_count].nsucc = 0;
            // We assign a default vector size of 2 by reserving 2 times the memory needed for our data type
            node_inf[node_count].successors = (unsigned long int*)malloc(2*sizeof(unsigned long int));
            if ( node_inf[node_count].successors == NULL )
            {
                printf("Memory of successors could not be reserved. Exiting the program. \n");
                return 1;
            }
            node_count += 1;
        }
        else if (isway==0)
        {
            printf("Hello2\n");
            j=0;
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
                salir = 0;
                while (first != NULL)
                {
                    contador = contador + 1;                
                    edges[contador-1]=strtoul(first,&trash,10);
                    h=0;
                    for (h=0;h<23895681;h++)
                    {
                        if (node_inf[h].id == edges[contador-1])
                        {
                            no_existe[contador-1] = 0;
                            break;
                        }
                        else
                        {
                            no_existe[contador-1] = 1;
                        }
                    }
                    first = strsep (&buffer,"|");
                }
                m=0;
                for (m=0;m<contador;m++)
                {
                    if (no_existe[m]==1)
                    {
                        salir = salir+1;
                    }
                }
                if (salir==0)
                {
                    if (contador > 2)
                    {
                        if ((contador%2)==0)
                        {
                            n=0;
                            for (n=0;n<contador;n=n+2)
                            {
                                printf("pasa");
                                put_successor ( node_inf, edges[n] , edges[n+1] );
                            }
                        }
                    }
                } 
            }
            else
            {
                trash = strsep (&buffer,"|");
                first = strsep (&buffer,"|");
                contador = 0;
                salir = 0;
                while (first != NULL)
                {
                    contador = contador + 1;                
                    edges[contador-1]=strtoul(first,&trash,10);
                    h=0;
                    for (h=0;h<23895681;h++)
                    {
                        if (node_inf[h].id == edges[contador-1])
                        {
                            no_existe[contador-1] = 0;
                            break;
                        }
                        else
                        {
                            no_existe[contador-1] = 1;
                        }
                    }
                    first = strsep (&buffer,"|");
                }
                m=0;
                for (m=0;m<contador;m++)
                {
                    if (no_existe[m]==1)
                    {
                        salir = salir+1;
                    }
                }
                if (salir==0)
                {
                    if (contador > 2)
                    {
                        if ((contador%2)==0)
                        {
                            n=0;
                            for (n=0;n<contador;n=n+2)
                            {
                                printf("pasa");
                                put_successor ( node_inf, edges[n] , edges[n+1] );
                                put_successor ( node_inf, edges[n+1] , edges[n] );
                            }
                        }
                    }
                } 
            }
        }
        else 
        {
            printf("relaaaation");
            break;
        }
    }
    printf("se acabo el file");
    p=0;
    for (p=0;p<23895681;p=p+10000) // See if we have saved the nodes
    {
        printf("The id of the node is: %lu \n",node_inf[p].id);
        printf("The latitude of the node is: %f \n",node_inf[p].lat);
        printf("The longitude of the node is: %f \n",node_inf[p].lon);
        printf("The number of successors of the node is: %i \n",node_inf[p].nsucc);
        //printf("The successors numer 1 of the node is: %lu \n",node_inf[i].successors[0]);
    }
    fclose(myfile);
    return 0;
}