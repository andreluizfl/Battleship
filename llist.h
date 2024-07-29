#ifndef LLIST_H
#define LLIST_H

#include <stdlib.h>
#include <stdbool.h>

/*
* Tipos definidos e Estrutura de dados
*/

typedef unsigned long PointerValue;
typedef unsigned long* Pointer;
typedef const unsigned long* CPointer;

//EventHandler
typedef void(* EventHandler_Destroy)(CPointer ptr);

typedef int(* EventHandler_Compare)(CPointer obj1, CPointer obj2);

typedef void* (*EventHandler_Copy)(CPointer obj, Pointer data);

typedef void(* EventHandler_Print)(CPointer obj, Pointer data);

typedef int(* EventHandler_Custom)(Pointer data, Pointer user_data);


typedef struct _SNode SNode;

typedef struct LList{
  const size_t count;
  EventHandler_Compare compare;
}LList;

/*
* Protótipos de Funções
*/

LList* llist_new(EventHandler_Compare comp);            //Cria(aloca) uma nova lista encadeada

void llist_destroy(LList* list);                        //Destrói (desaloca) a lista encadeada

void llist_destroy_full(LList* list,
                        EventHandler_Destroy destroy);  //Destrói (desaloca) a lista encadeada e todos os elementos

LList* llist_copy_new(const LList* list);               //Cria(aloca) uma nova lista encadeada como cópia de outra

LList* llist_copy_new_data(const LList* source,
                       EventHandler_Copy cpy,
                       Pointer additional_data);        //Copia os elementos de uma lista encadeada em outra

void llist_copy(LList* dest,const LList* source);       //Copia uma lista encadeada em outra

void llist_copy_data(LList* dest, const LList* source,
                     EventHandler_Copy cpy,
                     Pointer additional_data);          //Copia uma lista encadeada em outra juntamente com seu conteúdo

void llist_add_first( LList* list,
                     CPointer val);                     //Adiciona um elemento no inicio da lista

void llist_add_last( LList* list,
                    CPointer val);                      //Adiciona um elemento no fim na lista encadeada

void llist_add_ordered( LList* list,
                       CPointer val);                   //Adiciona um elemento ordenadamente na lista

bool llist_add_ordered_unique(LList* list,
                    CPointer val);                      //Adiciona um elemento ordenadamente na lista com unicidade

Pointer llist_remove_first( LList* list);               //Remove o primeiro elemento da da lista encadeada

Pointer llist_remove_last( LList* list);                //Remove o último elemento da lista encadeada

bool llist_remove_element( LList* list,
                          CPointer element);            //Remove um elemento da lista encadeada

Pointer llist_remove_element_at( LList* list,
                          int idx);                   //Remove um elemento da lista encadeada na posição idx

bool llist_remove_element_full( LList* list,
                          CPointer element,
                          EventHandler_Destroy destroy);//Remove um elemento da lista encadeada e o desaloca

void llist_clear( LList* list);                         //Remove todos elementos da lista encadeada

void llist_clear_full(LList* list,
                      EventHandler_Destroy destroy);    //Remove todos elementos da lista encadeada desalocando os elementos

Pointer llist_get(LList* list,
                const int index);                       //Obtém um valor de um elemento da lista encadeada

void llist_set(LList* list,
               const int index,
               CPointer value);                         //Atribui um valor a um elemento da lista encadeada

bool llist_contains(const LList* list,
                    CPointer value);                    //Verifica se um elemento está contido na lista encadeada

Pointer llist_get_accesedvalue(const LList* list);      //Obtém o último valor acessado

void llist_foreach(LList* list,
                   EventHandler_Custom user_func,
                   Pointer user_data);                  //Chamada de função customizada para cada elemento

void llist_sort(LList* list);                           //Ordena os elementos da lista de acordo com a função de ordenação

void llist_print( const LList* list,
                 EventHandler_Print print );            //Imprime todos elementos da lista encadeada

int llist_compare_asc(CPointer a,
                      CPointer b);                      //Função de comparação ascendente

int llist_compare_des(CPointer a,
                      CPointer b);                      //Função de comparação descendente

