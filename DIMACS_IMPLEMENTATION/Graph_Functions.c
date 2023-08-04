#include "APIG23.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//-------------------------FUNCIONES PARA LA HASH TABLE-------------------------
unsigned int hash(unsigned int key, u32 v_qty) {
  
    return key % v_qty;
}

HashNode* create_hash_node(unsigned int key, unsigned int value) {
    HashNode* node = (HashNode*) malloc(sizeof(HashNode));
    if (node == NULL) {
        printf("Error: Failed to allocate memory for the node.\n");
        exit(1);
    }
    node->key = key;
    node->position = value;
    node->next = NULL;
    return node;
}

HashTable* create_hash_table(int size) {
    HashTable* table = (HashTable*) malloc(sizeof(HashTable));
    if (table == NULL) {
        printf("Error: Failed to allocate memory for the table.\n");
        exit(1);
    }
    table->table = (HashNode**) calloc(size, sizeof(HashNode*));
    if (table->table == NULL) {
        printf("Error: Failed to allocate memory for the table.\n");
        exit(1);
    }
    table->size = size;
    return table;
}

void hash_table_put(HashTable* table, unsigned int key, unsigned int value, u32 v_qty) {
    unsigned int index = hash(key, v_qty);
    HashNode* current = table->table[index];
    while (current != NULL) {
        if (current->key == key) {
            current->position = value;
            return;
        }
        current = current->next;
    }
    HashNode* node = create_hash_node(key, value);
    node->next = table->table[index];
    table->table[index] = node;
}

unsigned int hash_table_get(HashTable* table, unsigned int key, u32 v_qty) {
    unsigned int index = hash(key, v_qty);
    HashNode* current = table->table[index];
    while (current != NULL) {
        if (current->key == key) {
            return current->position;
        }
        current = current->next;
    }
    return 4294967295;
}

