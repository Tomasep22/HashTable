#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// este es el struct para cada clave-valor

struct pair
{
    char *key;   // clave
    char *value; // valor
};

// este es el struct donde guardo la tabla

struct table
{
    // puntero a puntero struct pair
    struct pair **items;

    // cantidad de items
    int i;

    // capacidad maxima de items
    int c;
};

struct pair* initialize_item(char *key, char *value)
{

    // si algo vino mal salgo
    if((key) == NULL || (value) == NULL)
    {
        printf("error en los datos");
        return NULL;
    }

    // pido memoria para guardar el key
    char *k = malloc(strlen(key)+1); // +1 porque strlen no cuenta el nul terminator

    // si fallo  retorno NULL
    if(!k)
    {
        printf("fallo malloc");
        return NULL;
    }

    // pido memoria para guardar el value
    char *v = malloc(strlen(value)+1); // +1 porque strlen no cuenta el nul terminator


    // si fallo  retorno NULL y libero k
    if(!v)
    {
        printf("fallo malloc");
        free(k);
        return NULL;
    }

    strcpy(k,key);
    strcpy(v,value);

    struct pair *p = malloc(sizeof(struct pair));

       // si fallo  retorno NULL y libero k y v
    if(!p)
    {
        printf("fallo malloc");
        free(k);
        free(v);
        return NULL;
    }

    p->key = k;
    p->value = v;

    printf("%s\n%s\n", k,v);

    printf("%s\n%s\n", p->key,p->value);

    return p;
}

struct table* initialize_table(int size)
{

    struct table *t = malloc(sizeof(struct table));

    if(!t)
    {
        return NULL;
    }

    t->items = calloc(size,sizeof(struct pair*));

    if(!(t->items))
    {
        free(t);
        return NULL;
    }

    t->c = size;
    t->i = 0;

    return t;
}

void delete_item(struct pair* item)
{
    free(item->value);
    free(item->key);
    free(item);
}

void delete_table(struct table* table)
{
    int i = 0;
    while((table->c) > i){
        if(table->items[i] != 0){
            delete_item(table->items[i]);
        }
        i++;
    }
    free(table->items);
    free(table);
}



int main(void)
{
    struct table *t = initialize_table(53);

    char *key = "cat";
    char *value = "meow";

    struct pair *p = initialize_item(key,value);

    printf("%s\n%s\n",p->key,p->value);

    int i = ((131*131*99) + (131*97) + 116) % 53;
    printf("%d\n",i);

    (t->items)[i] = p;

    printf("%s\n",(t->items)[i]->key);

    delete_table(t);

    t = NULL;
    p = NULL;
}
