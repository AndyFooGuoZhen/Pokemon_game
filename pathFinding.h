#include <limits.h>
#include <cstdlib>

class Node{
    public:
    int xCoor;
    int yCoor;
    int pred[2];
    int cost;
    int distance;
    char charType;
    int index; //index of element in npc array
};

/*-----------------------------Binary Min Heap implementation-------------------------------------*/
//"heap implementation"
//https://www.digitalocean.com/community/tutorials/min-heap-binary-tree

typedef struct MinHeap MinHeap;

void initializeNodesDistAndCoor(struct Node nodes[21][80], int playerXCoor, int playerYCoor);
void initializeNodesCost(char **mapGrid, struct Node nodes[21][80], char hikerOrRival);
int costOfTerrain(char symbol, char hikerORRival);
void getNeighborNodes(struct Node currNode,struct Node nodes[21][80], struct Node *neigborNodes[3][3]);
void updateNodeDetails(struct Node nodes[21][80], int xCoor, int yCoor, int distance, int predXCoor, int predYCoor);
void generateShortestPaths(MinHeap *minheap, struct Node nodes[21][80]);
void printDjiktraCostMap(struct Node nodes[21][80]);
void djiktraCostFix(struct Node nodes[21][80]);
void buildDjiktraCostMap(MinHeap *heap,char **mapGrid, struct Node nodes[21][80], int playerXCoor, int playerYCoor, char hikerOrRival);

struct MinHeap {
    Node *node;

    // Current Size of the Heap
    int size;
    // Maximum capacity of the heap
    int capacity;
};

int parent(int i) {
    // Get the index of the parent
    return (i - 1) / 2;
}

int left_child(int i) {
    return (2*i + 1);
}

int right_child(int i) {
    return (2*i + 2);
}

/*
 Return the root node element, since that's the minimum
*/
Node get_min(MinHeap* heap) {
    return heap->node[0];
}

/*
Initializes binary min heap
*/
MinHeap* init_minheap(int capacity) {
    MinHeap* minheap = (MinHeap*) calloc (1, sizeof(MinHeap));
    minheap->node = (Node*) calloc (capacity, sizeof(Node));
    minheap->capacity = capacity;
    minheap->size = 0;
    return minheap;
}

/*
Inserts a node into min heap, based on its distance
*/
MinHeap* insert_minheap(MinHeap* heap, Node node) {

    if (heap->size == heap->capacity) {
        // fprintf(stderr, "Cannot insert. Heap is already full!\n");
        return heap;
    }
    // We can add it. Increase the size and add it to the end
    heap->size++;
    heap->node[heap->size - 1] = node;

    // Keep swapping until we reach the root
    int curr = heap->size - 1;
    // As long as you aren't in the root node, and while the 
    // parent of the last element is greater than it
    while (curr > 0 && heap->node[parent(curr)].distance > heap->node[curr].distance) {
        // Swap
        Node temp = heap->node[parent(curr)];
        heap->node[parent(curr)] = heap->node[curr];
        heap->node[curr] = temp;
        // Update the current index of element
        curr = parent(curr);
    }

    return heap; 
}


MinHeap* heapify(MinHeap* heap, int index) {
    // Rearranges the heap as to maintain
    // the min-heap property
    if (heap->size <= 1)
        return heap;
    
    int left = left_child(index); 
    int right = right_child(index); 

    // Variable to get the smallest element of the subtree
    // of an element an index
    int smallest = index; 
    
    // If the left child is smaller than this element, it is
    // the smallest
    if (left < heap->size && heap->node[left].distance < heap->node[index].distance) 
        smallest = left; 
    
    // Similarly for the right, but we are updating the smallest element
    // so that it will definitely give the least element of the subtree
    if (right < heap->size && heap->node[right].distance < heap->node[smallest].distance) 
        smallest = right; 

    if (smallest != index) 
    { 
        Node temp = heap->node[index];
        heap->node[index] = heap->node[smallest];
        heap->node[smallest] = temp;
        heap = heapify(heap, smallest); 
    }

    return heap;
}

