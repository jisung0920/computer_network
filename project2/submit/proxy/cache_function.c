#include "proxy.h"
#include "cache.h"

LinkedList* clinit() {
    LinkedList* ptr =(LinkedList*) malloc(sizeof(LinkedList));
    ptr->remainder_size=MAX_CACHE_SIZE;
    ptr->header=NULL;

    return ptr;
}

Node* nodeinit(char* url, char* object, int size) {
    if (size > MAX_OBJECT_SIZE) return NULL;
    Node* ptr = (Node*) malloc(sizeof(Node));
    bzero(ptr->url, MAX_BUF_SIZE);
    bzero(ptr->object, MAX_OBJECT_SIZE);
    strcpy(ptr->url, url);
    memcpy(ptr->object, object, size);
    ptr->object_size= size;
    time_t current_time;
    time(&current_time);
    ptr->timestemp=current_time;

    ptr->next = NULL;

    return ptr;
}

Node* deleteOldest(LinkedList *sl) {
    // write your code..
    if (sl->header != NULL) {
        Node* tmp = sl->header;
        sl->remainder_size += tmp->object_size;
        sl->header = tmp->next;

        return tmp;
    } else return NULL;
}

/*
 * add newNode to dl's last position
 */
void add(LinkedList *sl, Node *newNode) {

    if (newNode == NULL) return;
    // write your code..
    if (sl->remainder_size < newNode->object_size) {
        deleteOldest(sl);
        add(sl, newNode);
    }
    if (sl->header!=NULL) {
        Node* tmp = sl->header;
        while((tmp->next)!=NULL) tmp = tmp->next;
        tmp->next = newNode;

        sl->remainder_size -= newNode->object_size;
    } else sl->header = newNode;
}

/*
 * search Node to sl's url
 */
Node* search(LinkedList *sl, char* url) {
    if (sl->header == NULL)
      return NULL;

    Node* pre = sl->header;

    if (strcmp(pre->url, url) == 0)
      return pre;

    Node* tmp = sl->header->next;

    if (tmp == NULL)
      return NULL;

    while(strcmp(tmp->url, url)!=0) {
        pre = pre->next;
        tmp = tmp->next;
        if (tmp == NULL)
          break;
    }

    if (tmp==NULL)
      return NULL;

    pre->next = tmp->next;
    tmp->next = NULL;
    sl->remainder_size += tmp->object_size;
    add(sl, tmp);

    return tmp;
}

void p_cache_list(LinkedList *cachelist){
      Node* node = cachelist->header;
      int count =1;
      if(node!=NULL){
        printf("\n\n## cache url list===========\n");
        while(node!=NULL){
          printf("    [%2d] : %s (size : %d )\n",count++,node->url,node->object_size );
          node = node->next;
        }
        printf("============================\n\n");
      }
}
