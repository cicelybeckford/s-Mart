#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "s-Mart.h"


StoreItem* item;

int main() {
  
    dummyItem = (StoreItem*) malloc(sizeof(StoreItem));
    dummyItem->data = -1;  
    dummyItem->key = -1; 

    insert(milk, 20);
    insert(eggs, 70);
    insert(cereal, 80);
    insert(snacks, 25);
    insert(trash_can, 44);
    insert(toothbrush, 32);
    insert(body_wash, 11);
    insert(shampoo, 78);
    insert(fruits, 97);

    display();
    item = search(milk);

    if(item != NULL) {
        printf("Element found: %d\n", item->data);
    } else {
        printf("Element not found\n");
    }

    delete(item);
    item = search(snacks);

    if(item != NULL) {
      printf("Element found: %d\n", item->data);
    } else {
      printf("Element not found\n");
    }
}