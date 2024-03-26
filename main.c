// Omar Alfoqahaa 1200207
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define STRING_SIZE 27
#define LINE_SIZE 100

typedef struct Element { // struct element
    char *keyWord;
    char *meaning1;
    char *meaning2;
    char *meaning3;
    char status;

} Element;

typedef struct HashTable { // struct hash table
    Element *table;
    int size;
    int numberOfElements;
    int numberOFCollision;
} HashTable;

void insertElement(Element *element, HashTable *quadraticHash, HashTable *doubleHash); // prototype

int numberOfLines(const char *filename) { // function to count number of lines in file
    FILE *file = fopen(filename, "r");
    int countElements = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            countElements++;
        }
    }
    fclose(file);

    return countElements;
}

int isPrime(int num) {   //Function to check if the number is prime
    if (num <= 1)
        return 0;

    for (int i = 2; i <= num / 2; i++)
        if (num % i == 0)
            return 0;

    return 1;
}

int nextPrimeNumber(int numberOfElements) {        //Function to get the table size by check the first prime number after the given number

    while (!isPrime(numberOfElements))
        numberOfElements++;

    return numberOfElements;
}

int tableSize(const char *filename) {  // function to count the suitable table size
    int n = numberOfLines(filename);
    int table_size = ceil(n / 0.7);

    return nextPrimeNumber(table_size);

}

int asciiValue(char const KeyWord[STRING_SIZE]) {   //function to get the ascii value of a string
    int asciiValue = 0;
    int i = 0;
    while (KeyWord[i] != '\0') {
        asciiValue += KeyWord[i];
        i++;
    }
    return asciiValue;
}

int quadraticHashing(char keyWord[STRING_SIZE], HashTable *table,int i) {    //get the hash value for from the Quadratic function
    int key = asciiValue(keyWord);
    int HashValue =(key + i * i) % table->size; // hi(key) = [h(key) + i2] % tableSize , for i = 0, 1, . . . , tableSize – 1

    return HashValue;
}

int doubleHashing(char keyWord[STRING_SIZE], HashTable *table, int i) {    //get the hash value for from the Double function

    int key = asciiValue(keyWord);
    int HashValue = (key + i * (1 + (key % (table->size - 1)))) %
                    table->size; // hi(key) = [h(key) + i*hp(key)]% tableSize for i = 0, 1, . . . , tableSize – 1 || hp(key) = 1 + key % (tableSize - 1)

    return HashValue;
}

Element *createEmptyElement() { // create empty element
    Element *emptyElement = (Element *) malloc(sizeof(Element));

    if (emptyElement != NULL) {
        // memory allocation
        emptyElement->status = 'E';
        emptyElement->keyWord = (char *) malloc(sizeof(char) * STRING_SIZE);
        emptyElement->meaning1 = (char *) malloc(sizeof(char) * STRING_SIZE);
        emptyElement->meaning2 = (char *) malloc(sizeof(char) * STRING_SIZE);
        emptyElement->meaning3 = (char *) malloc(sizeof(char) * STRING_SIZE);

        // initialize string members
        emptyElement->keyWord[0] = '\0';
        emptyElement->meaning1[0] = '\0';
        emptyElement->meaning2[0] = '\0';
        emptyElement->meaning3[0] = '\0';
    }

    return emptyElement;
}

void creatHashTable(HashTable *quadraticHash, HashTable *doubleHash,int size) {  //creat a hash table by set all elements to null for first
    quadraticHash->table = (Element *) malloc(sizeof(Element) * size); // memory allocation for quadratic
    quadraticHash->size = size;
    quadraticHash->numberOfElements = 0;
    quadraticHash->numberOFCollision = 0;

    doubleHash->table = (Element *) malloc(sizeof(Element) * size); // memory allocation for double
    doubleHash->size = size;
    doubleHash->numberOfElements = 0;
    doubleHash->numberOFCollision = 0;

    if (quadraticHash->table != NULL && doubleHash->table != NULL) {
        for (int i = 0; i < size; i++) {
            quadraticHash->table[i] = *createEmptyElement();
            doubleHash->table[i] = *createEmptyElement();
        }

    }

}


