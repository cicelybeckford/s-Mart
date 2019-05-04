#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "s-Mart.h"

#define MCGOOGLES_SIZE 100
#define NUM_CUSTOMERS 50
#define NUM_STOCKERS 10
#define PURCHASES_PER_CUSTOMER 10
#define EXPECTED_NUM_PURCHASES NUM_CUSTOMERS * PURCHASES_PER_CUSTOMER

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
    int total_purchases_attempted = 0;
    int item_index = PickRandomStoreItem();
    int result = Purchase(smrt, item_index);
  
    while (result >= 0){
        total_purchases_attempted++;
        if (result == 1){
            total_purchases_attempted++;
            //printf("Customer #%d purchased item #%d\n", customer_id, item_index);
        }
        else if (result == 0){
            //printf("Customer #%d tried to purchase item #%d but it was sold out\n", customer_id, item_index);
        }
        item_index = PickRandomStoreItem();
        result = Purchase(smrt, item_index);
    }
    
    printf("Customer #%d attempted %d purchases\n", customer_id, total_purchases_attempted);
	  return NULL;
}

/**
 * Thread function that represents a restocker in the store. A restocker:
 *  - restocks a specifc item on the shelf.
 * The restocker restocks the shelf with it's respective item until there is
 * no more in storage.
 */
void* sMartStocker(void* tid) {
    int stocker_id = (int)(long) tid;
    int items_restocked = 0;
    int result = Restock(smrt, stocker_id);
    
    //Check if valid
    while (result >= 0){
        items_restocked++;
//         printf("Item #%d left in storage: %d\n", stocker_id, smrt->items[stocker_id]->storage_count);
//         printf("Item #%d left in shelf: %d\n", stocker_id, smrt->items[stocker_id]->shelf_count);
        result = Restock(smrt, stocker_id);
//         printf("%d - result  %d - stocker\n", result, stocker_id);
//         printf("stock\n");
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
//         printf("cus thread\n");
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        pthread_create(&(stockers[i]), NULL, sMartStocker, (void *)(long)i);
//         printf("stock thread\n");
    }
    
    // Wait for all customer and cook threads.
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        printf("Joining Cust Thread %d...\n", i);
        pthread_join(customers[i], NULL);
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        printf("Joining Stock Thread %d...\n", i);
        pthread_join(stockers[i], NULL);
    }
    
    CloseStore(smrt);

    return 0;
}