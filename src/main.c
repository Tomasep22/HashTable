
// en get_hash el sondeo salta de a pasos de tamaño (hash_b + 1), en módulo mod.
// La condición para que el salto recorra los mod casilleros es que el paso y mod no compartan divisores (que sean coprimos).
/*
si mod es primo, cualquier paso entre 1 y mod-1 es coprimo con él —un primo no comparte divisores
con nadie menor que él salvo el 1—, así que el sondeo siempre barre la tabla entera.
(Por eso también, en hash_b+1, el +1 es para que el paso nunca sea 0.) Ese es el motivo que sostiene el
direccionamiento abierto: primo = cobertura total del sondeo = el insert siempre encuentra lugar si lo hay.

// un módulo primo reparte mejor los hashes cuando las claves tienen estructura o factores en común.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BASE_1 131
#define BASE_2 137

#define MIN_SHRINK_SIZE 107
#define MIN_SIZE 53



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

    // cantidad de tombtones
    int t;
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

struct table* initialize_table(unsigned size)
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
    t->t = 0;

    return t;
}


void delete_item(struct pair* item)
{
    free(item->value);
    free(item->key);
    free(item);
}

// hace deep free
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

int hash_function(char* s, int base, unsigned int mod)
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

int get_hash(char* s, int hash_a, unsigned int mod, int col)
{
    int hash_b = hash_function(s, BASE_2, mod - 1);
    int i = (hash_a + col * (hash_b + 1)) % mod;
    return i;

}

int is_prime(unsigned int n){

    if(n == 0) return 0;
    if(n == 1) return 0;

    for(unsigned int i=2;i<n;i++)
    {
        if (n!=i && n % i == 0){
            return 0;
        }
    }

    return 1;
}

unsigned int next_prime_up(unsigned int prime)
{
    prime *= 2;
    while(is_prime(prime) == 0){
        prime++;
    }

    return prime;
}

unsigned int next_prime_down(unsigned int prime)
{
    prime = (unsigned int)(round((0.5) * (double)prime));
    while(is_prime(prime) == 0){
        prime--;
    }

    return prime;
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

void resize(struct table *t, unsigned int size);

void delete_raw(struct table *t, char *key)
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
    t->t++;
    return;
}

void insert_raw(struct table *t, struct pair *p)
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
        t->t--;
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
    delete_raw(t,t->items[flag]->key);
    t->t--;
    t->items[flag] = p;
    return;
}

void delete(struct table *t, char *key)
{
    if(!key) return;
    if(!t) return;

    if (t->c >= MIN_SHRINK_SIZE && (((t->i) * 100) / t->c) < 10)
    {
        printf("delete function, shrinking table\n");
        printf("table at %d/%d (< 10%%)\n", t->i, t->c);
        resize(t,next_prime_down(t->c));
        delete_raw(t,key);
        return;
    }

    delete_raw(t,key);
    return;

}


void insert(struct table *t, struct pair *p)
{

    if ((((t->i + t->t) * 100) / t->c) > 70)
    {
        printf("insert function, growing table\n");
        printf("table at %d/%d (> 70%%)\n", t->i, t->c);
        printf("%s\n",p->key);
        resize(t,next_prime_up(t->c));
        insert_raw(t,p);
        return;
    }

    insert_raw(t,p);

}

void resize(struct table *t, unsigned int size)
{
    // resolver recursividad

    struct table *n = initialize_table(size);

    for(int i =0; i < t->c; i++)
    {
        if(t->items[i] != NULL && t->items[i] != TOMBSTONE)
        {
            insert_raw(n,t->items[i]);
        }
    }

    struct table temp = *t;
    *t = *n;
    *n = temp;

    // printf("%s\n",t->items[71]->key); // amp

    free(n->items);
    free(n);

    // printf("%s\n",t->items[71]->key); // amp

}


int main(void)
{
    struct table *t = initialize_table(MIN_SIZE);

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

    printf("tomb count %d\n",t->t); // 0
    delete(t,"amp");
    printf("tomb count %d\n",t->t); // 1

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
    printf("tomb count %d\n",t->t); // 0

    struct pair *p5 = initialize_item("cat", "miau");
    insert(t, p5);
    printf("%s %s\n", t->items[21]->key, t->items[21]->value); // cat miau

    printf("%d\n", t->i); // 3


    resize(t,MIN_SHRINK_SIZE);

    printf("main %d \n",t->c);


    printf("%d %d\n",t->c, t->i);


    printf("t %d\n",t->t);

    char *key4 = "cat4";
    char *value4 = "meow";

    char *key5 = "amp5";
    char *value5 = "current";

    char *key6 = "aca6";
    char *value6 = "alla";

    char *key7 = "cat7";
    char *value7 = "meow";

    char *key8 = "amp8";
    char *value8 = "current";

    char *key9 = "aca9";
    char *value9 = "alla";

    char *key10 = "cat10";
    char *value10 = "meow";

    char *key11 = "amp11";
    char *value11 = "current";

    char *key12 = "aca12";
    char *value12 = "alla";

    char *key13 = "cat13";
    char *value13 = "meow";

    char *key14 = "amp14";
    char *value14 = "current";

    char *key15 = "aca15";
    char *value15 = "alla";

    char *key16 = "cat16";
    char *value16 = "meow";

    char *key17 = "amp17";
    char *value17 = "current";

    char *key18 = "aca18";
    char *value18 = "alla";

    char *key19 = "cat19";
    char *value19 = "meow";

    char *key20 = "amp20";
    char *value20 = "current";

    char *key21 = "aca21";
    char *value21 = "alla";

    char *key22 = "cat22";
    char *value22 = "meow";

    char *key23 = "amp23";
    char *value23 = "current";

    char *key24 = "aca24";
    char *value24 = "alla";

    char *key25 = "cat25";
    char *value25 = "meow";

    char *key26 = "amp26";
    char *value26 = "current";

    char *key27 = "aca27";
    char *value27 = "alla";

    char *key28 = "cat28";
    char *value28 = "meow";

    char *key29 = "amp29";
    char *value29 = "current";

    char *key30 = "aca30";
    char *value30 = "alla";

    char *key31 = "cat31";
    char *value31 = "meow";

    char *key32 = "amp32";
    char *value32 = "current";

    char *key33 = "aca33";
    char *value33 = "alla";

    char *key34 = "cat34";
    char *value34 = "meow";

    char *key35 = "amp35";
    char *value35 = "current";

    char *key36 = "aca36";
    char *value36 = "alla";

    char *key37 = "cat37";
    char *value37 = "meow";

    char *key38 = "amp38";
    char *value38 = "current";

    char *key39 = "aca39";
    char *value39 = "alla";

    struct pair *p6 = initialize_item(key4,value4);

    struct pair *p7 = initialize_item(key5,value2);

    struct pair *p8 = initialize_item(key6,value3);

    struct pair *p9 = initialize_item(key7,value);

    struct pair *p10 = initialize_item(key8,value2);

    struct pair *p11 = initialize_item(key9,value3);

    struct pair *p12 = initialize_item(key10,value);

    struct pair *p13 = initialize_item(key11,value2);

    struct pair *p14 = initialize_item(key12,value3);

    struct pair *p15 = initialize_item(key13,value);

    struct pair *p16 = initialize_item(key14,value2);

    struct pair *p17 = initialize_item(key15,value3);

    struct pair *p18 = initialize_item(key16,value);

    struct pair *p19 = initialize_item(key17,value2);

    struct pair *p20 = initialize_item(key18,value3);

    struct pair *p21 = initialize_item(key19,value);

    struct pair *p22 = initialize_item(key20,value2);

    struct pair *p23 = initialize_item(key21,value3);

    struct pair *p24 = initialize_item(key22,value);

    struct pair *p25 = initialize_item(key23,value2);

    struct pair *p26 = initialize_item(key24,value3);

    struct pair *p27 = initialize_item(key25,value);

    struct pair *p28 = initialize_item(key26,value2);

    struct pair *p29 = initialize_item(key27,value3);

    struct pair *p30 = initialize_item(key28,value);

    struct pair *p31 = initialize_item(key29,value2);

    struct pair *p32 = initialize_item(key30,value3);

    struct pair *p33 = initialize_item(key31,value);

    struct pair *p34 = initialize_item(key32,value2);

    struct pair *p35 = initialize_item(key33,value3);

    struct pair *p36 = initialize_item(key34,value);

    struct pair *p37 = initialize_item(key35,value2);

    struct pair *p38 = initialize_item(key36,value3);

    struct pair *p39 = initialize_item(key37,value);

    struct pair *p40 = initialize_item(key38,value2);

    struct pair *p41 = initialize_item(key39,value3);

    insert(t,p6);
    insert(t,p7);
    insert(t,p8);
    insert(t,p9);
    insert(t,p10);
    insert(t,p11);
    insert(t,p12);
    insert(t,p13);
    insert(t,p14);
    insert(t,p15);
    insert(t,p16);
    insert(t,p17);
    insert(t,p18);
    insert(t,p19);
    insert(t,p20);
    insert(t,p21);
    insert(t,p22);
    insert(t,p23);
    insert(t,p24);
    insert(t,p25);
    insert(t,p26);
    insert(t,p27);
    insert(t,p28);
    insert(t,p29);
    insert(t,p30);
    insert(t,p31);
    insert(t,p32);
    insert(t,p33);
    insert(t,p34);
    insert(t,p35);
    insert(t,p36);
    insert(t,p37);
    insert(t,p38);
    insert(t,p39);
    insert(t,p40);
    insert(t,p41);

    delete(t,key4);
    delete(t,key5);
    delete(t,key6);
    delete(t,key7);
    delete(t,key8);
    delete(t,key9);
    delete(t,key10);
    delete(t,key11);
    delete(t,key12);
    delete(t,key13);
    delete(t,key14);
    delete(t,key15);
    delete(t,key16);
    delete(t,key17);
    delete(t,key18);
    delete(t,key19);
    delete(t,key20);
    delete(t,key21);
    delete(t,key22);
    delete(t,key23);
    delete(t,key24);
    delete(t,key25);
    delete(t,key26);
    delete(t,key27);
    delete(t,key28);
    delete(t,key29);
    delete(t,key30);
    delete(t,key31);
    delete(t,key32);
    delete(t,key33);
    delete(t,key34);
    delete(t,key35);
    delete(t,key36);
    delete(t,key37);
    delete(t,key38);
    delete(t,key39);

    delete_table(t);

    t = NULL;
    p = NULL;
    p2 = NULL;
    p3 = NULL;

}
