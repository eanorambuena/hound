#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef int* pointer;

typedef struct list_node_t {
    pointer value;
    struct list_node_t* next;
} ListNode;

typedef struct list_t {
    ListNode* head;
    ListNode* tail;
} List;

typedef struct pattern_node_t {
    char* value;
    List* neighbors;
} PatternNode;

List* newList () {
    List* list_p = calloc(1, sizeof(List));
    list_p->head = NULL;
    list_p->tail = NULL;
    return list_p;
}

void freeList (List* list_p) {
    if (list_p == NULL) {
        return;
    }
    ListNode* node_p = list_p->head;
    while (node_p != NULL) {
        ListNode* next_p = node_p->next;
        free(node_p);
        node_p = next_p;
    }
    free(list_p);
}

void insert (List* list_p, pointer value) {
    ListNode* node_p = calloc(1, sizeof(ListNode));
    node_p->value = value;
    node_p->next = NULL;
    if (list_p->head == NULL) {
        list_p->head = node_p;
        list_p->tail = node_p;
    } else {
        list_p->tail->next = node_p;
        list_p->tail = node_p;
    }
}

PatternNode* newNode (char* value) {
    PatternNode* node_p = calloc(1, sizeof(PatternNode));
    node_p->value = value;
    node_p->neighbors = newList();
    return node_p;
}

void freePattern (PatternNode* root) {
    // free all nodes in the tree, avoiding repeated free
    // root could be in its own neighbors list
    if (root == NULL) return;
    ListNode* node_p = root->neighbors->head;
    while (node_p != NULL) {
        if (node_p->value != (pointer) root) {
            freePattern((PatternNode*) node_p->value);
        }
        node_p = node_p->next;
    }
    freeList(root->neighbors);
    free(root);
}

bool checkPattern (PatternNode* root, char* string) {
    if (root == NULL) return true;
    if (strlen(string) == 0) return false;
    // check if root matches the first character of string
    if (strlen(root->value) > 0 && strcmp(root->value, "")) {
        if (!strcmp(root->value, "u")) {
            if (string[0] < 'A' || string[0] > 'Z') return false;
        } else if (!strcmp(root->value, "d")) {
            if (string[0] < '0' || string[0] > '9') return false;
        } else {
            if (string[0] != root->value[0]) return false;
        }
    }
    if (!strcmp(root->value, "")) string--;
    // check if root matches the rest of string
    ListNode* node_p = root->neighbors->head;
    while (node_p != NULL) {
        if (checkPattern((PatternNode*) node_p->value, string + 1)) {
            return true;
        }
        node_p = node_p->next;
    }
    return root->neighbors->head == NULL;
}

PatternNode* generatePattern (char* pattern) {
    PatternNode* root = newNode("");
    PatternNode* node_p = root;
    PatternNode* last_node_p = NULL;
    int i = 0;
    while (pattern[i] != '\0') {
        if (pattern[i] == '+') {
            insert(node_p->neighbors, (pointer) node_p);
        } else {
            last_node_p = node_p;
            // create string from pattern[i]
            char* str = calloc(2, sizeof(char));
            str[0] = pattern[i];
            node_p = newNode(str);
            insert(last_node_p->neighbors, (pointer) node_p);
        }
        i++;
    }
    return root;
}

char* boolToResultString (bool b) {
    if (b) return "PASSED";
    return "FAILED";
}

bool assert(bool value, bool expected, bool debug) {
    if (debug) printf("%s\n", boolToResultString(value == expected));
    return value == expected;
}

bool testCheckPattern (bool debug) {
    // pattern u+d 
    PatternNode* node_1_p = newNode("u"); // u
    insert(node_1_p->neighbors, (pointer) node_1_p); // +
    PatternNode* node_2_p = newNode("d");
    insert(node_1_p->neighbors, (pointer) node_2_p); // d
    bool b1 = assert(checkPattern(node_1_p, "A1"), true, debug);
    bool b2 = assert(checkPattern(node_1_p, "A"), false, debug);
    bool b3 = assert(checkPattern(node_1_p, "A1B"), true, debug);
    freePattern(node_1_p);
    return b1 && b2 && b3;
}

bool testGeneratePattern (bool debug) {
    // pattern d+u
    PatternNode* node_1_p = generatePattern("d+u");
    bool b1 = assert(checkPattern(node_1_p, "1A"), true, debug);
    bool b2 = assert(checkPattern(node_1_p, "1"), false, debug);
    bool b3 = assert(checkPattern(node_1_p, "1AB"), true, debug);
    bool b4 = assert(checkPattern(node_1_p, "1A1"), true, debug);
    bool b5 = assert(checkPattern(node_1_p, "12A"), true, debug);
    bool b6 = assert(checkPattern(node_1_p, "A1"), false, debug);
    freePattern(node_1_p);
    return b1 && b2 && b3 && b4 && b5 && b6;
}

int main () {
    testCheckPattern(true);
    testGeneratePattern(true);
    return 0;
}