void hash_table_remove(HashTable* table, unsigned int key, u32 v_qty) {
    unsigned int index = hash(key, v_qty);
    HashNode* current = table->table[index];
    HashNode* previous = NULL;
    while (current != NULL) {
        if (current->key == key) {
            if (previous == NULL) {
                table->table[index] = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}
void free_hash_table(HashTable* table) {
 
    for (int i = 0; i < table->size; i++) {
        HashNode* current = table->table[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current);
            current = next;
        }
    }
    // Free the hash table itself
    free(table->table);
    free(table);
}
//-------------------------FUNCIONES PARA LA HASH TABLE-------------------------

//------------------------------FUNCIONES ORDENAR-------------------------------
void heapify(u32 *arr1, u32 *arr2, int start, int end){
    int root = start;
    while (root * 2 + 1 <= end){
        int child = root * 2 + 1;
        if (child + 1 <= end && arr1[child] < arr1[child + 1])
            child += 1;
        if (arr1[root] < arr1[child]) {
            int tmp = arr1[root];
            arr1[root] = arr1[child];
            arr1[child] = tmp;
            tmp = arr2[root];
            arr2[root] = arr2[child];
            arr2[child] = tmp;
            root = child;
        }
        else
            return;
    }
}

void heap_sort_subarray(u32 *arr1, u32 *arr2, int start, int end){
    int n = end - start + 1;
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(&arr1[start], &arr2[start], i, n - 1);
    for (int i = n - 1; i >= 1; i--) {
        int tmp = arr1[start];
        arr1[start] = arr1[start + i];
        arr1[start + i] = tmp;
        tmp = arr2[start];
        arr2[start] = arr2[start + i];
        arr2[start + i] = tmp;
        heapify(&arr1[start], &arr2[start], 0, i - 1);
    }
}
//------------------------------FUNCIONES ORDENAR-------------------------------

//----------------------------FUNCIONES CREAR GRAFO-----------------------------
vertice *crearVertice(u32 name, u32 degree, u32 index){
    //vertice *v = malloc(sizeof(struct Vertice));
    vertice *v = calloc(1,sizeof(vertice));
    v->V_name = name;
    v->V_degree = degree;
    v->order_in_natural_pos = index;
     v->vecinos = (vertice**) calloc(degree, sizeof(vertice*)); 
    return v;
}
void cargarDatos(Grafo G, u32 *l_arr, u32* r_arr, HashTable* table, u32 v_qty, u32 e_qty){
    u32 actual_vertex = l_arr[0];
    u32 vertex_index = 0;
    //u32 neighbor_qty;
    u32 position_found;
    u32 delta;
    u32 degree = 0;
    //Upload all vertices clean with their degree
    for (u32 i = 0; i < e_qty*2+1; i++){
        if(l_arr[i]!=actual_vertex){
            vertice *v = crearVertice(actual_vertex,degree,vertex_index);
            G->vertices[vertex_index] = v;
            hash_table_put(table, actual_vertex,vertex_index,v_qty);
            actual_vertex = l_arr[i];
            vertex_index++;
            degree = 0;
        }
        degree++;

    }

    vertex_index = 0;
    actual_vertex = l_arr[0];
    delta = 0;
    u32 neighbor_index = 0;
    for (u32 i = 0; i < e_qty*2; i++){ // iterate all edges and add each neigbor for all vertexs

        if(l_arr[i]!=actual_vertex){ 
            actual_vertex = l_arr[i];
            vertex_index++;
            neighbor_index = 0;
        }

        position_found = hash_table_get(table, r_arr[i], v_qty); // retrieve position to catch the pointer in our **vertices
        G->vertices[vertex_index]->vecinos[neighbor_index] = G->vertices[position_found]; // we give the pointer of the right vertex 
   
        if(delta < G->vertices[vertex_index]->V_degree){
            delta = G->vertices[vertex_index]->V_degree;
        }
        neighbor_index++;
    }
    G->max_degree = delta; // assign delta to graph
    
}
u32 leer_lados(u32 *l_arr, u32 *r_arr, u32 e_qty){
    u32 scan_res = 2;
    u32 left;    
    u32 right;
    u32 flag = 0;
    u32 edges_count = 0;
    for(u32 i = 0; i<e_qty ; i++){
        
        scan_res = fscanf(stdin, "\ne %u %u", &left, &right);
        if (scan_res != 2){
            flag = 1;
            break;
        }

        l_arr[i] = left;
        l_arr[i+e_qty] = right;

        r_arr[i] = right;
        r_arr[i+e_qty] = left; 

        edges_count++;
    }
    if (edges_count != e_qty){
        flag = 1;
    }
    

    return flag;
}
void ordenar_lados(u32 *l_arr, u32 *r_arr, u32 e_qty){
    u32 actual_vertex = l_arr[0];
    u32 initial_index = 0;
    heap_sort_subarray(l_arr,r_arr,0,e_qty*2-1); // sort all vertices

    for(u32 i = 0; i<e_qty*2+1; i++){ // quick sort the right for each distinct vertex / +1 cause we need the last to be compared

        if(l_arr[i] != actual_vertex){
            heap_sort_subarray(r_arr,l_arr,initial_index,i-1);
            actual_vertex = l_arr[i];
            initial_index = i;

        }

    }
}

void chequear_comentarios() {
    char ch;
    while (fscanf(stdin, "%c", &ch) != EOF && ch != 'p') {
        while (fscanf(stdin, "%c", &ch) != EOF && ch != '\n');
    }
}

Grafo InitEstructura(Grafo G){
    u32 scan_res;
    u32 v_qty;
    u32 e_qty;
    char *edge = calloc(10,sizeof(char));

    //check comments
    chequear_comentarios();


    //read first two values
    scan_res = fscanf(stdin, "%s %u %u",edge,&v_qty, &e_qty);
    
    if(scan_res!=3 || strncmp(edge,"edge",10)!=0){
        free(G);
        G = NULL;
        free(edge);
        return G;
    }
    free(edge);
    if (v_qty == 0 || e_qty == 0){
        free(G);
        G = NULL;
        return G;
    }

    G->V_num = v_qty;
    G->E_num = e_qty;
    G->max_degree = 0;
    G->vertices = calloc(v_qty, sizeof(vertice *)); //alloc to array of pointers for cant of vertex (v_qty pointers)

    u32 *l_arr = calloc(e_qty*2+1,sizeof(u32)); // column izq + column der
    u32 *r_arr = calloc(e_qty*2+1,sizeof(u32)); // column der + column izq
    HashTable* table = create_hash_table(v_qty*2);

    leer_lados(l_arr, r_arr, e_qty);
    ordenar_lados(l_arr, r_arr, e_qty);
    cargarDatos(G, l_arr, r_arr, table, v_qty, e_qty);

    free(l_arr);
    free(r_arr);
    free_hash_table(table);

    return G;
}
//----------------------------FUNCIONES CREAR GRAFO-----------------------------

Grafo ConstruirGrafo(){
    Grafo G = calloc(1,sizeof(struct GrafoSt));
    G->V_num = 0;
    G->E_num = 0;
    G->max_degree = 0;
    G->vertices = NULL;
    G = InitEstructura(G);
    return G;
}

void DestruirGrafo(Grafo G){
    if (G!=NULL){
        for (u32 i = 0; i < G->V_num; i++) {
            free(G->vertices[i]->vecinos);
            free(G->vertices[i]);
        }
        free(G->vertices);
        free(G);
    }
    

}
u32 NumeroDeVertices(Grafo G){
    if (G!=NULL){
        return G->V_num;
    }
    return 4294967295;
}
u32 NumeroDeLados(Grafo G){
    if (G!=NULL){
        return G->E_num;
    }
    return 4294967295;
}
u32 Delta(Grafo G){
    if (G!=NULL){
        return G->max_degree;
    }
    return 4294967295;
}
u32 Nombre(u32 i, Grafo G){
    if (G!=NULL){
        return G->vertices[i]->V_name;
    }
    return 4294967295;
}
u32 Grado(u32 i, Grafo G){
    if (G!=NULL){
        return G->vertices[i]->V_degree;
    }
    return 4294967295;
}
u32 IndiceVecino(u32 j,u32 i,Grafo G){
    if (G!=NULL){
        u32 n_qty = G->vertices[i]->V_degree;
        u32 v_qty = G->V_num;
        if(i<v_qty && j<n_qty){
            return G->vertices[i]->vecinos[j]->order_in_natural_pos;
        }
        else{
            return 4294967295;
        }
    }
    return 4294967295;
}
 
//Testeo de crear y destruir grafos:
//   FILE                     vertices,lados,delta                           Tiempo
// R22_93_15                    (22V 93E, 21D)                       OK aprox 0.000387segs
// q13                          (169V 3328E, 48D)                    OK aprox 0.002086segs
// bxb15_22_10                  (750V 2670E, 72D)                    OK aprox 0.002205segs
// school1                      (385V, 19095E, 282D)                 OK aprox 0.032978segs
// Q53                          (2809V, 242528E, 208D)               OK aprox 0.211982segs
// R10                                    OK 
// KD                           (500V,124750E, 499D)                 OK aprox 0.118785segs
// KM                           (1000V, 499500E, 999D)               OK aprox 0.440687segs
// C290000K4                    (290004V, 290005E, 3D)               OK aprox 0.482742segs   
// CMKv2HKD                     (200500V, 324749E, 499D)             OK aprox 0.439442segs
// GGNM1200                     (1440002V, 2877601E, 4D)             OK aprox 2.678254segs
// Octo                         (3080V, 4303060E, 2977D)             OK aprox 2.678254segs
// largecomplex4                (1604120, 6415706, 3550D)            OK aprox 9.015465segs
// Harkon                       (6160V, 18092520, 6057D)             OK aprox 21.648429segs
// Gf12345_12111222_457_15      (221651V, 12528006E, 12340D)         OK aprox 15.658401segs
// BQSstar1_MM                  (1001541V, 1967270E, 1001540D)       OK aprox 2.568323segs
// BxB1100_999_54_2017          (2210104V,8807410E,4196D)            OK aprox 12.544118segs
// R1999999_10123123_1          (1999999V, 10123123E, 1999998D)      OK aprox 10.116076segs