bool llist_check_sorted(LList* list);                   //Função que checa se a lista está ordenada


/*
*  Definição da estrutura privada
*/

typedef struct _SNode
{
  Pointer value;
  struct _SNode* next;
}_SNode;

typedef struct _LList
{
  size_t count;
  EventHandler_Compare compare;
  SNode* head;
  SNode* tail;
  int last_index_acess;
  SNode* last_node_acess;
}_LList;

/*
* Implementação das Funções
*/

//Cria(aloca) uma nova lista encadeada
LList* llist_new(EventHandler_Compare compare){
    size_t memory_size = sizeof(_LList);
    _LList* l = (_LList*)malloc(memory_size);
    memset(l,0,memory_size);
    if( l != NULL )
    {
        l->last_index_acess = -1;
        if(compare != NULL){
            l->compare = compare;
        }else{
            l->compare = (EventHandler_Compare)llist_compare_asc;
        }
    }else{
        printf("llist_new. LList memory allocation failed!");
    }
    return (LList*)l;
}

//Destrói (desaloca) a lista encadeada
void llist_destroy(LList* l){
    if(l!=NULL){
        llist_clear(l);
        free((void*)l);
        l = NULL;
    }else{
        printf("llist_destroy. LList null pointer!");
    }
}

//Destrói (desaloca) a lista encadeada e todos os elementos
void llist_destroy_full(LList* l, EventHandler_Destroy destroy){
    if(l!=NULL){
        if(destroy!=NULL){
            llist_clear_full(l,destroy);
        }else{
            printf("llist_destroy_full. Destroy handler is null pointer. No element will be destroyed!");
            llist_clear(l);
        }
        free((void*)l);
        l = NULL;
    }else{
        printf("llist_destroy_full. LList null pointer!");
    }
}

//Cria(aloca) uma nova lista encadeada como cópia de outra
LList* llist_copy_new(const LList* list)
{
    LList* l = llist_new(list->compare);
    _SNode* curr;
    if(l!=NULL){
        curr = ((_LList*)list)->head;
        while( curr != NULL  )
        {
            llist_add_last(l,curr->value);
            curr = curr->next;
        }
    }else{
        printf("llist_copy_new. New LList memory allocation failed");
    }
    return l;
}

//Copia os elementos de uma lista encadeada em outra
LList* llist_copy_new_data(const LList* list, EventHandler_Copy cpy, Pointer additional_data){
    LList* l = NULL;
    _SNode* curr;
    if(cpy != NULL){
        l = llist_new(list->compare);
        if(l!=NULL){
            curr = ((_LList*)list)->head;
            while( curr != NULL  ){
                llist_add_last(l,(CPointer)cpy((CPointer)curr->value,additional_data));
                curr = curr->next;
            }
        }else{
            printf("llist_copy_new_data. New LList memory allocation failed");
        }
    }else{
        printf("llist_copy_new_data. Copy handler is null  pointer");
    }
    return l;
}

//Copia uma lista encadeada em outra
void llist_copy(LList* dest,const LList* source){
    _SNode* curr;
    if(dest==NULL){
        printf("llist_copy. Destination LList cannot be null pointer");
        return;
    }
    if(source==NULL){
        printf("llist_copy. Source LList cannot be null pointer");
        return;
    }
    dest->compare = source->compare;
    curr = ((_LList*)source)->head;
    while( curr != NULL  )
    {
        llist_add_last(dest,curr->value);
        curr = curr->next;
    }
}

//Copia uma lista encadeada em outra juntamente com seu conteúdo
void llist_copy_data(LList* dest, const LList* source, EventHandler_Copy cpy, Pointer additional_data){
    if(dest==NULL){
        printf("llist_copy_data. Destination LList cannot be null pointer");
        return;
    }
    if(source==NULL){
        printf("llist_copy_data. Source LList cannot be null pointer");
        return;
    }
    if(cpy==NULL){
        printf("llist_copy_data. Copy handler cannot be null pointer");
        return;
    }
    dest->compare = source->compare;
    _SNode* curr = ((_LList*)source)->head;
    while( curr != NULL  ){
        if(cpy != NULL){
            llist_add_last(dest,(CPointer)cpy(curr->value,additional_data));
        }else{
            llist_add_last(dest,NULL);
        }
        curr = curr->next;
    }
}