void rehash(HashTable *quadraticHash, HashTable *doubleHash) { // rehash when Load factor > 0.7

    int newSize = quadraticHash->size * 2; // new size by double it
    newSize = nextPrimeNumber(newSize); // find the next prime to get the suitable size value

    HashTable newQuadraticHash;
    HashTable newDoubleHash;


    creatHashTable(&newQuadraticHash, &newDoubleHash, newSize);

    for (int i = 0; i < quadraticHash->size; ++i) {  // copy the old  elements to new bigger tables
        if (quadraticHash->table[i].status == 'O' || quadraticHash->table[i].status == 'D' ) {
            insertElement(&quadraticHash->table[i], &newQuadraticHash, &newDoubleHash);
        }
    }


    *quadraticHash = newQuadraticHash; // reassign
    *doubleHash = newDoubleHash;

}

void insertElement(Element *element, HashTable *quadraticHash, HashTable *doubleHash) { // insert element to both (Quadratic & Double table)

    int i = 0;
    int quadraticIndex = quadraticHashing(element->keyWord, quadraticHash, i);

    while (quadraticHash->table[quadraticIndex].status == 'O') {
        quadraticHash->numberOFCollision++;
        printf("collision in Quadratic: %d, that occurred on: %s \n", quadraticHash->numberOFCollision, element->keyWord);
        quadraticIndex = quadraticHashing(element->keyWord, quadraticHash, ++i);
    }


    quadraticHash->table[quadraticIndex] = *element;
    quadraticHash->table[quadraticIndex].status = 'O';
    quadraticHash->numberOfElements++;

    i = 0;
    int doubleIndex = doubleHashing(element->keyWord, doubleHash, i);


    while (doubleHash->table[doubleIndex].status == 'O') {
        doubleHash->numberOFCollision++;
        printf("collision in Double: %d, that occurred on: %s\n", doubleHash->numberOFCollision, element->keyWord);
        doubleIndex = doubleHashing(element->keyWord, doubleHash, ++i);
    }

    doubleHash->table[doubleIndex] = *element;
    doubleHash->table[doubleIndex].status = 'O';
    doubleHash->numberOfElements++;

    double loadFactor = (double) quadraticHash->numberOfElements / quadraticHash->size;
    if (loadFactor > 0.7) { // rehash
        rehash(quadraticHash, doubleHash);
    }

}


void printHash(HashTable *hashTable) {  //print the hash table function
    printf("%-15s %-10s %-15s %-15s %-15s %-15s\n", "index", "status", "value", "Meaning 1", "Meaning 2", "Meaning 3");
    for (int i = 0; i < hashTable->size; i++) {
        if (hashTable->table[i].status == 'E')
            printf("%-15d %-10s %-15s %-15s %-15s %-15s\n", i, "E", "NULL", "NULL", "NULL", "NULL");
        else if (hashTable->table[i].status == 'D')
            printf("%-15d %-10s %-15s %-15s %-15s %-15s\n", i, "D", hashTable->table[i].keyWord,
                   hashTable->table[i].meaning1, hashTable->table[i].meaning2, hashTable->table[i].meaning3);
        else
            printf("%-15d %-10s %-15s %-15s %-15s %-15s\n", i, "O", hashTable->table[i].keyWord,
                   hashTable->table[i].meaning1, hashTable->table[i].meaning2, hashTable->table[i].meaning3);
    }
    printf("Num of Collisions: %d\n\n", hashTable->numberOFCollision);
}

