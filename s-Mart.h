#ifndef OSFINALPROJECT_SMART_H_
#define OSFINALPROJECT_SMART_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ITEMS_LENGTH 10

// Let a menu item be a string.
typedef char* ItemType;

// Contents of an Order.
typedef struct ItemStruct {
    ItemType item_type;
    int item_index;
    int shelf_count;
    int storage_count;
    int max_shelf_size;
    bool sold_out;
    pthread_cond_t can_purchase, can_stock;
} Item;

// A restuarant contains:
//  - An array of orders
//  - its current size (the number of orders currently handled by the restaurant)
//  - its max size (the maximum number of orders the restaurant can handle)
//  - The order number of the upcoming order
//  - The number of orders fulfilled
//  - The number of orders the restaurant expects to fulfill
//  - Synchronization objects:
//    - A lock, required to modify any part of the restaurant
//    - condition variables, used to ensure the restaurant is only
//      modified when it is able to receive orders (not full)
//      or fulfill orders (not empty).
typedef struct Store {
    Item* items[ITEMS_LENGTH];
//     int current_size;
//     int max_size;
//     int next_order_number;
    int items_purchased;
    int items_sold_out;
    int expected_num_purchases;
    pthread_mutex_t mutex;
} sMart;

/**
 * Picks a random menu item and returns it.
 */
int PickRandomStoreItem();

/**
 * Creates a restaurant with a maximum size and the expected number of orders.
 * Returns the restaurant.
 * 
 * This function should:
 *  - allocate space for the restaurant
 *  - initialize all its variables
 *  - initialize its synchronization objects
 */
sMart* OpenStore(int expected_num_purchases);

/**
 * Closes the restaurant. This function should:
 *  - ensure all orders have been fulfilled
 *  - ensure the number of orders fulfilled matches the expected number of orders
 *  - destroy all the synchronization objects
 *  - free the space of the restaurant
 */
void CloseStore(sMart* smrt);
  
/**
 * Add an order to the restaurant. This function should:
 *  - Wait until the restaurant is not full
 *  - Add an order to the back of the orders queue
 *  - populate the order number of the order
 *  - return the order number
 */
int Restock(sMart* smrt, int index);

/**
 * Gets an order from the restaurant. This funtion should:
 *  - Wait until the restaurant is not empty
 *  - get an order from the front of the orders queue
 *  - return the order
 * 
 * If there are no orders left, this function should notify the other cooks
 * that there are no orders left.
 */

int Purchase(sMart* smrt, int index);

#endif  // OSFINALPROJECT_SMART_H_