//Adiciona um elemento no inicio da lista
void llist_add_first(LList* list, CPointer val)
{
    _LList* l = (_LList*)list;
    _SNode* node;
    if( l == NULL )
    {
        printf("llist_add_first. LList is null pointer");
        return;
    }
    if( (l->head == NULL && l->tail != NULL) || (l->head != NULL && l->tail == NULL) )
    {
        printf("llist_add_first. LList internal problem. There is something wrong with your assignment of head/tail to the list");
        return;
    }else{
        node = (_SNode*)malloc(sizeof(_SNode));
        if(node == NULL){
            printf("llist_add_first. SNode memory allocation failed");
            return;
        }
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        node->value = (Pointer)val;
        node->next = l->head;
        l->head = node;
        if(l->tail==NULL){
            l->tail = node;
        }
        l->count++;
    }
}

//Adiciona um elemento no fim na lista encadeada
void llist_add_last( LList* list, CPointer val)
{
    _LList* l = (_LList*)list;
    _SNode* node;
    if( l == NULL )
    {
        printf("llist_add_last. LList is null pointer");
        return;
    }
    if( (l->head == NULL && l->tail != NULL) || (l->head != NULL && l->tail == NULL) )
    {
        printf("llist_add_last. LList internal problem. There is something wrong with your assignment of head/tail to the list");
        return;
    }else{
        node = (_SNode*)malloc(sizeof(_SNode));
        if(node == NULL){
            printf("llist_add_first. SNode memory allocation failed");
            return;
        }
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        node->value = (Pointer)val;
        node->next = NULL;
        if(l->tail != NULL){
            l->tail->next = node;
        }
        l->tail = node;
        if(l->head == NULL){
            l->head = node;
        }
        l->count++;
        l->last_index_acess = -1;
    }
}

//Adiciona um elemento no inicio da lista
void llist_add_ordered( LList* list, CPointer val){
    _LList* l = (_LList*)list;
    _SNode* a = NULL;
    _SNode* c;
    _SNode* node;
    if( l == NULL )
    {
        printf("llist_add_ordered. LList is null pointer");
        return;
    }
    if(l->compare == NULL){
        printf("llist_add_ordered. Compare handler is null pointer. The element will be added at last position!");
        llist_add_last(list,val);
    }else{
        node = (_SNode*)malloc(sizeof(_SNode));
        if(node == NULL){
            printf("llist_add_ordered. SNode memory allocation failed");
            return;
        }
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        node->value = (Pointer)val;
        node->next = NULL;
        c = l->head;
        while(c!=NULL && l->compare(c->value,val) < 0){
            a = c;
            c = c->next;
        }
        if( c != NULL){
            if(a==NULL){
                l->head = node;
                node->next = c;
            }else{
                a->next = node;
                node->next = c;
            }
        }else{
            if(a!=NULL){
                a->next = node;
                l->tail = node;
            }else{
                l->head = node;
                l->tail = node;
            }
        }
        l->count++;
    }
}

//Adiciona um elemento ordenadamente na lista com unicidade
bool llist_add_ordered_unique(LList* list, CPointer val){
    int diff=-1;
    _LList* l = (_LList*)list;
    _SNode* a = NULL;
    _SNode* c;
    _SNode* node;
    if( l == NULL )
    {
        printf("llist_add_ordered. LList is null pointer");
        return false;
    }
    if(l->compare == NULL){
        printf("llist_add_ordered. Compare handler is null pointer. The element will be added at last position!");
        llist_add_last(list,val);
        return true;
    }else{
        c = l->head;
        while(c!=NULL && (diff = l->compare(c->value,val)) < 0){
            a = c;
            c = c->next;
        }
        if(diff!=0){
            node = (_SNode*)malloc(sizeof(_SNode));
            if(node == NULL){
                printf("llist_add_ordered. SNode memory allocation failed");
                return false;
            }
            l->last_index_acess = -1;
            l->last_node_acess = NULL;
            node->value = (Pointer)val;
            node->next = NULL;
            if(c != NULL){
                if(a==NULL){
                    l->head = node;
                    node->next = c;
                }else{
                    a->next = node;
                    node->next = c;
                }
            }else{
                if(a!=NULL){
                    a->next = node;
                    l->tail = node;
                }else{
                    l->head = node;
                    l->tail = node;
                }
            }
            l->count++;
            return true;
        }else{
            return false;
        }
    }
}

