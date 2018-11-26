#ifndef BaseLib_Collection_IHeap_h_IsIncluded
#define BaseLib_Collection_IHeap_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/Export.h"

namespace BaseLib { namespace Collection {

/* File heap.h - Abstract Base Class for Heaps
 * ----------------------------------------------------------------------------
 *  Shane Saunders
 */

/* --- Heap ---
 * This is an abstract base class from which specific heap classes can be
 * derived.  Different heaps derived from this abstract base class can be used
 * interchangeably by algorithms that were written using the universal
 * interface it provides.
 *
 * This heap stores integer items, and associates with each item a long integer
 * key.  Any derived heap heap must provide the following methods:
 *
 * deleteMin()    - removes the item with the minimum key from the heap, and
 *                  returns it.
 * insert()       - inserts an item 'item' with key 'key' into the heap.
 * decreaseKey()  - decreases the key of item 'item' to the new value newKey.
 * nItems()       - returns the number of items currently in the heap.
 * nComps()       - returns the number of key comparison operations.
 * dump()         - prints a text representation of the heap to the standard
 *                  output.
 */
class DLL_STATE Heap {
public:
    virtual ~Heap() { }
    virtual int deleteMin() = 0;
    virtual std::pair<int, long double> deleteMinimum() = 0;
    virtual void insert(int item, long double key) = 0;
    virtual void decreaseKey(int item, long double newKey) = 0;
    virtual long double getKey(int item) = 0;
    virtual int nItems() const = 0;
    virtual long nComps() const = 0;
    virtual void dump() const = 0;
};

class DLL_STATE HeapDesc {
public:
    virtual ~HeapDesc() { }
    virtual Heap *newInstance(int n) const = 0;
};

template <class T>
class HeapD: public HeapDesc {
public:
    Heap *newInstance(int n) const { return new T(n); }
};


/* --- FHeap ---
 * Fibonacci heap class.
 *
 * trees - An array of pointers to trees at root level in the heap.  Entry i
 *         in the array points to the root node of a tree that has nodes of
 *         dimension i on the main trunk.
 * nodes - An array of pointers to nodes in the heap.  Nodes are indexed
 *         according to their vertex number.  This array can then be used to
 *         look up the node for corresponding to a vertex number, and is
 *         useful when freeing space taken up by the heap.
 * maxNodes - The maximum number of nodes allowed in the heap.
 * maxTrees - The maximum number of trees allowed in the heap (calculated from
 *             maxNodes).
 * itemCount - The current number of nodes in the heap.
 * treeSum - The binary value represented by trees in the heap.
 *           By maintaining this it is easy to keep track of the maximum rank
 *           tree in the heap.
 * compCount - can be used for experimental purposes when counting the number
 *             of key comparisons.
 */
//template <typename ITEM, typename KEY>
class DLL_STATE FHeap : public Heap
{
public:
    FHeap(int n);
    ~FHeap();

private:
    /* --- FHeapNode ---
     * Fibonacci heap node class.
     *
     * A nodes has the following pointers:
     * parent      - a pointer to the nodes parent node (if any).
     * child       - a pointer to a child node (typically the highest rank child).
     * left, right - sibling pointers which provide a circular doubly linked list
     *               containing all the parents nodes children.
     *
     * The remaining fields are:
     * rank        - the nodes rank, that is, the number of children it has.
     * key         - the nodes key.
     * item        - the number of the item that the node is associated with.
     */
    class FHeapNode
    {
    public:
        FHeapNode()
            : parent(NULL)
            , left(NULL)
            , right(NULL)
            , child(NULL)
            , rank(0)
            , marked(0)
            , key(0)
            , item(0)
        {}

    public:
        FHeapNode *parent;
        FHeapNode *left, *right;
        FHeapNode *child;
        int rank;
        int marked;
        long double key;
        int item;
    };

public:
    int deleteMin();
    std::pair<int, long double> getMin();
    std::pair<int, long double> deleteMinimum();
    void insert(int item, long double k);
    void decreaseKey(int item, long double newValue);
    long double getKey(int item);
    int nItems() const { return itemCount; }

    long nComps() const { return compCount; }
    void dump() const;

private:
    FHeapNode **trees;
    FHeapNode **nodes;
    int maxNodes, maxTrees, itemCount, treeSum;
    long compCount;

