#include "s-Mart.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool ShelfEmpty(Item* item);
bool ShelfFull(Item* item);
bool IsSoldOut(Item* item);

Item* ConstructItem(int index);
void DestroyItem(Item* item);


ItemType Items[] = {"Eggs", "Milk", "Detergent", "Toothbrush", "Body Wash", "Water",
                    "Meat", "Rice", "Vegetables", "Fruits"};
int ShelfCount[] = {5, 6, 4, 5, 9, 6, 4, 10, 7, 8};
int StorageCount[] = {53, 57, 20, 39, 12, 21, 40, 70, 65, 75};

int ItemsLength = 10;

int PickRandomStoreItem() {
    time_t t;
    srand((unsigned)time(&t));
    return rand()%ItemsLength;
}

sMart* OpenStore(int expected_num_purchases) {
    sMart* smrt = NULL;
    smrt = malloc(sizeof(sMart)); 
    int i;
    for (i = 0; i < ItemsLength; i++){
        smrt->items[i] = ConstructItem(i);
        printf("%s\n", smrt->items[i]->item_type);
    }
    smrt->items_purchased = 0;
    smrt->expected_num_purchases = expected_num_purchases;
    pthread_mutex_init(&smrt->mutex, NULL);
    
    printf("Store is open!\n");
    return smrt;
}

void CloseStore(sMart* smrt) {
    pthread_mutex_destroy(&smrt->mutex);
    int i;
    for (i = 0; i < ItemsLength; i++){
        DestroyItem(smrt->items[i]);
    }
    printf("\nItems Purchased: %d\n", smrt->items_purchased);
    printf("Number of times a customer came for an item that was sold out: %d\n", smrt->items_sold_out);
    printf("\nTotal items handled: %d\n", smrt->items_purchased + smrt->items_sold_out);
    printf("\nExpected Number of Items Handled: %d\n", smrt->expected_num_purchases);
    free(smrt);
    
    printf("Store is closed!\n");
}

int Restock(sMart* smrt, int index) {
    pthread_mutex_lock(&smrt->mutex);
    
    // add to front if there are no orders
    while (ShelfFull(smrt->items[index])) {
        if (smrt->items_purchased == smrt->expected_num_purchases 
            || smrt->items[index]->storage_count == 0){
            pthread_cond_broadcast(&smrt->items[index]->can_stock);
            pthread_mutex_unlock(&smrt->mutex);
            return -1;
        }
        pthread_cond_wait(&smrt->items[index]->can_stock, &smrt->mutex);
    }

    // add order to back
    smrt->items[index]->storage_count--;
    smrt->items[index]->shelf_count++;
    
    // set order number and increment the next order number
    
    pthread_cond_signal(&smrt->items[index]->can_purchase);
    pthread_mutex_unlock(&smrt->mutex);
    return index;
}

int Purchase(sMart* smrt, int index) {
    pthread_mutex_lock(&smrt->mutex);
    
    while (ShelfEmpty(smrt->items[index])) {
        // if there are no orders left, notify other cooks
        if (IsSoldOut(smrt->items[index])){
            smrt->items_sold_out++;
            pthread_cond_broadcast(&smrt->items[index]->can_purchase);
            pthread_mutex_unlock(&smrt->mutex);
            return -1;
        }
        // wait until the restaurant is no longer empty
        pthread_cond_wait(&smrt->items[index]->can_purchase, &smrt->mutex);
    }
    
    // Get order from front of queue.
    smrt->items[index]->shelf_count--;
    smrt->items_purchased++;
    
    pthread_cond_signal(&smrt->items[index]->can_stock);
    pthread_mutex_unlock(&smrt->mutex);
    return index;
}

// Optional helper functions (you can implement if you think they would be useful)

Item* ConstructItem(int index){
    Item* item = NULL;
    item = malloc(sizeof(Item)); 
    item->item_type = Items[index];
    item->shelf_count = ShelfCount[index];
    item->max_shelf_size = ShelfCount[index];
    item->storage_count = StorageCount[index];
    item->sold_out = false;
    pthread_cond_init(&item->can_purchase, NULL);
    pthread_cond_init(&item->can_stock, NULL);
    return item;
}

void DestroyItem(Item* item){
    pthread_cond_destroy(&item->can_purchase);
    pthread_cond_destroy(&item->can_stock);
    return;
}

bool ShelfEmpty(Item* item) {
  return (item->shelf_count == 0);
}

bool ShelfFull(Item* item) {
  return (item->shelf_count == item->max_shelf_size);
}

bool IsSoldOut(Item* item) {
    return item->shelf_count == 0 && item->storage_count == 0;
}