//Remove o primeiro elemento da da lista encadeada
Pointer llist_remove_first( LList* list){
    _SNode* h = NULL;
    _SNode* p = NULL;
    _LList* l = (_LList*)list;
    Pointer value;

    if( l == NULL )
    {
        printf("llist_remove_first. LList is null pointer");
        return NULL;
    }
    else{
        if( l->head == NULL && l->tail == NULL )
        {
            return NULL;
        }
        else if( (l->head == NULL && l->tail != NULL) || (l->head != NULL && l->tail == NULL) )
        {
            printf("llist_remove_first. There is something seriously wrong with your list. One of the head/tail is empty while other is not");
            return NULL;
        }
    }
    l->last_index_acess = -1;
    l->last_node_acess = NULL;
    h = l->head;
    p = h->next;
    value = h->value;
    if(l->head == l->tail){
        l->head = NULL;
        l->tail = NULL;
    }else{
        l->head = p;
    }
    l->count--;
    free(h);
    return value;
}

//Remove o último elemento da lista encadeada
Pointer llist_remove_last( LList* list){
    int i;
    _SNode* t = NULL;
    _SNode* a = NULL;
    _LList* l = (_LList*)list;
    Pointer value;

    if( l == NULL )
    {
        printf("llist_remove_first. LList is null pointer");
        return NULL;
    }
    else{
        if( l->head == NULL && l->tail == NULL )
        {
            return NULL;
        }
        else if( l->head == NULL  || l->tail == NULL )
        {
            printf("llist_remove_first. There is something seriously wrong with your list. One of the head/tail is empty while other is not");
            return NULL;
        }
    }
    l->last_index_acess = -1;
    l->last_node_acess = NULL;
    a = l->head;
    for(i=0; i < ((int)l->count)-2; i++){
        a = a->next;
    }
    t = l->tail;
    value = t->value;
    if(l->head == l->tail){
        l->tail = NULL;
        l->head = NULL;
    }else{
        l->tail = a;
        a->next = NULL;
    }
    l->count--;
    free(t);
    return value;
}

//Remove um elemento da lista encadeada
bool llist_remove_element( LList* list, CPointer element){
    _SNode* a = NULL;
    _SNode* c;
    _LList* l = (_LList*)list;
    bool not_equal;
    if( l == NULL )
    {
        printf("llist_remove_element. LList is null pointer");
        return false;
    }
    else{
        if( l->head == NULL && l->tail == NULL )
        {
            return false;
        }
        else if( l->head == NULL  || l->tail == NULL )
        {
            printf("llist_remove_element. There is something seriously wrong with your list. One of the head/tail is empty while other is not");
            return false;
        }
    }
    c = l->head;
    while(c!= NULL && (not_equal = (list->compare(c->value,element) != 0)) ){
        a = c;
        c = c->next;
    }
    if(c!=NULL && !not_equal){
        if(a==NULL){
            l->head = c->next;
            if(c->next == NULL)
                l->tail = NULL;
        }else{
            if(c == l->tail){
                l->tail = a;
                l->tail->next = NULL;
            }else{
                a->next = c->next;
            }
        }
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        l->count--;
        free(c);
        return true;
    }else{
        return false;
    }
}

