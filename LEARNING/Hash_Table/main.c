#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

// Maximum length for person names
#define MAX_NAME 256
// Size of the hash table array
#define TABLE_SIZE 10
// Special pointer value marking deleted entries
#define DELETED_NODE (person*)(0xFFFFFFFFFFFFFFFFUL)

typedef struct {
    char name[MAX_NAME];
    int age;       
} person;

// Array of pointers to person structures
person * hash_table[TABLE_SIZE];

// Hash function to convert a name string to a table index
unsigned int hash(char *name){
    // Calculate string length
    int length = strlen(name);
    unsigned int hash_value = 0;

    // Process each character in the string
    for (int i = 0; i < length; i++){
        hash_value += name[i];  // Add ASCII value
        hash_value = (hash_value * name[i]) % TABLE_SIZE;
    }
    return hash_value; 
}

// Initialize all table entries to NULL (empty)
void init_hash_table(){
    for (int i = 0; i < TABLE_SIZE; i++){
        hash_table[i] = NULL;
    }
    // Empty Hash Table
}

// Display the contents of the hash table
void print_table(){
    printf("\tStart\n");

    for (int i = 0; i < TABLE_SIZE; i++){
        if (hash_table[i] != NULL){  // If position contains a person
            printf("Index --> %i, %s\n", i, hash_table[i]->name);
        }
        else if (hash_table[i] == DELETED_NODE){  // If position marked as deleted
            printf("Index --> %i, DELETED\n", i);
        }
        else{  // If position is empty
            printf("Index --> %i, %s\n", i, hash_table[i]->name);
        }
    }
    printf("\tEnd\n");
}

// Insert a person into the hash table using linear probing for collision resolution
bool hash_table_insert(person *p){
    if (p == NULL) return false;  // Reject NULL pointers
    
    int index = hash(p->name);  // Calculate initial position
    
    // Linear probing to find available slot
    for (int i = 0; i < TABLE_SIZE; i++){
        int try = (index + i) % TABLE_SIZE;  // Calculate probe position
        if (hash_table[try] == NULL || hash_table[try] == DELETED_NODE){
            hash_table[try] = p;  // Store pointer at available position
            return true;  // Success
        }
    }
    return false;  // Table full, insertion failed
    
    // Note: Code below is unreachable due to the return statement above
    if (hash_table[index] != NULL) return false;
    hash_table[index] = p;
    return true;
}

// Look up a person by name using linear probing
person *hash_table_lookup(char *name){
    int index = hash(name);  // Calculate initial position
    
    // Linear probing to find the entry
    for(int i = 0; i < TABLE_SIZE; i++){
        int try = (index + i) % TABLE_SIZE;  // Calculate probe position
        if (hash_table[try] == NULL) return NULL;  // Empty slot means not found
        if (hash_table[try] == DELETED_NODE) continue;  // Skip deleted entries

        // Compare names (limited to TABLE_SIZE characters)
        if (strncmp(hash_table[try]->name, name, TABLE_SIZE) == 0){
            return hash_table[try];  // Return pointer if found
        }
    }
    return NULL;  // Not found
}

// Delete a person by name using linear probing
person *hash_table_delete(char *name){
    int index = hash(name);  // Calculate initial position
    
    // Linear probing to find the entry
    for(int i = 0; i < TABLE_SIZE; i++){
        int try = (index + i) % TABLE_SIZE;  // Calculate probe position
        if (hash_table[try] == NULL) return NULL;  // Empty slot means not found
        if (hash_table[try] == DELETED_NODE) continue;  // Skip deleted entries

        // Compare names (limited to TABLE_SIZE characters)
        if (strncmp(hash_table[try]->name, name, TABLE_SIZE) == 0){
            person *tmp = hash_table[try];  // Save pointer to return
            hash_table[try] = DELETED_NODE;  // Mark as deleted
            return tmp;  // Return the deleted entry
        }
    }
    return NULL;  // Not found
}

int main(){
    init_hash_table();
    print_table();

    person john = {.name = "JOHN", .age = 20};
    person martin = {.name = "MARTIN", .age = 25};
    person sara = {.name = "SARA", .age = 30};
    person kang = {.name = "KANG", .age = 35};
    person jaggu = {.name = "JAGGU",.age = 40};
    person emma = {.name = "EMMA", .age = 28};
    person alex = {.name = "ALEX", .age = 33};
    person sophia = {.name = "SOPHIA", .age = 27};
    person michael = {.name = "MICHAEL", .age = 42};
    person olivia = {.name = "OLIVIA", .age = 31};
    
    hash_table_insert(&john);
    hash_table_insert(&martin);
    hash_table_insert(&sara);
    hash_table_insert(&kang);
    hash_table_insert(&jaggu);
    hash_table_insert(&emma);
    hash_table_insert(&alex);
    hash_table_insert(&sophia);
    hash_table_insert(&michael);
    hash_table_insert(&olivia);

    print_table();

    // Example of looking up a person
    // person *tmp_person = hash_table_lookup("JOHN");
    // if (tmp_person == NULL){
    //     printf("Not found\n");
    // }else{
    //     printf("Found %s\n", tmp_person->name);
    // }
    // tmp_person = hash_table_lookup("JAGUAR");
    // if (tmp_person == NULL){
    //     printf("Not found\n");
    // }else{
    //     printf("Found %s\n", tmp_person->name);
    // }

    // Example of deleting a person
    // tmp_person = hash_table_delete("JOHN");
    // if (tmp_person == NULL){
    //     printf("Not found\n");
    // }else{
    //     printf("Deleted %s\n", tmp_person->name);
    // }

    // print_table();

    // Print hash values for different names
    // printf("MARTIN --> %d\n", hash("MARTIN"));
    // printf("JOHN --> %d\n", hash("JOHN"));
    // printf("SARA --> %d\n", hash("SARA"));
    // printf("KANG --> %d\n", hash("KANG"));
    // printf("JAGGU --> %d\n", hash("JAGGU"));

    return 0; 
}


