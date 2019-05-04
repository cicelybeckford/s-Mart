#ifndef OSFINALPROJECT_SMART_H_
#define OSFINALPROJECT_SMART_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ITEMS_LENGTH 5

// Store item is of type string
typedef char* ItemType;

// Contents of an Order.
typedef struct ItemStruct {
    ItemType item_type;
    int item_index;
    int shelf_count;
    int storage_count;
    int max_shelf_size;
    bool sold_out;
    //pthread_mutex_t mutex;
    pthread_cond_t can_purchase, can_stock;
} Item;

// A store contains:
//  - An array of items
//  - The number of items purchased
//  - The number of items that are sold out
//  - Expected number of purchases
//  - Synchronization objects:
//    - A lock, required to modify any part of the restaurant
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
 * Picks a random store item and returns it.
 */
int PickRandomStoreItem();

/**
 * Creates a store with an expected number of purchases.
 * Returns the store.
 * 
 * This function:
 *  - allocates space for the store
 *  - initializes all its variables
 *  - initializes its synchronization object
 */
sMart* OpenStore(int expected_num_purchases);

/**
 * Closes the store. This function:
 *  - ensures all orders have been fulfilled
 *  - ensures the number of orders fulfilled matches the expected number of orders
 *  - destroys all the synchronization objects
 *  - frees the space of the store
 */
void CloseStore(sMart* smrt);
  
/**
 * Restock an item to the shelf. This function:
 *  - waits until the shelf is not full
 *  - adds the item to the shelf from storage
 *  - returns the item
 */
int Restock(sMart* smrt, int index);

/**
 * Gets an item from the shelf. This function:
 *  - Waits until the shelf is not empty
 *  - gets an item from the shelf
 * 
 * If the item is sold out, this function will notify the other customers
 * that this item is sold out.
 */

int Purchase(sMart* smrt, int index);

#endif  // OSFINALPROJECT_SMART_H_