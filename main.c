#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "s-Mart.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define MCGOOGLES_SIZE 100
#define NUM_CUSTOMERS 10
#define NUM_STOCKERS 5
#define PURCHASES_PER_CUSTOMER 10
#define EXPECTED_NUM_PURCHASES NUM_CUSTOMERS * PURCHASES_PER_CUSTOMER

// Global variable for the restaurant.
sMart *smrt;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* sMartCustomer(void* tid) {
    int customer_id = (int)(long) tid;
    int total_purchases_attempted = 0;
    int item_index;
    
    for (int i = 0; i < PURCHASES_PER_CUSTOMER; i++) {
        item_index = PickRandomStoreItem();
        int result = Purchase(smrt, item_index);
        if (result >= 0){
            total_purchases_attempted++;
            printf("Customer #%d purchased item #%d\n", customer_id, result);
        }
        else{
            printf("Customer #%d tried to purchase item #%d but it was sold out\n", customer_id, item_index);
        }
    }
    
    printf("Customer #%d attempted %d purchases\n", customer_id, total_purchases_attempted);
	return NULL;
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* sMartStocker(void* tid) {
    int stocker_id = (int)(long) tid;
    int items_restocked = 0;
    int result = Restock(smrt, stocker_id);
    
    //Check if valid
    while (result >= 0){
        items_restocked++;
        printf("Item #%d left in storage: %d\n", stocker_id, smrt->items[stocker_id]->storage_count);
        result = Restock(smrt, stocker_id);
    }
    
    printf("Stocker #%d restocked %d items\n", stocker_id, items_restocked);
    return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    smrt = OpenStore(EXPECTED_NUM_PURCHASES);
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t stockers[NUM_STOCKERS];
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&(customers[i]), NULL, sMartCustomer, (void *)(long)i);
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        pthread_create(&(stockers[i]), NULL, sMartStocker, (void *)(long)i);
    }
    
    // Wait for all customer and cook threads.
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
    for (int i = 0; i < NUM_STOCKERS; i++) {
        pthread_join(stockers[i], NULL);
    }
    
    CloseStore(smrt);

    return 0;
}