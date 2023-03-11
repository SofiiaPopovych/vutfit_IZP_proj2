//Popovych Sofiia 
//xpopov13


/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <stdbool.h>
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    
    struct obj_t *obj;
    obj = malloc(sizeof(struct obj_t) * cap);
        c->capacity = cap;
        c->obj = obj;
        c->size = 0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    c = resize_cluster(c, c->capacity + CLUSTER_CHUNK); 
    c->obj[c->size] = obj; //adding an object to the first free position
    c->size++; //increase the number of the object by 1
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    int first_c1_size = c1->size;

    for(int i = 0; i < c2->size; i++) {
        append_cluster(c1,c2->obj[i]); //add all obj from c2 to c1
    }
    if(c2->size > c1->size) { //if c1 less than c2 than we increase it to the required size
        c1->size = first_c1_size + c2->size;
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    free(carr[idx].obj); // release memory for the cluster at index idx
    // shift the cluster in the array from idx index to the left
    for(int i = idx; i < narr - 1; i++) {
        carr[i] = carr[i + 1];
    }
    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float a = (o1->x - o2->x) * (o1->x - o2->x);
    float b = (o1->y - o2->y) * (o1->y - o2->y);

    return sqrtf(a + b);
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    return obj_distance(&(c1->obj[c1->size - 1]), &(c2->obj[c2->size - 1]));
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
   
    float min_dist = INT_MAX;
    float dist;

    // calculate the distance between all clusters to each other
    for(int i = 0; i < narr ; i++) {
        for(int j = 0; j < narr; j++) {
            if(i != j) {
                dist = cluster_distance(&carr[i],&carr[j]);
                // search for minimum distance
                if(dist < min_dist) {
                    min_dist = dist;
				    *c1 = i;
				    *c2 = j;
                }
            }
        }
    }
}

// looks for the most distant clusters
void find_longest_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
   
    float max_dist = -1;
    float dist;

    // calculate the distance between all clusters to each other
    for(int i = 0; i < narr ; i++) {
        for(int j = 0; j < narr; j++) {
            if(i != j) {
                dist = cluster_distance(&carr[i],&carr[j]);
                // search for maximum distance
                if(dist > max_dist) {
                    max_dist = dist;
				    *c1 = i;
				    *c2 = j;
                }
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


void clear_all_clusters(struct cluster_t *carr, int narr);

// checks the value of x and y in load_clusters
int is_valid_value(struct cluster_t *c, int x, int narr) {
    if(0 <= x && x <= 1000) {
        return 1;
    }

    clear_all_clusters(c,narr);
    return 0;
}

//checks the value of id
int is_valid_id(struct cluster_t *c, int id, int narr) {
    
    for(int i = 0; i < narr - 1; i++) {
        if(c[i].obj->id == id) {
             clear_all_clusters(c,narr);
    
            return 0;
        }
    }

    return 1;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

   //TODO
    FILE *file;
    int count;
    file = fopen(filename, "r");
    
    if(file) {

        fscanf(file, "count=%d\n", &count);
        *arr = malloc(count * sizeof(struct cluster_t)); //allocate memory for cluster array

        int id;
        float x, y;
        
        for(int i = 0; i < count; i++) {
            init_cluster(&(*arr)[i],1);
            fscanf(file, "%d %f %f", &id, &x, &y);
            
            if((is_valid_value(*arr, x, i + 1) == 0) || (is_valid_value(*arr, y , i + 1) == 0)) {
                fclose(file);
                return -2;
            }

            if (is_valid_id(*arr, id, i + 1) == 0) {
                fclose(file);
                return -3;
            }
            
            ((*arr)[i]).size = 1;
            ((*arr)[i]).obj->id = id;
            ((*arr)[i]).obj->x = x;
            ((*arr)[i]).obj->y = y;
        }
        fclose(file);
    } else {
        return -1;
    } 
    return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    } 
}


void clear_all_clusters(struct cluster_t *carr, int narr) {
    for(int i = 0; i < narr; i++) {
        free(carr[i].obj);
    }
    free(carr);
}


int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    //TODO
    int n_cluster = load_clusters(argv[1], &clusters); //load the cluster into the clusters array
    int final_cluster = n_cluster; //save the initial value of the cluster count
    bool longest_dist = false; // flag for the function find_longest_neighbours

    //cases for various possible errors
    switch(n_cluster) { 
        case -1:
            fprintf(stderr, "Chyba. Nejde nacitani souboru\n");
            return 1;
            break;
        
        case -2:
            fprintf(stderr, "Bylo zadane cislo mimo interval <0, 1000>\n");
            return 1;
            break;

        case -3:
            fprintf(stderr, "ID objektu se opakuje\n");
            return 1;
            break;

        default:
            break;
    }

    if(argc > 1 && argc < 5) { //checking the number of parametrs
        if (argc == 2) {   
            final_cluster = 1;
        }
    
        if (argc == 3) {
            final_cluster = atoi(argv[2]);
            if(final_cluster < 1 || final_cluster > n_cluster) {
                //checking the number of clusters
                fprintf(stderr,"Spatne zadan argument\n");
                return -1;
            } 
        }

        if (argc == 4) {
            final_cluster = atoi(argv[2]);
            
            if (strcmp(argv[3], "-l") == 0) {
                longest_dist = true;
            } else {
                fprintf(stderr,"Spatny argument\n");
                return -1;
            }

            if (final_cluster < 1 || final_cluster > n_cluster) {
                //checking the number of clusters
                fprintf(stderr,"Spatne zadan argument\n");
                return -1;
            }
        }

    } else {
        fprintf(stderr, "Spatne zadany parametry\n");
        return 1;
    }

    int c1, c2;
    //the course of the cluster analysis itself up to the required number of clusters
    while(n_cluster != final_cluster) {
        //distance check
        if (longest_dist) {
            find_longest_neighbours(clusters,n_cluster,&c1,&c2);
        } else {
            find_neighbours(clusters,n_cluster,&c1,&c2);
        }
        merge_clusters(&clusters[c1],&clusters[c2]);
        n_cluster = remove_cluster(clusters,n_cluster,c2);
    }

    print_clusters(clusters, final_cluster);

    clear_all_clusters(clusters,final_cluster);

    return 0;
     
}