int deleteElement(char keyWord[STRING_SIZE], HashTable *quadraticHash, HashTable *doubleHash) { // delete element from both
    int i = 0;
    int quadraticIndex = quadraticHashing(keyWord, quadraticHash, i);

    if (quadraticHash->table[quadraticIndex].status == 'O') {
        while (strcmp(quadraticHash->table[quadraticIndex].keyWord, keyWord) != 0 && i < doubleHash->size) {
            quadraticIndex = quadraticHashing(keyWord, quadraticHash, ++i);
        }

        if (i >= quadraticHash->size) {
            printf("Not Found");
            return 0;
        }


        quadraticHash->numberOfElements--;
        quadraticHash->table[quadraticIndex].status = 'D';
    } else {    // status E || D
        printf("Not Found");
        return 0;
    }

    i = 0;
    int doubleIndex = doubleHashing(keyWord, doubleHash, i);

    if (doubleHash->table[doubleIndex].status == 'O') {
        while (strcmp(doubleHash->table[doubleIndex].keyWord, keyWord) != 0) {
            doubleIndex = doubleHashing(keyWord, doubleHash, ++i);
        }
        if (i >= doubleHash->size) {  // search until specific number of iteration
            printf("Not Found");
            return 0;
        }

        doubleHash->numberOfElements--;
        doubleHash->table[doubleIndex].status = 'D';

    } else {
        printf("Not Found");
        return 0;
    }

    return 1;
}

Element *searchQuadratic(char *keyWord, HashTable *quadraticHash) { // search an element in Quadratic table
    int i = 0;
    int quadraticIndex = quadraticHashing(keyWord, quadraticHash, i);

    if (quadraticHash->table[quadraticIndex].status == 'O') {
        while (strcmp(quadraticHash->table[quadraticIndex].keyWord, keyWord) != 0 && i < quadraticHash->size) { // if 'o' ==> check if the keyWords are same
            quadraticIndex = quadraticHashing(keyWord, quadraticHash, ++i);
        }

        if (i >= quadraticHash->size) // search until specific number of iteration
            return NULL;


        return &quadraticHash->table[quadraticIndex];
    }
    else {
        return NULL;
    }

}

Element *searchDouble(char *keyWord, HashTable *doubleHash) {  // search an element in Double table
    int i = 0;
    int doubleIndex = doubleHashing(keyWord, doubleHash, i);

    if (doubleHash->table[doubleIndex].status == 'O') {
        while (strcmp(doubleHash->table[doubleIndex].keyWord, keyWord) != 0 && i < doubleHash->size) {
            doubleIndex = doubleHashing(keyWord, doubleHash, ++i);
        }

        if (i >= doubleHash->size) // search until specific number of iteration
            return NULL;

        return &doubleHash->table[doubleIndex];

    } else {
        return NULL;
    }

}

void readFile(const char *filename, HashTable *quadraticHash, HashTable *doubleHash) { // read file
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file");
        return;
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove the newline character
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ":");


        char *keyword = NULL;
        char *meaning1 = NULL;
        char *meaning2 = NULL;
        char *meaning3 = NULL;

        if (token != NULL) {
            keyword = strdup(token);
            token = strtok(NULL, "#");
        }

        if (token != NULL) {
            meaning1 = strdup(token);
            token = strtok(NULL, "#");
        }

        if (token != NULL) {
            meaning2 = strdup(token);
            token = strtok(NULL, "#");
        }

        if (token != NULL) {
            meaning3 = strdup(token);
        }

        Element *element = createEmptyElement();
        element->keyWord = keyword;
        element->meaning1 = meaning1;
        element->meaning2 = meaning2;
        element->meaning3 = meaning3;
        insertElement(element, quadraticHash, doubleHash);
    }

    fclose(file);
}


void printTableInfo(HashTable *quadraticHash) {  // print table information
    double loadFactor = (double) quadraticHash->numberOfElements / quadraticHash->size;
    printf("LF= %f   Table Size= %d \n", loadFactor, quadraticHash->size);


}

void printHashFunctions() {  // print hash functions
    printf("Double hashing: hi(key) = [h(key) + i*(1 + key mod (tableSize - 1))] mod tableSize \n");
    printf("Quadratic Hashing: hi(key) = [h(key) + i2] mod tableSize\n");
    printf("where i = 1, 2, 3, 4 ... table size - 1\n");

}

