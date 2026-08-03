#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_1 131
#define BASE_2 137


static struct pair TOMBSTONE_OBJ;
static struct pair *const TOMBSTONE = &TOMBSTONE_OBJ;

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
        if(table->items[i] != 0 && table->items[i] != TOMBSTONE){
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

int get_hash(char* s, int hash_a, int mod, int col)
{
    int hash_b = hash_function(s, BASE_2, mod - 1);
    int i = (hash_a + col * (hash_b + 1)) % mod;
    return i;

}


char *search(struct table *t, char *key)
{
    if(!key) return NULL;
    if(!t) return NULL;

    int col = 0;
    int hash_a = hash_function(key,BASE_1,t->c);
    int hash = hash_a;
    while((t->items[hash]) != NULL && (t->items[hash] == TOMBSTONE || strcmp(key,t->items[hash]->key) != 0))
    {
        col++;
        hash = get_hash(key,hash_a,t->c,col);
    }

    if(!t->items[hash])
    {
        return NULL;
    }
    return t->items[hash]->value;
}

void delete(struct table *t, char *key)
{
    if(!key) return;
    if(!t) return;

    int col = 0;
    int hash_a = hash_function(key,BASE_1,t->c);
    int hash = hash_a;
    while((t->items[hash]) != NULL && (t->items[hash] == TOMBSTONE || strcmp(key,t->items[hash]->key) != 0))
    {
        col++;
        hash = get_hash(key,hash_a,t->c,col);
    }
    if(!t->items[hash])
    {
        return;
    }
    printf("deleting %s\n",t->items[hash]->key);
    delete_item(t->items[hash]);
    t->i--;
    t->items[hash] = TOMBSTONE;
    return;
}


void insert(struct table *t, struct pair *p)
{
    int col = 0;
    int hash_a = hash_function(p->key,BASE_1,t->c);
    int hash = hash_a;
    int flag = -1;
    int first_tombstone = -1;

    while(flag == -1)
        {
            // sigo hasta encontrar un casillero NULL o levantar la bandera

            hash = get_hash(p->key,hash_a,t->c,col);
            col++;


            if(first_tombstone == -1 && t->items[hash] == NULL)
            {
                // si es NULL y no tengo tombstone salgo y uso ese ultimo hash
                // levanto bandera
                flag = hash;
            }

            if(first_tombstone != -1 && t->items[hash] == NULL)
            {
                // despues del primer tombstone llegue a NULL,
                // levanto bandera y el loop para
                flag = first_tombstone;
            }

            if(t->items[hash] == TOMBSTONE && first_tombstone == -1)
            {
                // si es tombstone y no hay ninguno guardado
                // guardo el hash para ese tombstone
                first_tombstone = hash;
            }
            if(flag == -1 && t->items[hash] != NULL && t->items[hash] != TOMBSTONE && strcmp(p->key,t->items[hash]->key) == 0)
            {
                // si no hay bandera, no es null, no es tombstone, y las key coinciden, levanto bandera y el loop para
                // es update del ultimo hash
                flag = hash;
            }
        }

    t->i++;

    printf("flag %d\n",flag);

    if(flag != hash)
    {
        printf("inserting %s on %d\n",p->key,flag);
        // es el caso donde despues del tombstone llegue a null
        // inserto en donde estaba el tombstone
        t->items[flag] = p;
        return;
    }
    if((first_tombstone == -1) && t->items[flag] == NULL)
    {
        printf("inserting %s on %d\n",p->key,flag);
        // llegue a Null de una
        t->items[flag] = p;
        return;
    }

    // ultimo caso, llegue a la key
    printf("updating %s on %d\n",p->key,flag);
    delete(t,t->items[flag]->key);
    t->items[flag] = p;
    return;

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

    insert(t,p);
    insert(t,p2);
    insert(t,p3);

    printf("%s %s\n", t->items[21]->key, t->items[21]->value);
    printf("%s %s\n", t->items[7]->key, t->items[7]->value);
    printf("%s %s\n", t->items[27]->key, t->items[27]->value);

    printf("%d\n", t->i); // 3

    char *cat = search(t,"cat");
    char *amp = search(t,"amp");
    char *aca = search(t,"aca");
    char *dog = search(t,"dog");

    printf("%s\n", cat); // meow
    printf("%s\n", amp); // current
    printf("%s\n", aca); // alla

    if (dog) printf("%s\n", dog);
    if (!dog) printf("%p\n", dog); // (nil)

    delete(t,"amp");

    printf("%d\n", t->i); // 2

    char *amp2 = search(t,"amp");

    if (amp2) printf("%s\n", amp2);
    if (!amp2) printf("%p\n", amp2); // (nil)

    char *aca2 = search(t,"aca");

    printf("%s\n", aca2); // alla

    struct pair *p4 = initialize_item(key2,value2);

    insert(t,p4);
    printf("%s %s\n", t->items[7]->key, t->items[7]->value); // amp current
    printf("%d\n", t->i); // 3

    struct pair *p5 = initialize_item("cat", "miau");
    insert(t, p5);
    printf("%s %s\n", t->items[21]->key, t->items[21]->value); // cat miau

    printf("%d\n", t->i); // 3

    delete_table(t);


    t = NULL;
    p = NULL;
    p2 = NULL;
    p3 = NULL;

}
