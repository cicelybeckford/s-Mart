#ifndef OSFINALPROJECT_SMART_H_
#define OSFINALPROJECT_SMART_H_

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define ITEMS_LENGTH 10

// Store item is of type string
typedef char* ItemType;

// Contents of an Item.
//  - The index of an item
//  - The current number of that item on the shelf
//  - The current number of that item in storage_count
//  - The maximum size of the shelf
//  - Synchronization objects:
//   - condition variables, used to ensure the item is only
//      modified when it is able to be restocked (not full)
//      or be purchased (not empty).
typedef struct ItemStruct {
    ItemType item_type;
    int item_index;
    int shelf_count;
    int storage_count;
    int max_shelf_size;
    pthread_cond_t can_purchase, can_stock;
} Item;

// A store contains:
//  - An array of Items
//  - The number of items purchased
//  - The number of items that are sold out
//  - The total number of items in the store
//  - Synchronization objects:
//    - A lock, required to modify any part of the store
typedef struct Store {
    Item* items[ITEMS_LENGTH];
    int items_purchased;
    int items_sold_out;
    int total_num_items;
    pthread_mutex_t mutex;
} sMart;

/**
 * Picks a random store item and returns it.
 */
int PickRandomStoreItem();

/**
 * Creates a store with a preset Items.
 * Returns the store.
 * 
 * This function:
 *  - allocates space for the store
 *  - initializes all its variables
 *  - initializes its synchronization object
 */
sMart* OpenStore();

/**
 * Closes the store. This function:
 *  - ensures there are no more items in the store
 *  - destroys all the synchronization objects
 *  - frees the space of the store
 */
void CloseStore(sMart* smrt);
  
/**
 * Restock an item to the shelf. This function:
 *  - waits until the shelf is not full
 *  - takes an item from storage, and adds it to the shelf
 *  - returns the item index
 */
int Restock(sMart* smrt, int index);

/**
 * Gets an item from the shelf. This function:
 *  - Waits until the shelf is not empty
 *  - gets an item from the shelf
 *  - returns the status of that purchase attempt
 *   - 0: the item is sold out, 1: the purchase was successful
 *     -1: the store is empty (every item sold out)
 * 
 * If all items have been sold, this function will notify the other customers
 * that the store is empty.
 */

int Purchase(sMart* smrt, int index);

#endif  // OSFINALPROJECT_SMART_H_