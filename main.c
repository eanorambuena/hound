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

typedef struct pattern_t {
    char* name;
    PatternNode* root;
} Pattern;

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

void freePatternNode (PatternNode* root) {
    // free all nodes in the tree, avoiding repeated free
    // root could be in its own neighbors list
    if (root == NULL) return;
    ListNode* node_p = root->neighbors->head;
    while (node_p != NULL) {
        if (node_p->value != (pointer) root) {
            freePatternNode((PatternNode*) node_p->value);
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

Pattern* newPattern (char* name, char* pattern) {
    Pattern* pattern_p = calloc(1, sizeof(Pattern));
    pattern_p->name = name;
    pattern_p->root = generatePattern(pattern);
    return pattern_p;
}

void freePattern (Pattern* pattern_p) {
    freePatternNode(pattern_p->root);
    free(pattern_p);
}

int match (Pattern* pattern_p, char* string) {
    // return first index of string that matches pattern
    int i = 0;
    while (string[i] != '\0') {
        if (checkPattern(pattern_p->root, string + i)) {
            return i;
        }
        i++;
    }
    return -1;
}

char* boolToResultString (bool b) {
    if (b) return "Test \033[1;32mPASSED\033[1;0m";
    return "Test \033[1;31mFAILED\033[1;0m";
}

bool assert(bool value, bool expected, bool debug) {
    if (debug) printf("%s\n", boolToResultString(value == expected));
    return value == expected;
}

bool summarize (bool (*f) (bool)) {
    bool result = f(false);
    printf("SUMMARY: %s\n", boolToResultString(result));
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
    freePatternNode(node_1_p);
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
    freePatternNode(node_1_p);
    PatternNode* String = generatePattern("'u+'");
    PatternNode* Integer = generatePattern("d+");
    bool b7 = assert(checkPattern(Integer, "1"), true, debug);
    bool b8 = assert(checkPattern(Integer, "A"), false, debug);
    bool b9 = assert(checkPattern(Integer, "1A"), true, debug);
    bool b10 = assert(checkPattern(Integer, "A1"), false, debug);
    bool b11 = assert(checkPattern(String, "'ABC'"), true, debug);
    bool b12 = assert(checkPattern(Integer, "xy123"), true, debug);
    bool b13 = assert(checkPattern(String, "12"), true, debug);
    freePatternNode(Integer);
    freePatternNode(String);
    return b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9 && b10 && b11 && b12 && b13;
}

bool testMatch (bool debug) {
    Pattern* pattern_p = newPattern("ID", "d+u");
    bool b1 = assert(match(pattern_p, "1A") == 0, true, debug);
    bool b2 = assert(match(pattern_p, "1") == -1, true, debug);
    bool b3 = assert(match(pattern_p, "1AB") == 0, true, debug);
    bool b4 = assert(match(pattern_p, "1A1") == 0, true, debug);
    bool b5 = assert(match(pattern_p, "12A") == 0, true, debug);
    bool b6 = assert(match(pattern_p, "A1") == -1, true, debug);
    bool b7 = assert(match(pattern_p, "A1B") == 1, true, debug);
    bool b8 = assert(match(pattern_p, "abc123B2") == 3, true, debug);
    bool b9 = assert(match(pattern_p, "abc123b2") == -1, true, debug);
    bool b10 = assert(match(pattern_p, "abc123b2D") == 7, true, debug);
    freePattern(pattern_p);
    return b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9 && b10;
}

int main () {
    summarize(testCheckPattern);
    testGeneratePattern(true);
    summarize(testMatch);
    return 0;
}