//Remove um elemento da lista encadeada na posição idx
Pointer llist_remove_element_at( LList* list, int idx){
    Pointer element = NULL;
    _SNode* a = NULL;
    _SNode* c;
    _LList* l = (_LList*)list;
    if( l == NULL )
    {
        printf("llist_remove_element. LList is null pointer");
        return element;
    }
    else{
        if( l->head == NULL && l->tail == NULL )
        {
            return element;
        }
        else if( l->head == NULL  || l->tail == NULL )
        {
            printf("llist_remove_element. There is something seriously wrong with your list. One of the head/tail is empty while other is not");
            return element;
        }
    }
    c = l->head;
    while(c!= NULL && idx>0){
        a = c;
        c = c->next;
        --idx;
    }
    if(c!=NULL && idx>=0){
        element = c->value;
        if(a==NULL){
            l->head = c->next;
            if(c->next == NULL)
                l->tail = NULL;
        }else{
            if(c == l->tail){
                l->tail = a;
                l->tail->next = NULL;
            }else{
                a->next = c->next;
            }
        }
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        l->count--;
        free(c);
    }
    return element;
}
//Remove um elemento da lista encadeada e o desaloca
bool llist_remove_element_full( LList* list,
                          CPointer element,
                          EventHandler_Destroy destroy)
{
    _SNode* a = NULL;
    _SNode* c;
    _LList* l = (_LList*)list;
    c = l->head;
    while(c!= NULL && list->compare(c->value,element) != 0){
        a = c;
        c = c->next;
    }
    if(c!=NULL && list->compare(c->value,element) == 0){
        //if(memorymanager_isregistred(c->value))
            //system("pause");
        if(a==NULL){
            l->head = c->next;
            if(c->next == NULL)
                l->tail = NULL;
        }else{
            if(c == l->tail){
                l->tail = a;
                l->tail->next = NULL;
            }else{
                a->next = c->next;
            }
        }
        l->count--;
        if(destroy!=NULL)
            destroy(c->value);
        free(c);
        l->last_index_acess = -1;
        l->last_node_acess = NULL;
        return true;
    }else{
        return false;
    }
}


//Remove todos elementos da lista encadeada
void llist_clear( LList* list)
{
    _LList* l = ((_LList*)list);
    _SNode* next = NULL;
    _SNode* curr;
    if( l == NULL )
    {
        printf("llist_clear. LList is null pointer");
        return;
    }
    curr = l->head;
    while( curr != NULL  )
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
    l->head = NULL;
    l->tail = NULL;
    l->count = 0;
    l->last_index_acess = -1;
    l->last_node_acess = NULL;
}

//Remove todos elementos da lista encadeada desalocando os elementos
void llist_clear_full( LList* list, EventHandler_Destroy destroy){
    _LList* l = ((_LList*)list);
    _SNode* next = NULL;
    _SNode* curr;
    if( l == NULL )
    {
        printf("llist_clear_full. LList is null pointer");
        return;
    }
    curr = l->head;
    while( curr != NULL  )
    {
        next = curr->next;
        if(destroy != NULL)
            destroy(curr->value);
        free(curr);
        curr = next;
    }
    l->head = NULL;
    l->tail = NULL;
    l->count = 0;
    l->last_index_acess = -1;
    l->last_node_acess = NULL;
}

//Obtém um elemento da lista encadeada
Pointer llist_get(LList* list, int index){
    Pointer value = NULL;
    int index_diff;
    int i = index;
    _LList* l = (_LList*)list;
    if( l == NULL )
    {
        printf("llist_get. LList is null pointer");
        return NULL;
    }
    if(index >= 0 && index < l->count){
        /*
        l->last_node_acess = l->head;
        while(i != 0) {
            i--;
            l->last_node_acess = l->last_node_acess->next;
        }
        l->last_index_acess = index;
        value = l->last_node_acess->value;
*/
        if(l->last_index_acess == -1){
            l->last_node_acess = l->head;
            while(i != 0) {
                i--;
                l->last_node_acess = l->last_node_acess->next;
            }
            l->last_index_acess = index;
            value = l->last_node_acess->value;
        }else{
            //printf("derr. derr");
            //Se há registrado uma última posição acessada
            index_diff = index - l->last_index_acess;
            if(index_diff < 0){
                l->last_node_acess = l->head;
            }
            if(index_diff >= 0){
                i = index_diff;
            }
            while(i != 0) {
                i--;
                l->last_node_acess = l->last_node_acess->next;
            }
            l->last_index_acess = index;
            value = l->last_node_acess->value;
        }
    }else{
        printf("llist_get. Index out of bounds");
    }
    return value;
}

