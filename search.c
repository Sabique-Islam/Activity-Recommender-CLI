/*
Objectives:-

> Take a user prompt (words or sentences).
> Check it against the database (glove.6B.50d.txt).
> Perform cosine similarity search between the user prompt and the database.
> Returns the top 5 results (in descending order of cosine similarity).

*/

// Header files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Macro definitions
#define EMBEDDING_DIM 50


// Function declarations

// Cosine similarity function (Tested) [for prompt (checked in database) and database keywords]
float cosine_similarity(float *vec1, float *vec2){  
    float dot_product = 0.0, mag1 = 0.0, mag2 = 0.0;

    for(int i=0; i<EMBEDDING_DIM;i++){
        dot_product+=vec1[i]*vec2[i];
        mag1+=vec1[i]*vec1[i];
        mag2+=vec2[i]*vec2[i];
    }

    if (mag1==0.0 ||mag2==0.0 ){
        return 0.0;
    }

    //printf("Dot Product: %f, Mag1: %f, Mag2: %f\n", dot_product, mag1, mag2);

    float similarity = dot_product/(sqrt(mag1)*sqrt(mag2));
    return similarity; // range:- [-1,1]
}

int main(){

    char buffer[100];
    char show[100];

    FILE *embedding = fopen("./data/glove.6B.50d.txt","r");
    FILE *activity = fopen("./data/activities.csv","r");

    fgets(show, sizeof(show), activity); //To skip the header

    // Reading activity File
    for(int i=0; i<=1076; i++){
        fgets(show, sizeof(show), activity);
        printf("%s",show);
    }

    if (embedding==NULL || activity==NULL){
        printf("File doesn't exist or given path is incorrect.\n");
        exit(1);
    }

    // while(1){
    // printf("Enter a word or a sentence: ");
    // fgets(buffer,100,stdin);

    // // Remove the newline character that fgets might add
    // buffer[strcspn(buffer, "\n")] = '\0';

    // printf("You entered: %s\n",buffer);
    // }   

    return 0;
}