    void meld(FHeapNode *treeList);
    static void dumpNodes(FHeapNode *node, int level);
};

/*--- FHeap (public methods) ------------------------------------------------*/

/* --- Constructor ---
 * Creates an FHeap object capable of holding up to $n$ items.
 */
FHeap::FHeap(int n)
{
    int i;
#if FHEAP_DUMP
    printf("new, ");
#endif
    maxTrees = 1 + (int)(1.44 * ((float)log((float)n))/((float)log((float)2.0)));
    maxNodes = n;

    trees = new FHeapNode*[maxTrees];
    for(i = 0; i < maxTrees; i++) trees[i] =0;

    nodes = new FHeapNode*[n];
    for(i = 0; i < n; i++) nodes[i] = 0;

    itemCount = 0;

    /* The $treeSum$ of the heap helps to keep track of the maximum rank while
     * nodes are inserted or deleted.
     */
    treeSum = 0;

    /* For experimental purposes, we keep a count of the number of key
     * comparisons.
     */
    compCount = 0;

#if FHEAP_DUMP
    printf("new-exited, ");
#endif
}

/* --- Destructor ---
 */
FHeap::~FHeap()
{
    int i;

#if FHEAP_DUMP
    printf("delete, ");
#endif

    for(i = 0; i < maxNodes; i++) delete nodes[i];
    delete [] nodes;
    delete [] trees;

#if FHEAP_DUMP
    printf("delete-exited, ");
#endif
}

/* --- insert() ---
 * Inserts an item $item$ with associated key $k$ into the heap.
 */
void FHeap::insert(int item, long double k)
{
    FHeapNode *newNode;

#if FHEAP_DUMP
    printf("insert, ");
#endif

    /* create an initialise the new node */
    newNode = new FHeapNode;
    newNode->child = NULL;
    newNode->left = newNode->right = newNode;
    newNode->rank = 0;
    newNode->item = item;
    newNode->key = k;

    /* maintain a pointer to $item$'s new node in the heap */
    nodes[item] = newNode;

    /* meld the new node into the heap */
    meld(newNode);

    /* update the heaps node count */
    itemCount++;

#if FHEAP_DUMP
    printf("insert-exited, ");
#endif
}

/* --- getMin() ---
 * returns the minimum item from the heap.
 */
std::pair<int, long double> FHeap::getMin()
{
    FHeapNode *minNode, *child, *next;
    long double k, k2;
    int r, v, item;

    /* First we determine the maximum rank in the heap. */
    v = treeSum;
    r = -1;
    while(v) {
        v = v >> 1;
        r++;
    };

    /* Now determine which root node is the minimum. */
    minNode = trees[r];
    k = minNode->key;
    while(r > 0) {
        r--;
        next = trees[r];
        if(next) {
            if((k2 = next->key) < k) {
                k = k2;
                minNode = next;
            }
            compCount++;
        }
    }

    /* Record the vertex no of the old minimum node before deleting it. */
    //item = minNode->item;
    //nodes[item] = NULL;
    //delete minNode;
    //itemCount--;

    return std::pair<int, long double>(minNode->item, minNode->key);
}

std::pair<int, long double> FHeap::deleteMinimum()
{
    FHeapNode *minNode, *child, *next;
    long double k, k2, minkey;
    int r, v, item;

#if FHEAP_DUMP
    printf("deleteMin, ");
#endif

    /* First we determine the maximum rank in the heap. */
    v = treeSum;
    r = -1;
    while(v) {
        v = v >> 1;
        r++;
    };

    /* Now determine which root node is the minimum. */
    minNode = trees[r];
    k = minNode->key;
    while(r > 0) {
        r--;
        next = trees[r];
        if(next) {
            if((k2 = next->key) < k) {
                k = k2;
                minNode = next;
            }
            compCount++;
        }
    }

    /* We remove the minimum node from the heap but keep a pointer to it. */
    r = minNode->rank;
    trees[r] = NULL;
    treeSum -= (1 << r);

    child = minNode->child;
    if(child) meld(child);

    /* Record the vertex no of the old minimum node before deleting it. */
    item = minNode->item;
    minkey = minNode->key;
    nodes[item] = NULL;
    delete minNode;
    itemCount--;

#if FHEAP_DUMP
    printf("deleteMin-exited, ");
#endif

    return std::pair<int, long double>(item, minkey);
}

long double FHeap::getKey(int item)
{
    FHeapNode *cutNode;
    cutNode = nodes[item];
    long double val = cutNode->key;

    return val;
}
/* --- deleteMin() ---
 * Deletes and returns the minimum item from the heap.
 */
int FHeap::deleteMin()
{
    FHeapNode *minNode, *child, *next;
    long double k, k2;
    int r, v, item;

#if FHEAP_DUMP
    printf("deleteMin, ");
#endif

    /* First we determine the maximum rank in the heap. */
    v = treeSum;
    r = -1;
    while(v) {
        v = v >> 1;
        r++;
    };

    /* Now determine which root node is the minimum. */
    minNode = trees[r];
    k = minNode->key;
    while(r > 0) {
        r--;
        next = trees[r];
        if(next) {
            if((k2 = next->key) < k) {
                k = k2;
                minNode = next;
            }
            compCount++;
        }
    }

    /* We remove the minimum node from the heap but keep a pointer to it. */
    r = minNode->rank;
    trees[r] = NULL;
    treeSum -= (1 << r);

    child = minNode->child;
    if(child) meld(child);

    /* Record the vertex no of the old minimum node before deleting it. */
    item = minNode->item;
    nodes[item] = NULL;
    delete minNode;
    itemCount--;

#if FHEAP_DUMP
    printf("deleteMin-exited, ");
#endif

    return item;
}



/* --- decreaseKey() ---
 * Decreases the key used for item $item$ to the value newValue.  It is left
 * for the user to ensure that newValue is in-fact less than the current value
 */
void FHeap::decreaseKey(int item, long double newValue)
{
    FHeapNode *cutNode, *parent, *newRoots, *r, *l;
    int prevRank;

#if FHEAP_DUMP
    printf("decreaseKey on vn = %d, ", item);
#endif

    /* Obtain a pointer to the decreased node and its parent then decrease the
     * nodes key.
     */
    cutNode = nodes[item];
    parent = cutNode->parent;
    cutNode->key = newValue;

    /* No reinsertion occurs if the node changed was a root. */
    if(!parent) {
#if FHEAP_DUMP
        printf("decreaseKey-exited, ");
#endif
        return;
    }

    /* Update the left and right pointers of cutNode and its two neighbouring
     * nodes.
     */
    l = cutNode->left;
    r = cutNode->right;
    l->right = r;
    r->left = l;
    cutNode->left = cutNode->right = cutNode;

    /* Initially the list of new roots contains only one node. */
    newRoots = cutNode;

    /* While there is a parent node that is marked a cascading cut occurs. */
    while(parent && parent->marked) {

        /* Decrease the rank of cutNode's parent and update its child pointer.
         */
        parent->rank--;
        if(parent->rank) {
            if(parent->child == cutNode) parent->child = r;
        }
        else {
            parent->child = NULL;
        }

        /* Update the cutNode and parent pointers to the parent. */
        cutNode = parent;
        parent = cutNode->parent;

        /* Update the left and right pointers of cutNodes two neighbouring
         * nodes.
         */
        l = cutNode->left;
        r = cutNode->right;
        l->right = r;
        r->left = l;

        /* Add cutNode to the list of nodes to be reinserted as new roots. */
        l = newRoots->left;
        newRoots->left = l->right = cutNode;
        cutNode->left = l;
        cutNode->right = newRoots;
        newRoots = cutNode;
    }

    /* If the root node is being relocated then update the trees[] array.
     * Otherwise mark the parent of the last node cut.
     */
    if(!parent) {
        prevRank = cutNode->rank + 1;
        trees[prevRank] = NULL;
        treeSum -= (1 << prevRank);
    }
    else {
        /* Decrease the rank of cutNode's parent an update its child pointer.
         */
        parent->rank--;
        if(parent->rank) {
            if(parent->child == cutNode) parent->child = r;
        }
        else {
            parent->child = NULL;
        }

        parent->marked = 1;
    }

    /* Meld the new roots into the heap. */
    meld(newRoots);

#if FHEAP_DUMP
    printf("decreaseKey-exited, ");
#endif
}

/*--- FHeap (private methods) -----------------------------------------------*/

/* --- meld() ---
 * melds the linked list of trees pointed to by $treeList$ into the heap.
 */
void FHeap::meld(FHeapNode *treeList)
{
    FHeapNode *first, *next, *nodePtr, *newRoot, *temp, *temp2, *lc, *rc;
    int r;

#if FHEAP_DUMP
    printf("meld: ");
#endif

    /* We meld each tree in the circularly linked list back into the root level
     * of the heap.  Each node in the linked list is the root node of a tree.
     * The circularly linked list uses the sibling pointers of nodes.  This
     *  makes melding of the child nodes from a deleteMin operation simple.
     */
    nodePtr = first = treeList;

    do {

#if FHEAP_DUMP
        printf("%d, ", nodePtr->item);
#endif

        /* Keep a pointer to the next node and remove sibling and parent links
         * from the current node.  nodePtr points to the current node.
         */
        next = nodePtr->right;
        nodePtr->right = nodePtr->left = nodePtr;
        nodePtr->parent = NULL;

        /* We merge the current node, nodePtr, by inserting it into the
         * root level of the heap.
         */
        newRoot = nodePtr;
        r = nodePtr->rank;

        /* This loop inserts the new root into the heap, possibly restructuring
         * the heap to ensure that only one tree for each degree exists.
         */
        do {

            /* Check if there is already a tree of degree r in the heap.
             * If there is then we need to link it with newRoot so it will be
             * reinserted into a new place in the heap.
             */
            if((temp = trees[r])) {

                /* temp will be linked to newRoot and relocated so we no
                 * longer will have a tree of degree r.
                 */
                trees[r] = NULL;
                treeSum -= (1 << r);

                /* Swap temp and newRoot if necessary so that newRoot always
                 * points to the root node which has the smaller key of the
                 * two.
                 */
                if(temp->key < newRoot->key) {
                    temp2 = newRoot;
                    newRoot = temp;
                    temp = temp2;
                }
                compCount++;

                /* Link temp with newRoot, making sure that sibling pointers
                 * get updated if rank is greater than 0.  Also, increase r for
                 * the next pass through the loop since the rank of new has
                 * increased.
                 */
                if(r++ > 0) {
                    rc = newRoot->child;
                    lc = rc->left;
                    temp->left = lc;
                    temp->right = rc;
                    lc->right = rc->left = temp;
                }
                newRoot->child = temp;
                newRoot->rank = r;
                temp->parent = newRoot;
                temp->marked = 0;
            }
            /* Otherwise if there is not a tree of degree r in the heap we
             * allow newRoot, which possibly carries moved trees in the heap,
             * to be a tree of degree r in the heap.
             */
            else {

                trees[r] = newRoot;
                treeSum += (1 << r);;

                /* NOTE:  Because newRoot is now a root we ensure it is
                 *        marked.
                 */
                newRoot->marked = 1;
            }

            /* Note that temp will be NULL if and only if there was not a tree
             * of degree r.
             */
        } while(temp);

        nodePtr = next;

    } while(nodePtr != first);

#if FHEAP_DUMP
    printf("meld-exited, ");
#endif
}

/*--- FHeap (debugging) -----------------------------------------------------*/

/* --- dumpNodes() ---
 * Recursively print a text representation of the node subtree starting at the
 * node poined to by $node$, using an indentationlevel of $level$.
 */
void FHeap::dumpNodes(FHeapNode *node, int level)
{
#if FHEAP_DUMP
    FHeapNode *childNode, *partner;
    int i, childCount;

    /* Print leading whitespace for this level. */
    for(i = 0; i < level; i++) printf("   ");

    //printf("%d(%ld)[%d]\n", node->item, node->key, node->rank);
    std::cout << node->item << "(" << node->key << ")[" << node->rank << "]" << std::endl;

    if((childNode = node->child)) {
        childNode = node->child->right;

        childCount = 0;

        do {
            dumpNodes(childNode, level+1);
            if(childNode->rank > node->rank) {
                for(i = 0; i < level+1; i++) printf("   ");
                printf("error(dim)\n");  exit(1);
            }
            if(childNode->parent != node) {
                for(i = 0; i < level+1; i++) printf("   ");
                printf("error(parent)\n");
            }
            childNode = childNode->right;
            childCount++;
        } while(childNode != node->child->right);

        if(childCount != node->rank) {
            for(i = 0; i < level; i++) printf("   ");
            printf("error(childCount)\n");  exit(1);
        }
    }
    else {
        if(node->rank != 0) {
            for(i = 0; i < level; i++) printf("   ");
            printf("error(dim)\n"); exit(1);
        }
    }
#endif
}

/* --- dump() ---
 * Print a text representation of the heap to the standard output.
 */
void FHeap::dump() const
{
#if FHEAP_DUMP
    int i;
    FHeapNode *node;

    printf("\n");
    printf("treeSum = %d\n", treeSum);
    printf("array entries 0..maxTrees =");
    for(i = 0; i < maxTrees; i++) {
        printf(" %d", trees[i] ? 1 : 0 );
    }
    printf("\n\n");
    for(i = 0; i < maxTrees; i++) {
        if((node = trees[i])) {
            printf("tree %d\n\n", i);
            dumpNodes(node, 0);
            printf("\n");
        }
    }
    fflush(stdout);
#endif
}


}}

#endif // BaseLib_Collection_IHeap_h_IsIncluded
