#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_OPER_NUM 10000
#define MAX_NODE_NUM 500
#define INT_MAX 2147483647

typedef struct vertex {
  int name;
  int key;
  struct vertex *next;
} vertex;

typedef struct priorityQueue {
  int size;
  vertex *arr[MAX_NODE_NUM];
  int position[MAX_NODE_NUM + 1];  // index is the name of vertex, value is the
                                   // index of vertex in arr
} priorityQueue;

void initQueue(priorityQueue *pq, int nodeNum) {
  pq->size = nodeNum;
  for (int i = 0; i < nodeNum; i++) {
    pq->arr[i] = (vertex *)malloc(sizeof(vertex));
    pq->arr[i]->name = i + 1;
    pq->arr[i]->key = INT_MAX;
    pq->position[i + 1] = i;
  }
}

void initVertexLinkedList(vertex *vertexLinkedList[], int nodeNum) {
  for (int i = 1; i <= nodeNum; i++) {
    vertexLinkedList[i] = NULL;
  }
}

void swap(vertex **a, vertex **b) {
  vertex *temp = *a;
  *a = *b;
  *b = temp;
}

void heapifyUp(priorityQueue *pq, int index) {
  if (index == 0) {
    return;
  }
  int parent = (index - 1) / 2;
  if (pq->arr[parent]->key > pq->arr[index]->key) {
    swap(&pq->arr[parent], &pq->arr[index]);
    pq->position[pq->arr[parent]->name] = parent;
    pq->position[pq->arr[index]->name] = index;
    heapifyUp(pq, parent);
  }
}

void heapifyDown(priorityQueue *pq, int index) {
  int left = index * 2 + 1;
  int right = index * 2 + 2;
  int smallest = index;
  if (left < pq->size && pq->arr[left]->key < pq->arr[smallest]->key) {
    smallest = left;
  }
  if (right < pq->size && pq->arr[right]->key < pq->arr[smallest]->key) {
    smallest = right;
  }
  if (smallest != index) {
    swap(&pq->arr[smallest], &pq->arr[index]);
    pq->position[pq->arr[smallest]->name] = smallest;
    pq->position[pq->arr[index]->name] = index;
    heapifyDown(pq, smallest);
  }
}

vertex *extractMin(priorityQueue *pq) {
  if (pq->size == 0) {
    return NULL;
  }
  vertex *min = pq->arr[0];
  pq->arr[0] = pq->arr[pq->size - 1];
  pq->size--;
  heapifyDown(pq, 0);
  return min;
}

void insertEdge(int *weight[], int *numbers, vertex *vertexLinkedList[]) {
  if (weight[numbers[0]][numbers[1]] == 0) {
    weight[numbers[0]][numbers[1]] = numbers[2];
    weight[numbers[1]][numbers[0]] = numbers[2];

    // insert vertex into linked list head
    vertex *newVertex = (vertex *)malloc(sizeof(vertex));
    newVertex->name = numbers[1];
    newVertex->key = numbers[2];

    if (vertexLinkedList[numbers[0]] != NULL) {
      vertex *temp = vertexLinkedList[numbers[0]];
      vertexLinkedList[numbers[0]] = newVertex;
      newVertex->next = temp;
    } else {
      newVertex->next = NULL;
      vertexLinkedList[numbers[0]] = newVertex;
    }

    // insert the opposite direction
    newVertex = (vertex *)malloc(sizeof(vertex));
    newVertex->name = numbers[0];
    newVertex->key = numbers[2];

    if (vertexLinkedList[numbers[1]] != NULL) {
      vertex *temp = vertexLinkedList[numbers[1]];
      vertexLinkedList[numbers[1]] = newVertex;
      newVertex->next = temp;
    } else {
      newVertex->next = NULL;
      vertexLinkedList[numbers[1]] = newVertex;
    }
  }
}

