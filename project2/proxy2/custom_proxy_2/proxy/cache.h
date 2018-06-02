LinkedList* clinit();
Node* nodeinit(char* url, char* object, int size);
Node* deleteOldest(LinkedList *sl);
void add(LinkedList *sl, Node *newNode);
Node* search(LinkedList *sl, char* url);
void p_cache_list(LinkedList *cachelist);
