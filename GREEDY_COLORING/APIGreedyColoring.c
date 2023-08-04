#include <stdio.h>
#include <stdlib.h>
#include "APIParte2.h"

struct ColorIndex {
    u32 value;
    u32 index;
};

struct infoColor {
    struct ColorIndex *color;
    u32 *sum_color;
};
//Usamos compare para el qsort de c 
int compare(const void* a, const void* b) {
    struct infoColor* ci1 = (struct infoColor*)a;
    struct infoColor* ci2 = (struct infoColor*)b;
    u32 *color_sum = ci2->sum_color;
    return color_sum[ci2->color->value] - color_sum[ci1->color->value];
}


void heapify2(u32 *arr1, u32 *arr2, int start, int end){
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

void heap_sort_subarray2(u32 *arr1, u32 *arr2, int start, int end){
    int n = end - start + 1;
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify2(&arr1[start], &arr2[start], i, n - 1);
    for (int i = n - 1; i >= 1; i--) {
        int tmp = arr1[start];
        arr1[start] = arr1[start + i];
        arr1[start + i] = tmp;
        tmp = arr2[start];
        arr2[start] = arr2[start + i];
        arr2[start + i] = tmp;
        heapify2(&arr1[start], &arr2[start], 0, i - 1);
    }
}

void randomize(u32 *Orden, u32 v_qty){
    u32 max = v_qty - 1;
    u32 min = 0;
    u32 key;
    u32 temp;
    for (u32 i = 0; i < v_qty; i++){
        key = (rand() % (max - min + 1)) + min;
        temp = Orden[i];
        Orden[i] = Orden[key];
        Orden[key] = temp;
    }

    
    
}
char OrdenJedi(Grafo G,u32* Orden,u32* Color){

    u32 maxColor = 0;
    u32 n = NumeroDeVertices(G);
    u32 color_index;
    u32 degree;
    for(u32 i = 0 ; i<n; i++){
        if(Color[i] >= maxColor){
            maxColor = Color[i];
        }
    }
    maxColor++;
    u32 *SumColor = calloc(maxColor,sizeof(u32));

    u32 max_F = 0;
    for (u32 i = 0; i < n; i++){ // SumColor sus indices refieren al color x 
        color_index = Color[i];
        degree = Grado(i, G);
       
        SumColor[color_index] =SumColor[color_index] + color_index*degree ; //F(x)
        if(SumColor[color_index] > SumColor[max_F]){
            max_F = color_index;
        }

    } 
    //struct infoColor i_color[n];
    struct ColorIndex *colorOrden = malloc(n*sizeof(struct ColorIndex)); // Guardamos los colores + indice 
    for(u32 i = 0 ; i<n; i++){
        colorOrden[i].value = Color[i];
        colorOrden[i].index = i;
    }
    struct infoColor *InfoColor = calloc(n,sizeof(struct infoColor));
    for (u32 i = 0; i < n; i++) { // a cada uno le damos un espacio de informacion junto con los los F(x)
        InfoColor[i].color = &colorOrden[i];
        InfoColor[i].sum_color = SumColor;
    }
    qsort(InfoColor, n, sizeof(struct infoColor), compare);
    for (u32 i = 0; i < n; i++){ // Asignamos el nuevo Orden
        Orden[i] = InfoColor[i].color->index;
    }
    
    free(colorOrden);
    free(InfoColor);
    free(SumColor);
        
    return (char) 0;

}
char OrdenImparPar(u32 n, u32 *Orden, u32 *Color){
    u32 even_qty = 0; 
    u32 odd_qty = 0;
    u32 *even = malloc(n*sizeof(u32));
    u32 *orden_even = malloc(n*sizeof(u32));

    u32 *odd = malloc(n*sizeof(u32));
    u32 *orden_odd= malloc(n*sizeof(u32));
    for(u32 i = 0 ; i<n ; i++){
        if(Color[i] % 2 == 0){
            even[even_qty] = Color[i];
            orden_even[even_qty] = i;
            even_qty++;
        }
        else{
            odd[odd_qty] = Color[i];
            orden_odd[odd_qty] = i;
            odd_qty++;
        }
    }


    if (!even || !orden_even || !odd || !orden_odd){
        return (char) 1;
    }
    

    // Ordenamos los pares e impares con sus indices
    heap_sort_subarray2(even, orden_even, 0, even_qty - 1); 
    heap_sort_subarray2(odd, orden_odd, 0, odd_qty - 1);
    u32 j = 0;

    
    for (int i = odd_qty - 1; i >= 0; i--){ // add from last to first odd elements in Orden
        Orden[j] = orden_odd[i];            
        j++;
    } 
    for (int i = even_qty - 1; i >=0; i--){ // add the rest from last to first even elements in Orden
        Orden[j] = orden_even[i];
        j++;
  
    }

    
    free(even);
    free(orden_even);
    free(odd);
    free(orden_odd);
    
    return (char) 0;   
}



u32 Greedy(Grafo G,u32* Orden,u32* Color){
    u32 color_qty = 0;
    u32 color_max = 0;
    u32 n_qty;
    u32 v_qty = NumeroDeVertices(G);
    u32 delta = Delta(G);
    u32 color_assing;
    u32 *temp_array = calloc(delta,sizeof(u32));
    //u32 *n_colors = malloc(delta*sizeof(u32));
    for(u32 i = 0; i<v_qty; i++){
        Color[i] = -1;                                   // Initialize every color in one
    }

    Color[Orden[0]] = 0; // v_qty - 1
    color_qty++;
   
    u32 *n_colors = malloc(delta*sizeof(u32));
    for(u32 i = 0; i<delta; i++){
        n_colors[i] = -1;                                   // Initialize every n_color
    }
    for (u32 i = 1; i < v_qty; i++){
        
        n_qty = Grado(Orden[i], G); 
        
        u32 u = 0;
        u32 index_temp = 0;
        for(u32 l=0; l<n_qty; l++){
            u32 neighbor_index = IndiceVecino(l, Orden[i], G);
            if(Color[neighbor_index] <= color_max && n_colors[Color[neighbor_index]] != Color[neighbor_index]){ // si no esta marcado => true
                n_colors[Color[neighbor_index]] = Color[neighbor_index]; // In our n_colors "hash table" we initialize present colors
                temp_array[index_temp] = Color[neighbor_index];         // To better performance we save the key color to clear it from the array later
                index_temp++;
                u++;
            }
            
        }
    
        color_assing = 0;

        for(u32 k = 0; k<u; k++){
            if(color_assing == n_colors[color_assing]){ // if color_assing which is a key of n_colors is initialize with that color then is used

                color_assing++;
                
            }
            else{
                break;                                  // first color we find then use it 
            }
        }
        Color[Orden[i]] = color_assing;
        
        if (color_max < color_assing){
            color_max = color_assing;
            color_qty++;
        }

        for (u32 i = 0; i < index_temp; i++){
            n_colors[temp_array[i]] = -1;    // temp_array help me to know where we initialize early to erase it, better perfomance than iterating all over n_colors
        }

       
    }
    if (color_qty > delta + 1) {
        return __UINT32_MAX__;
    }
    free(temp_array);
    free(n_colors);
    
   

    return color_qty;
}