void saveHashTableToFile(HashTable *hashTable) { // save to file
    FILE *out;
    out = fopen(" saved_words.txt", "w");

    for (int i = 0; i < hashTable->size; ++i) {
        if (hashTable->table[i].status == 'O') {
            fprintf(out, "%s:%s#%s#%s\n", hashTable->table[i].keyWord, hashTable->table[i].meaning1,
                    hashTable->table[i].meaning2, hashTable->table[i].meaning3);
        }
    }
    fclose(out);
}

int main() {
    HashTable quadraticHash; // initialize Quadratic table
    HashTable doubleHash;    //  initialize Double table

    int n = tableSize("words.txt");
    creatHashTable(&quadraticHash, &doubleHash, n);

    readFile("words.txt", &quadraticHash, &doubleHash);
    char option;

    do {
        char keyword[STRING_SIZE];
        char meaning1[STRING_SIZE];
        char meaning2[STRING_SIZE];
        char meaning3[STRING_SIZE];

        printf("\nOptions:\n");
        printf("1. Print hashed tables (including empty spots)\n");
        printf("2. Print out table size and the load factor\n");
        printf("3. Print out the used hash functions\n");
        printf("4. Insert a new record to hash table (insertion will be done on both hash tables)\n");
        printf("5. Search for a specific word \n");
        printf("6. Delete a specific word (from both tables)\n");
        printf("7. Compare between the two methods in terms of the number of collisions occurred\n");
        printf("8. Save hash table back to a file saved_words.txt (of the double hashing)\n");
        printf("9. Exit\n");

        printf("Enter your choice (1-9): ");
        scanf(" %c", &option);

        switch (option) {
            case '1':
                printHash(&quadraticHash);
                printHash(&doubleHash);
                break;
            case '2':
                printTableInfo(&quadraticHash);
                break;
            case '3':
                printHashFunctions();
                break;
            case '4':
                printf("enter a key word to insert: \n");
                fflush(stdin);
                gets(keyword);
                printf("enter first meaning: \n");
                gets(meaning1);
                printf("enter second meaning: \n");
                gets(meaning2);
                printf("enter third meaning: \n");
                gets(meaning3);
                Element *newElement = createEmptyElement();
                newElement->keyWord = keyword;
                newElement->meaning1 = meaning1;
                newElement->meaning2 = meaning2;
                newElement->meaning3 = meaning3;
                insertElement(newElement, &quadraticHash, &doubleHash);
                printf("Added successfully\n");

                break;
            case '5':
                do {
                    printf("specify which table to search in: \n"
                           "A. Quadratic\n"
                           "B. Double\n");

                    scanf(" %c", &option);
                } while (option != 'A' && option != 'B' && option != 'a' && option != 'b');
                printf("enter the keyword to search: \n");
                fflush(stdin);
                gets(keyword);

                Element *searchResult;
                if (option == 'A' || option == 'a')
                    searchResult = searchQuadratic(keyword, &quadraticHash);
                else
                    searchResult = searchDouble(keyword, &doubleHash);

                if (searchResult != NULL)
                    printf("Keyword: %-15s \n Meaning 1: %-15s \n Meaning 2: %-15s \n Meaning 3: %-15s\n",
                           searchResult->keyWord, searchResult->meaning1, searchResult->meaning2, searchResult->meaning3);
                else
                    printf("NOT FOUND !! \n");
                break;
            case '6':
                printf("enter the keyword to delete: \n");
                fflush(stdin);
                gets(keyword);

                if (deleteElement(keyword, &quadraticHash, &doubleHash))
                    printf("Deleted Successfully\n");


                break;
            case '7':
                printf("collision in Quadratic: %d \n", quadraticHash.numberOFCollision);
                printf("collision in Double: %d \n", doubleHash.numberOFCollision);

                break;
            case '8':
                saveHashTableToFile(&doubleHash);
                printf("saved successfully");
                break;
            case '9':
                printf("Exiting program.\n");

                exit(0);

            default:
                printf("Invalid choice. Please enter a number between 1 and 9.\n");

        }

    } while (option != 9);

    return 0;
}