void deleteEdge(int *weight[], int *numbers, vertex *vertexLinkedList[]) {
  if (weight[numbers[0]][numbers[1]] != 0) {
    weight[numbers[0]][numbers[1]] = 0;
    weight[numbers[1]][numbers[0]] = 0;

    // delete vertex from linked list
    vertex *temp = vertexLinkedList[numbers[0]];
    vertex *prev = NULL;
    while (temp != NULL) {
      if (temp->name == numbers[1]) {
        if (prev == NULL) {
          vertexLinkedList[numbers[0]] = temp->next;
        } else {
          prev->next = temp->next;
        }
        free(temp);
        break;
      }
      prev = temp;
      temp = temp->next;
    }

    // delete the opposite direction
    temp = vertexLinkedList[numbers[1]];
    prev = NULL;
    while (temp != NULL) {
      if (temp->name == numbers[0]) {
        if (prev == NULL) {
          vertexLinkedList[numbers[1]] = temp->next;
        } else {
          prev->next = temp->next;
        }
        free(temp);
        break;
      }
      prev = temp;
      temp = temp->next;
    }
  }
}

void changeWeight(int *weight[], int *numbers, vertex *vertexLinkedList[]) {
  if (weight[numbers[0]][numbers[1]] != 0) {
    weight[numbers[0]][numbers[1]] = numbers[2];
    weight[numbers[1]][numbers[0]] = numbers[2];

    // change vertex in linked list
    vertex *temp = vertexLinkedList[numbers[0]];
    while (temp != NULL) {
      if (temp->name == numbers[1]) {
        temp->key = numbers[2];
        break;
      }
      temp = temp->next;
    }

    // change the opposite direction
    temp = vertexLinkedList[numbers[1]];
    while (temp != NULL) {
      if (temp->name == numbers[0]) {
        temp->key = numbers[2];
        break;
      }
      temp = temp->next;
    }
  }
}

void findMST(priorityQueue *pq, int *weight[], int nodeNum,
             vertex *vertexLinkedList[]) {
  int MST = 0;
  pq->arr[0]->key = 0;
  int connected[nodeNum];
  for (int i = 0; i < nodeNum; i++) {
    connected[i] = 0;
  }

  while (pq->size != 0) {
    vertex *u = extractMin(pq);
    connected[u->name - 1] = 1;
    if (u->key == INT_MAX) {
      //printf("u->name: %d\n", u->name);
      //printf("u->key: %d\n", u->key);

      printf("Disconnected\n");
      return;
    }
    MST += u->key;

    // update key of adjacent vertices
    vertex *temp = vertexLinkedList[u->name];
    while (temp != NULL) {
      if (connected[temp->name - 1] == 0 &&
          pq->arr[pq->position[temp->name]]->key > weight[u->name][temp->name]) {
        pq->arr[pq->position[temp->name]]->key = weight[u->name][temp->name];
        heapifyUp(pq, pq->position[temp->name]);
      }
      temp = temp->next;
    }
  }
  printf("%d\n", MST);
}

int main() {
  // clock start
  clock_t start, end;
  double cpu_time_used;
  start = clock();

  FILE *file = fopen("mst_in.txt", "r");

  int operNum = 0;
  int nodeNum = 0;
  fscanf(file, "%d", &nodeNum);

  int **weight = (int **)malloc(sizeof(int *) * (nodeNum + 1));
  for (int i = 0; i <= nodeNum; i++) {
    weight[i] = (int *)malloc(sizeof(int) * (nodeNum + 1));
    for (int j = 0; j <= nodeNum; j++) {
      weight[i][j] = 0;
    }
  }

  priorityQueue pq;
  initQueue(&pq, nodeNum);

  vertex *vertexLinkedList[MAX_NODE_NUM + 1];
  initVertexLinkedList(vertexLinkedList, nodeNum);

  char oper[15];  // for storing operation temporarily
  int numbers[3];

  while (operNum < MAX_OPER_NUM && fscanf(file, "%s", oper) != EOF) {
    if (strcmp(oper, "insertEdge") == 0) {
      fscanf(file, "%d %d %d", &numbers[0], &numbers[1], &numbers[2]);
      insertEdge(weight, numbers, vertexLinkedList);
    } else if (strcmp(oper, "changeWeight") == 0) {
      fscanf(file, "%d %d %d", &numbers[0], &numbers[1], &numbers[2]);
      changeWeight(weight, numbers, vertexLinkedList);
    } else if (strcmp(oper, "deleteEdge") == 0) {
      fscanf(file, "%d %d", &numbers[0], &numbers[1]);
      deleteEdge(weight, numbers, vertexLinkedList);
    } else {
      initQueue(&pq, nodeNum);
      findMST(&pq, weight, nodeNum, vertexLinkedList);
    }
    operNum++;
  }

  fclose(file);

  // clock end
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("time: %f\n", cpu_time_used);

  return 0;
}