/*
Decreases a node's distance to the given value 
*/
void decreaseKey(MinHeap* heap, int xCoor, int yCoor, int value){
    int index = 0;

    for(int i=0; i<heap->size; i++){
        if(heap->node[i].xCoor == xCoor && heap->node[i].yCoor == yCoor){
            index = i;
        }
    }
    heap->node[index].distance = value;
    Node temp;

    while(index != 0 && heap->node[parent(index)].distance > heap->node[index].distance){
        temp = heap->node[index];
        heap->node[index] = heap->node[parent(index)];
        heap->node[parent(index)] = temp;
        index = parent(index);
    }
}

/*
Deletes node with min distance
*/
MinHeap* delete_minimum(MinHeap* heap) {
    // Deletes the minimum element, at the root
    if (!heap || heap->size == 0)
        return heap;

    int size = heap->size;
    Node last_element = heap->node[size-1];
    
    // Update root value with the last element
    heap->node[0] = last_element;

    // Now remove the last element, by decreasing the size
    heap->size--;
    size--;

    // We need to call heapify(), to maintain the min-heap
    // property
    heap = heapify(heap, 0);
    return heap;
}

void free_minheap(MinHeap* heap) {
    if (!heap)
        return;
    free(heap->node);
    free(heap);
}

/*----------------------------Node implementation-------------------------------------*/

//Treats every element in the map as a node, used for generating shortest paths with djikstras


// void initializeNodesDistAndCoor(struct Node nodes[21][80], int playerXCoor, int playerYCoor);
// void initializeNodesCost(char **mapGrid, struct Node nodes[21][80], char hikerOrRival);
// int costOfTerrain(char symbol, char hikerORRival);
// void getNeighborNodes(struct Node currNode,struct Node nodes[21][80], struct Node *neigborNodes[3][3]);
// void updateNodeDetails(struct Node nodes[21][80], int xCoor, int yCoor, int distance, int predXCoor, int predYCoor);
// void generateShortestPaths(MinHeap *minheap, struct Node nodes[21][80]);
// void printDjiktraCostMap(struct Node nodes[21][80]);
// void djiktraCostFix(struct Node nodes[21][80]);
// void buildDjiktraCostMap(MinHeap *heap,char **mapGrid, struct Node nodes[21][80], int playerXCoor, int playerYCoor, char hikerOrRival);

/*
Initializes nodes before djiktra,
-provide nodes with relative x and y coordinates
-sets all distances of nodes to infinity
-sets distance and cost of pc node to 0

@Param x and y coordinate of PC node
*/
//Need to be tweaked to scan map for different terrain elements
void initializeNodesDistAndCoor(struct Node nodes[21][80], int playerXCoor, int playerYCoor){

    for(int i=0; i<21; i++){
        for(int j =0; j<80; j++){
            nodes[i][j].xCoor = i;
            nodes[i][j].yCoor = j;
            nodes[i][j].distance = INT_MAX;
        }
    }

    nodes[playerXCoor][playerYCoor].distance = 0;
    nodes[playerXCoor][playerYCoor].cost = 0;
}

/*
Sets Node's cost based on their relative symbols, also gives nodes their terrainSymbol
@Param : map , nodes array
        hikerOrRival :  H -> Hiker (cost of grass is 15, mountain is 15, forest is 15)I've talked to my supervisorIII
                        R -> Rival (cost of grass is 20, mountain is INF, forest is INF)
*/
void initializeNodesCost(char **mapGrid, struct Node nodes[21][80], char hikerOrRival){

    for(int i=0; i<21; i++){
        for(int j=0; j<80; j++){

            if(i == 0 || i == 20 || j==0 || j == 79){
                nodes[i][j].cost = INT_MAX;
            }else{
                nodes[i][j].cost = costOfTerrain(mapGrid[i][j], hikerOrRival);
                nodes[i][j].charType = mapGrid[i][j];
            }
        }
    }
}