//Atribui um valor a um elemento da lista encadeada
void llist_set(LList* list,const int index, CPointer value){
    int index_diff;
    int i = index;
    _LList* l = (_LList*)list;
    if( l == NULL )
    {
        printf("llist_set. LList is null pointer");
        return;
    }
    if(index >= 0 && index < l->count){

        if(l->last_index_acess == -1){
            //value = l->head->value;
            l->last_node_acess = l->head;
            while(i != 0) {
                i--;
                l->last_node_acess = l->last_node_acess->next;
            }
            l->last_index_acess = index;
            l->last_node_acess->value = (Pointer)value;
        }else{
            //Se há registrado uma última posição acessada
            index_diff = index - l->last_index_acess;
            if(index_diff < 0){
                l->last_node_acess = l->head;
            }
            if(index_diff > 0){
                i = index_diff;
            }
            while(i != 0) {
                i--;
                l->last_node_acess = l->last_node_acess->next;
            }
            if(l->last_node_acess!=NULL){
                l->last_index_acess = index;
                l->last_node_acess->value = (Pointer)value;
            }

        }
    }else{
        printf("llist_get. Index out of bounds");
    }
}

//Verifica se um elemento está contido na lista encadeada
bool llist_contains(const LList* list, CPointer value){
    _LList* l = (_LList*)list;
    _SNode* curr;
    int i = 0;
    if( l == NULL){
        fprintf(stderr, "LINE: %d, METHOD:'llist_contains_element'. Lista não inicializada!\n", __LINE__);
        return false;
    }
    curr =  l->head;
    while(curr != NULL){
        i++;
        if(l->compare(curr->value,value)==0){
            ((_LList*)list)->last_index_acess = i;
            ((_LList*)list)->last_node_acess = curr;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

//Obtém o último valor acessado
Pointer llist_get_accesedvalue(const LList* list){
    _LList* l = (_LList*)list;
    if(l->last_node_acess!=NULL){
        return l->last_node_acess->value;
    }else{
        return NULL;
    }
}

//Chamada de função customizada para cada elemento
void llist_foreach(LList* llist,EventHandler_Custom user_func,Pointer user_data){
    _SNode* n;
    if( llist == NULL){
        printf("llist_constains. LList is null pointer");
        return;
    }
    n = ((_LList*)llist)->head;
    while(n && user_func(n->value,user_data) == 0){
        n = n->next;
    }
}

/* Versão modificada do mergesort de Simon Tatham(2001)
 *
 * This file is copyright 2001 Simon Tatham.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

static void mergesort(LList* l, EventHandler_Compare compare) {
    _LList* list = (_LList*)l;
    _SNode *p, *q, *e, *tail;
    _SNode * head = ((_LList*)list)->head;
    int insize, nmerges, psize, qsize, i;
    //
    // Silly special case: if `list' was passed in as NULL, return
    // NULL immediately.
    //
    if (!head)
        return;
    insize = 1;
    while (1) {
        p = head;
        head = NULL;
        tail = NULL;
        nmerges = 0;  // count number of merges we do in this pass //

        while (p) {
            nmerges++;
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
                q = q->next;
                if (!q)
                    break;
            }
            // if q hasn't fallen off end, we have two heads to merge //
            qsize = insize;
            // now we have two heads; merge them //
            while (psize > 0 || (qsize > 0 && q)) {
                // decide whether next element of merge comes from p or q //
                if (psize == 0) {
                    // p is empty; e must come from q. //
                    e = q; q = q->next; qsize--;

                } else if (qsize == 0 || !q) {
                    // q is empty; e must come from p. //
                    e = p; p = p->next; psize--;
                } else if (compare(p->value,q->value) <= 0) {
                    // First element of p is lower (or same);
                     // e must come from p. //
                    e = p; p = p->next; psize--;
                } else {
                    // First element of q is lower; e must come from q. //
                    e = q; q = q->next; qsize--;
                }
                // add the next element to the merged head //
                if (tail) {
                    tail->next = e;
                } else {
                    head = e;
                }
                tail = e;
            }
            // now p has stepped `insize' places along, and q has too //
            p = q;
        }
	    tail->next = NULL;

        // If we have done only one merge, we're finished. //
        if (nmerges <= 1){   // allow for nmerges==0, the empty head case //
            list->head = head;
            list->tail = tail;
            return;
        }
        // Otherwise repeat, merging heads twice the size //
        insize *= 2;
    }
}

/* Versão de MergeSort não utilizada
//Source:  http://stackoverflow.com/a/3032462
void merge_sort_list(_LList *list,EventHandler_Compare compare)
{
    int currSize=1,numMerges,leftSize,rightSize;
    _SNode *tail,*left,*right,*next,*curr;
    curr = list->head;
    if (!curr || !curr->next) return;  // Trivial case

    do { // For each power of two<=curr length
        numMerges=0;
        left=curr;
        tail=curr=NULL; // Start at the start

        while (left) { // Do this curr_len/currSize times:
            numMerges++,right=left,leftSize=0,rightSize=currSize;
            // Cut curr into two halves (but don't overrun)
            while (right && leftSize<currSize) leftSize++,right=right->next;
            // Run through the currs appending onto what we have so far.
            while (leftSize>0 || (rightSize>0 && right)) {
                // Left empty, take right OR Right empty, take left, OR compare.
                if (!leftSize)                  {next=right;right=right->next;rightSize--;}
                else if (!rightSize || !right)  {next=left;left=left->next;leftSize--;}
                else if (compare(left->value,right->value) <= 0) {next=left;left=left->next;leftSize--;}
                else                            {next=right;right=right->next;rightSize--;}
                // Update pointers to keep track of where we are:
                if (tail) tail->next=next;  else curr=next;
                // Sort prev pointer
                //next->prev=tail; // Optional.
                tail=next;
            }
            // Right is now AFTER the curr we just sorted, so start the next sort there.
            left=right;
        }
        // Terminate the curr, double the curr-sort size.
        tail->next=0,currSize<<=1;
    } while (numMerges>1); // If we only did one merge, then we just sorted the whole curr.
    list->head = curr;
    list->tail = tail;
}
*/

//Ordena os elementos da lista de acordo com a função de ordenação
void llist_sort(LList* list){
    if(list == NULL){
        printf("llist_sort. LList is null pointer");
        return;
    }
    if(list->compare == NULL){
        printf("llist_sort. Compare handler is null pointer");
        return;
    }
    mergesort(list,list->compare);
    ((_LList*)list)->last_index_acess = -1;
    //merge_sort_list(list,list->compare);
}

/*//Imprime um elemento da lista encadeada
static void llist_print_element(const _SNode* node , EventHandler_Print print){
    if( node ){
        print(node->value,NULL);
    }else{
        printf("llist_print_element. SNode is null pointer");
        return;
    }
}*/

//Imprime todos elementos da lista encadeada
void llist_print( const LList* list, EventHandler_Print print )
{
    SNode* curr;
    _LList* l = (_LList*)list;
    if( l == NULL){
        printf("llist_print. LList is null pointer");
        return;
    }
    if(l->count == 0){
        printf("Empty");
    }else{
        curr = l->head;
        while(curr != NULL){
            print(curr->value,NULL);
            curr = curr->next;
        }
        //llist_foreach((LList*)list,(EventHandler_Custom)print,NULL);
    }
    printf("\n");
}

//Função de comparearação ascendente
int llist_compare_asc(CPointer a, CPointer b){
    return a - b;
}

//Função de comparearação descendente
int llist_compare_des(CPointer a, CPointer b){
    return b - a;
}

bool llist_check_sorted(LList* list){
    _LList* l = (_LList*)list;
    _SNode* a = NULL;
    _SNode* c;
    if( l == NULL){
        printf("llist_check_sorted. LList is null pointer");
        return false;
    }
    if(l->compare == NULL){
        printf("llist_check_sorted. Compare handler is null pointer");
        return false;
    }else{
        a = l->head;
        c = a->next;
        while((c!=NULL && a!=NULL)){
            if(l->compare(a->value,c->value) > 0)
                return false;
            a = c;
            c = a->next;
        }
        return true;
    }
}

#endif
