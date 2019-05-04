#include "McGoogles.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(McGoogles* mcg);
bool IsFull(McGoogles* mcg);
void AddOrderToBack(Order **orders, Order *order);

Item* ConstructItem(int index);


ItemType Items[] = {"Eggs", "Milk", "Detergent", "Toothbrush", "Body Wash", "Water",
                    "Meat", "Rice", "Vegetables", "Fruits"};
int ShelfCount[] = {5, 6, 4, 5, 9, 6, 4, 10, 7, 8};
int StorageCount[] = {53, 57, 20, 39, 12, 21, 40, 70, 65, 75};
int ItemsLength = 10;

MenuItem PickRandomStoreItem() {
    time_t t;
    srand((unsigned)time(&t));
    return rand()%ItemsLength;
}

sMart* OpenStore(int expected_num_purchases) {
    sMart* smrt = NULL;
    smrt = malloc(sizeof(sMart)); 
    int i;
    for (i = 0; i < ItemsLength; i++){
        smrt->items[i] = ConstructItem[i];
    }
    smrt->items_purchased = 0;
    srmt->expected_num_purchases = expected_num_purchases;
    pthread_mutex_init(&smrt->mutex, NULL);
    
    printf("Store is open!\n");
    return smrt;
}

void CloseStore(sMart* smrt) {
    if (IsEmpty(mcg)){
        pthread_mutex_destroy(&mcg->mutex);
        pthread_cond_destroy(&mcg->can_add_orders);
        pthread_cond_destroy(&mcg->can_get_orders);
        printf("Fulfilled %d orders and expected %d.\n", mcg->orders_handled,
           mcg->expected_num_orders);
        free(mcg);
    }
    
    printf("Restaurant is closed!\n");
}

int Restock(sMart* smrt, int index) {
    pthread_mutex_lock(&smrt->mutex);
    
    // add to front if there are no orders
    while (ShelfFull(smrt, index)) {
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

Order *GetOrder(McGoogles* mcg) {
    pthread_mutex_lock(&smrt->mutex);
    
    while (IsEmpty(mcg)) {
        // if there are no orders left, notify other cooks
        if (smrt->orders_handled == mcg->expected_num_orders){
            pthread_cond_broadcast(&mcg->can_get_orders);
            pthread_mutex_unlock(&mcg->mutex);
            return NULL;
        }
        // wait until the restaurant is no longer empty
        pthread_cond_wait(&mcg->can_get_orders, &mcg->mutex);
    }
    
    // Get order from front of queue.
    Order *front = mcg->orders;
    mcg->orders = mcg->orders->next;
    front->next = NULL;
    
    mcg->current_size--;
    mcg->orders_handled++;
    
    pthread_cond_signal(&mcg->can_add_orders);
    pthread_mutex_unlock(&mcg->mutex);
    return front;
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
bool IsEmpty(McGoogles* mcg) {
  return (mcg->current_size == 0);
}

bool ShelfFull(sMart* smrt, int index) {
  return (smrt->items[index]->shelf_count 
          == smrt->items[index]->max_shelf_size);
}

void AddOrderToBack(Order **orders, Order *order) {}