/*
Helper method for providing cost of terrain/symbol
depending on hiker or rival
 @Param hikerOrRival :  H -> Hiker (cost of grass is 15, mountain is 15, forest is 15)
                        R -> Rival (cost of grass is 20, mountain is INF, forest is INF)
*/
int costOfTerrain(char symbol, char hikerORRival){
    int cost = 0;

    switch(symbol){

        case '%':
        case '^':
            cost = INT_MAX;
            break;
        
        case '#':
        case '.':
            cost = 10;
            break;
        
        case 'M':
        case 'C':
            cost = 50;
            break;
        
       case ':':
            cost = 20;
            break;
        
        case 'S':
            cost = 30;
            break;

        default:
            break;
    }

    if(hikerORRival == 'H'){

        if(symbol == ':' || symbol == '%' || symbol == '^' ){
            cost = 15;
        }
    }

    return cost;
}

/*
Method to get neighboring nodes, helper method for djiktra
*/
void getNeighborNodes(struct Node currNode,struct Node nodes[21][80], struct Node *neigborNodes[3][3]){

    int currX = currNode.xCoor;
    int currY = currNode.yCoor;
    int tempX;
    int tempY;

    //Initially, all neighbor nodes are null
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            neigborNodes[i][j] = NULL;
        }
    }

    for(int i=0; i<3; i++){
        tempX = currX - 1 + i;
        
        if(tempX <0 || tempX > 20){
            continue;
        }

        for(int j=0; j<3; j++){

            tempY = currY - 1 + j;

            if(tempY<0 ||tempY>79 ){
                continue;
            }

            neigborNodes[i][j] = (struct Node *)malloc(sizeof(struct Node));
            *neigborNodes[i][j] = nodes[tempX][tempY];
            // printf("(%d, %d)", neigborNodes[i][j]->xCoor, neigborNodes[i][j]->yCoor);
        }
    }

}

/*
Updates a node's details when performing djiktra's decrease key
NOTE: this node is not in the heap
*/
void updateNodeDetails(struct Node nodes[21][80], int xCoor, int yCoor, int distance, int predXCoor, int predYCoor){

    nodes[xCoor][yCoor].distance = distance;
    nodes[xCoor][yCoor].pred[0] = predXCoor;
     nodes[xCoor][yCoor].pred[1] = predYCoor;
}

/*
Uses Djiktra's algorithm to compute distances for all nodes in the map
-Inserts all nodes into binary min heap
-Perform Djiktra's algorithm to update distances of all nodes
*/
 void generateShortestPaths(MinHeap *heap, struct Node nodes[21][80]){

    //Inserts relevant nodes into min heap
    for(int i=0; i<21; i++){
        for(int j =0; j<80; j++){
            if(nodes[i][j].cost != INT_MAX){
                insert_minheap(heap, nodes[i][j]);
            }
        }
    }

    while(heap->size != 0){
        struct Node currMinNode = get_min(heap);
        struct Node *neighborNodes[3][3];
        getNeighborNodes(currMinNode, nodes, neighborNodes);

        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                if(neighborNodes[i][j] != NULL ){
                    if(neighborNodes[i][j]->cost != INT_MAX && neighborNodes[i][j]->distance > currMinNode.distance + neighborNodes[i][j]->cost){
                        decreaseKey(heap, neighborNodes[i][j]->xCoor, neighborNodes[i][j]->yCoor,currMinNode.distance + neighborNodes[i][j]->cost);
                        updateNodeDetails(nodes,neighborNodes[i][j]->xCoor, neighborNodes[i][j]->yCoor,currMinNode.distance + neighborNodes[i][j]->cost, currMinNode.xCoor, currMinNode.yCoor);
                    }
                }
            }
        }

        delete_minimum(heap);
    }

    djiktraCostFix(nodes);

 }


/*
Helper method for fixing inaccesible areas of the map
- Transforms arbitrarilly small costs to INT_MAX
*/
void djiktraCostFix(struct Node nodes[21][80]){

    for(int i=0; i<21; i++){
        for(int j=0; j<80; j++){
            if(nodes[i][j].distance < -100000){
                nodes[i][j].distance = INT_MAX;
            }
        }
    }
}

/*
Method for computing djiktra cost map for hiker or rival
*/
void buildDjiktraCostMap(MinHeap *heap,char **mapGrid, struct Node nodes[21][80], int playerXCoor, int playerYCoor, char hikerOrRival){
    initializeNodesDistAndCoor(nodes, playerXCoor, playerYCoor);
    initializeNodesCost(mapGrid, nodes, hikerOrRival);
    generateShortestPaths(heap, nodes);
}


 