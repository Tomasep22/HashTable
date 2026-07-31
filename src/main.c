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

int hash_function(char* s, int base, int mod)
{
    int len = strlen(s);

    if (!len){
        return 0;
    }

    unsigned long long r = s[0];

    for(int i = 1; i<len; i++){
        r *= base;
        r += s[i];
    }

    return (int)(r%mod);
}

int get_hash(char* s, int base, int base2, int mod, int col)
{
    int hash_a = hash_function(s,base,mod);
    int hash_b = hash_function(s,base2,mod);
    int i = (hash_a + col * (hash_b + 1)) % mod;
    return i;

}


int main(void)
{
    struct table *t = initialize_table(53);

    char *key = "cat";
    char *value = "meow";

    char *key2 = "amp";
    char *value2 = "current";

    char *key3 = "aca";
    char *value3 = "alla";


    struct pair *p = initialize_item(key,value);

    struct pair *p2 = initialize_item(key2,value2);

    struct pair *p3 = initialize_item(key3,value3);


    printf("%s\n%s\n",p->key,p->value);
    printf("%s\n%s\n",p2->key,p2->value);
    printf("%s\n%s\n",p3->key,p3->value);


    int hash = hash_function(key,131,53);
    int hash2 = hash_function(key2,131,53);
    int hash3 = hash_function(key3,131,53);

    printf("%d\n",hash);
    printf("%d\n",hash2);
    printf("%d\n",hash3);

    int c = 0;
    int c2 = 0;
    int c3 = 0;

    if(t->items[hash] == NULL){
        t->items[hash] = p;
    }
    else {
            while(t->items[hash] != NULL)
                {
                    c++;
                    hash = get_hash(key,131,137,53,c);
                }
            t->items[hash] = p;
    }

    if(t->items[hash2] == NULL){
        t->items[hash2] = p2;
    }
    else {
        while(t->items[hash2] != NULL)
            {
                c2++;
                hash2 = get_hash(key2,131,137,53,c2);
            }
        t->items[hash2] = p2;
    }

    if(t->items[hash3] == NULL){
        t->items[hash3] = p3;
    }
    else {
        while(t->items[hash3] != NULL)
            {
                c3++;
                hash3 = get_hash(key3,131,137,53,c3);
            }
        t->items[hash3] = p3;
    }

    printf("%s %s\n", t->items[hash]->key, t->items[hash]->value);
    printf("%s %s\n", t->items[hash2]->key, t->items[hash2]->value);
    printf("%s %s\n", t->items[hash3]->key, t->items[hash3]->value);

    printf("%d\n", hash);  // 21
    printf("%d\n", hash2); // 5
    printf("%d\n", hash3); // 51

    delete_table(t);

    t = NULL;
    p = NULL;
    p2 = NULL;
    p3 = NULL;


}
