#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "s-Mart.h"

#define NUM_CUSTOMERS 10
#define NUM_STOCKERS ITEMS_LENGTH

// Global variable for the store.
sMart *smrt;

/**
 * Thread funtion that represents a customer. A customer:
 *  - allocates space (memory) for a purchase.
 *  - selects an item.
 *  - removes the item from the "shelf".
 */
void* sMartCustomer(void* tid) {
    int customer_id = (int)(long) tid;
    int total_purchases = 0;
    int item_index = PickRandomStoreItem();
    int result = Purchase(smrt, item_index);
  
    while (result >= 0){
        if (result == 1){
            total_purchases++;
        }
        item_index = PickRandomStoreItem();
        result = Purchase(smrt, item_index);
    }
    
    printf("Customer #%d purchased %d items.\n", customer_id, total_purchases);
	  return NULL;
}

/**
 * Thread function that represents a stocker in the store. A stocker:
 *  - restocks a specifc item on the shelf.
 * The stocker restocks the shelf with it's respective item until there is
 * no more in storage.
 */
void* sMartStocker(void* tid) {
    int stocker_id = (int)(long) tid;
    int items_restocked = 0;
    int result = Restock(smrt, stocker_id);
    
    //Check if valid
    while (result >= 0){
        items_restocked++;
        result = Restock(smrt, stocker_id);
    }
    
    printf("Stocker #%d restocked %d items\n", stocker_id, items_restocked);
    return NULL;
}

/**
 * Runs when the program begins executing. This program:
 *  - opens the store
 *  - creates customers and restockers
 *  - waits for all customers and restockers to be done
 *  - closes the store.
 */
int main() {
    smrt = OpenStore();
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t stockers[NUM_STOCKERS];
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&(customers[i]), NULL, sMartCustomer, (void *)(long)i);
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        pthread_create(&(stockers[i]), NULL, sMartStocker, (void *)(long)i);
    }
    
    // Wait for all customer and stocker threads.
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        printf("Joining Customer Thread %d...\n", i);
        pthread_join(customers[i], NULL);
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        printf("Joining Stocker Thread %d...\n", i);
        pthread_join(stockers[i], NULL);
    }
    
    CloseStore(smrt);
    return 0;
}