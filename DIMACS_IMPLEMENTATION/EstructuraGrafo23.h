
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int u32;

typedef struct Vertice vertice; // vertice puntero a la estructura VErtice




// Define the hash set structure
typedef struct HashNode {
    unsigned int key;
    unsigned int position;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode** table;
    int size;
} HashTable;

struct Vertice{
  u32 V_name;
  u32 V_degree;
  u32 order_in_natural_pos;
  vertice **vecinos;
};
typedef struct GrafoSt{
  u32 V_num;
  u32 E_num;
  u32 max_degree;
  vertice **vertices;
}GrafoSt;