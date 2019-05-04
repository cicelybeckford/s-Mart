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
int StorageCount[] = {20, 20, 20, 20, 20, 21, 40, 70, 65, 75};
int ItemsLength = 10; // MUST BE THE SAME AS ITEMS_LENGTH IN "s-Mart.h"

int PickRandomStoreItem() {
    return rand()%ItemsLength;
}

sMart* OpenStore() {
    sMart* smrt = NULL;
    smrt = malloc(sizeof(sMart)); 
    smrt->total_num_items = 0;
    int i;
    
    for (i = 0; i < ItemsLength; i++){
        smrt->items[i] = ConstructItem(i);
        smrt->total_num_items = smrt->total_num_items + ShelfCount[i] + StorageCount[i];
    }
  
    smrt->items_purchased = 0;
    smrt->items_sold_out = 0;
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
    printf("Total number of items in store when opened: %d\n", smrt->total_num_items);
    free(smrt);
    
    printf("Store is closed!\n");
}

int Restock(sMart* smrt, int index) {
    pthread_mutex_lock(&smrt->mutex);
    
    // wait until shelf is not full to restock
    while (ShelfFull(smrt->items[index])) {
        pthread_cond_wait(&smrt->items[index]->can_stock, &smrt->mutex);
    }
    smrt->items[index]->storage_count--;
    smrt->items[index]->shelf_count++;
    
    // if there are no more items in storage, the stocker's job is done
    if (smrt->items[index]->storage_count == 0){
        pthread_cond_signal(&smrt->items[index]->can_purchase);
        pthread_mutex_unlock(&smrt->mutex);
        return -1; // break from thread
        }
    
    pthread_cond_signal(&smrt->items[index]->can_purchase);
    pthread_mutex_unlock(&smrt->mutex);
    return index;
}

int Purchase(sMart* smrt, int index) {
    pthread_mutex_lock(&smrt->mutex);
    
    // wait until shelf is not empty to purchase
    while (ShelfEmpty(smrt->items[index])) {
        if (IsSoldOut(smrt->items[index])){
            pthread_cond_broadcast(&smrt->items[index]->can_purchase);
            pthread_mutex_unlock(&smrt->mutex);
            if (smrt->items_sold_out == ItemsLength){
                return -1; //stop thread
            }
            return 0; // pick a random item again
        }
        pthread_cond_wait(&smrt->items[index]->can_purchase, &smrt->mutex);
    }
  
    // Get item from shelf.
    smrt->items[index]->shelf_count--;
    smrt->items_purchased++;
  
    if (IsSoldOut(smrt->items[index])){
        smrt->items_sold_out++;
    }
    else {
        pthread_cond_signal(&smrt->items[index]->can_stock);
    }
  
    // if store is empty, notify other customers
    if (smrt->items_sold_out == ItemsLength){
        pthread_cond_broadcast(&smrt->items[index]->can_purchase);
        pthread_mutex_unlock(&smrt->mutex);
        return -1;
    }
    
    pthread_mutex_unlock(&smrt->mutex);
    return 1;
}

// Helper functions

// Populate Items array
Item* ConstructItem(int index){
    Item* item = NULL;
    item = malloc(sizeof(Item)); 
    item->item_type = Items[index];
    item->shelf_count = ShelfCount[index];
    item->max_shelf_size = ShelfCount[index];
    item->storage_count = StorageCount[index];
    pthread_cond_init(&item->can_purchase, NULL);
    pthread_cond_init(&item->can_stock, NULL);
    return item;
}

// Destroy all the synchronization objects and free space
void DestroyItem(Item* item){
    pthread_cond_destroy(&item->can_purchase);
    pthread_cond_destroy(&item->can_stock);
    free(item); 
    return;
}

bool ShelfEmpty(Item* item) {
    return (item->shelf_count == 0);
}

bool ShelfFull(Item* item) {
    return (item->shelf_count == item->max_shelf_size);
}

bool IsSoldOut(Item* item) {
    return (item->shelf_count == 0 && item->storage_count == 0);
}