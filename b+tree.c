#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#define order 4 
//4 values
// therefore, we have the capacity to store 6 pointers and 5 values, but we will store only 4 data values and 5 pointers

typedef union pointerType_tag
{
    struct bPlusNode *nodeptr[order + 2];
    struct flightNode_tag *dataptr[order + 2];
} pointerType;

typedef struct Time
{
    int Hour;
    int Min;
} Time;

typedef struct Flight_tag
{
    int flightID;
    Time departureTime;
    Time ETA;
    struct Flight_tag *next;
    struct Flight_tag *prev;
} Flight;

typedef struct bPlusNode
{
    Time key[order+1];
    int isLeaf;
    pointerType children;
    int activeKeys;
    struct bPlusNode *parent;
} BPlusTreeNode;

typedef struct flightNode_tag
{
    Flight *lptr;
    struct flightNode_tag *prev;
    struct flightNode_tag *next;
    int size;
} DataNode;

typedef struct Bucket_tag
{
    struct Bucket_tag *next;
    Time beginningETA;
    Time endETA;
    BPlusTreeNode *root;
    int isLeaf;
} Bucket;

int maxTime(Time t1, Time t2) // ret_val=1 indicates t1 < t2 and 0 indicates t1 = t2 and -1 indicates t1 > t2
{
    int ret_val = 1;
    if (t1.Hour > t2.Hour)
        ret_val = -1;
    else if (t1.Hour < t2.Hour)
        ret_val = 1;
    else // same hour
    {
        if (t1.Min > t2.Min)
            ret_val = -1;
        else if (t1.Min < t2.Min)
            ret_val = 1;
        else // same min
        {
            ret_val = 0;
        }
    }
    return ret_val;
}

Time TimeDiff(Time A, Time B)
{
    Time temp;

    if (A.Hour > B.Hour)
    {
        temp.Hour = A.Hour - B.Hour;
        temp.Min = A.Min - B.Min;

        if (temp.Min < 0)
        {
            temp.Min = temp.Min + 60;
            temp.Hour--;
        }
    }

    else if (A.Hour == B.Hour)
    {
        temp.Hour = 0;
        temp.Min = A.Min - B.Min;
        if (temp.Min < 0)
        {
            temp = TimeDiff(B,A);
        }
    }

    else
        temp = TimeDiff(B, A);

    return temp;
}

Flight *insertInPlaneList(Flight *head, Flight *newNode) // inserting in ascending order
{
    if (head == NULL)
        head = newNode;

    else if (maxTime(head->departureTime, newNode->departureTime) == -1) // Insert at start
    {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }

    else // General Case
    {
        Flight *ptr, *prev_ptr;
        ptr = head;

        while (ptr != NULL && maxTime(ptr->departureTime, newNode->departureTime) >= 0)
        {
            prev_ptr = ptr;
            ptr = ptr->next;
        }

        if (ptr == NULL) // Insert at end
        {
            prev_ptr->next = newNode;
            newNode->prev = prev_ptr;
            newNode->next = NULL;
        }

        else
        {
            newNode->next = ptr;
            ptr->prev = newNode;
            prev_ptr->next = newNode;
            newNode->prev = prev_ptr;
        }
    }

    return head;
}

void visit(Flight *ptr)
{
    while (ptr != NULL)
    {
        printf("\nFlight ID is %d\n", ptr->flightID);
        printf("Flight Departure = %d:%d\n", ptr->departureTime.Hour, ptr->departureTime.Min);
        ptr = ptr->next;
    }
}

Flight *createNode(int ID, Time departureTime, Time ETA)
{
    Flight *nptr;
    nptr = (Flight *)malloc(sizeof(Flight));

    nptr->flightID = ID;
    nptr->next = NULL;
    nptr->prev = NULL;

    nptr->departureTime.Hour = departureTime.Hour;
    nptr->departureTime.Min = departureTime.Min;

    nptr->ETA.Hour = ETA.Hour;
    nptr->ETA.Min = ETA.Min;

    return nptr;
}

DataNode *createDataNode()
{
    DataNode *newNode;
    newNode = (DataNode *)malloc(sizeof(DataNode));
    newNode->lptr = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->size = 0;
    return newNode;
}

BPlusTreeNode *createTreeNode()
{
    BPlusTreeNode *newNode; // new BPlusTreeNode
    newNode = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    newNode->activeKeys = 0;
    newNode->isLeaf = 0;
    for (int i = 0; i < order; i++)
    {
        newNode->children.dataptr[i] = NULL;
    }
    return newNode;
}

void printTree(BPlusTreeNode *root)
{
    int i = 0;
    printf ("Keys\n");
    while (i < root->activeKeys)
    {
        printf ("%d:%d ", root->key[i].Hour, root->key[i].Min);
        i++;
    }
    printf ("\n");
    if (root->isLeaf == 1)
    {
        printf ("dataValues\n");
        DataNode *data = root->children.dataptr[0];
        while (data != NULL)
        {
            Flight *pointer = data->lptr;
            while (pointer != NULL)
            {
                printf ("%d:%d ", pointer->departureTime.Hour, pointer->departureTime.Min);
                pointer = pointer->next;
            }
            printf ("\n");
            data = data->next;
        }
        printf ("\n");
    }
    else
    {
        printf ("recusrion");
        i = 0;
        printf ("%d", root->activeKeys);
        while (i <= root->activeKeys)
        {
            if (root->children.nodeptr[i] != NULL) printTree(root->children.nodeptr[i]);
            i++;
        }
    }
}

BPlusTreeNode *splitBPlusTreeNode(BPlusTreeNode *root)
{
    printf("split BPlusTreeNode\n");
    int i;
    BPlusTreeNode *newNode = createTreeNode();
    newNode->activeKeys = order / 2;

    if (root->parent == NULL)
    {
        printf ("splitting root");
        for (i = order / 2 + 1; i <= order; i++)
        {
            newNode->children.dataptr[i - order / 2 - 1] = root->children.dataptr[i];
            newNode->key[i - order / 2 - 1] = root->key[i];
            root->children.dataptr[i] = NULL;
        }
        printf("%d", i);
        newNode->isLeaf = 1;
        newNode->children.dataptr[i - order / 2 - 1] = root->children.dataptr[i];

        newNode->activeKeys = root->activeKeys = order / 2;
        root->children.dataptr[order / 2]->next = newNode->children.dataptr[0];
        newNode->children.dataptr[0]->prev = root->children.dataptr[order / 2];

        BPlusTreeNode *newRoot = createTreeNode();
        newNode->parent = root->parent = newRoot;
        newRoot->key[0] = root->key[order / 2];
        // printf ("%d:%d ", newRoot->key[0].Hour, newRoot->key[0].Min);
        newRoot->isLeaf = 0;
        newRoot->children.nodeptr[0] = root;
        newRoot->children.nodeptr[1] = newNode;
        newRoot->parent = NULL;
        newRoot->activeKeys = 1;
        printTree(newRoot);
    }

    else if (root->isLeaf == 1)
    {
        printf ("splitting leaf");
        for (i = order / 2 + 1; i <= order; i++)
        {
            newNode->children.dataptr[i - order / 2 - 1] = root->children.dataptr[i];
            newNode->key[i - order / 2 - 1] = root->key[i];
            root->children.dataptr[i] = NULL;
        }
        printf("%d", i);
        newNode->isLeaf = 1;
        newNode->children.dataptr[i - order / 2 - 1] = root->children.dataptr[i];

        newNode->activeKeys = root->activeKeys = order / 2;
        root->children.dataptr[order / 2]->next = newNode->children.dataptr[0];
        newNode->children.dataptr[0]->prev = root->children.dataptr[order / 2];

        BPlusTreeNode *parent = root->parent;
        printf (" %d", parent->activeKeys);
        i = parent->activeKeys - 1;
        printf ("%d", parent->activeKeys);
        while (i >= 0 && maxTime(parent->key[i], root->key[0]) < 0)
        {
            parent->key[i + 1] = parent->key[i];
            parent->children.nodeptr[i + 2] = parent->children.nodeptr[i + 1];
            i--;
        }
        i++;
        printf("%d", i);
        parent->key[i] = root->key[order / 2];
        parent->children.nodeptr[i] = root;
        parent->children.nodeptr[i + 1] = newNode;
        parent->activeKeys++;
        printf("\n");
        for (i = 0; i < parent->activeKeys; i++)
        {
            printf("%d:%d ", parent->key[i].Hour, parent->key[i].Min);
        }
        printf("\n");
        for (i = 0; i <= parent->activeKeys; i++)
        {
            if (parent->children.nodeptr[i] != NULL)
            {
                BPlusTreeNode *traversalnode = parent->children.nodeptr[i];
                for (int j = 0; j < traversalnode->activeKeys; j++)
                {
                    printf("%d:%d ", traversalnode->key[j]);
                }
                for (int j = 0; j <= traversalnode->activeKeys; j++)
                {
                    printf("Now print respective dataNodes: \n");
                    DataNode *dataNodeTraversal = traversalnode->children.dataptr[j];
                    if (dataNodeTraversal != NULL)
                    {
                        printf("flighttraversal\n");
                        Flight *flightTraversal = dataNodeTraversal->lptr;
                        while (flightTraversal != NULL)
                        {
                            printf("%d:%d ", flightTraversal->departureTime.Hour, flightTraversal->departureTime.Min);
                            flightTraversal = flightTraversal->next;
                        }
                        printf("\n");
                    }
                }
                printf("\n");
            }
        }
    }
    else
    {
        BPlusTreeNode *newNode = createTreeNode();
        newNode->activeKeys = order/2;
        newNode->isLeaf = 1;
        newNode->parent = root->parent;
        BPlusTreeNode *parent = root->parent;

        for (i = order/2 + 1; i <= order; i++)
        {
            newNode->children.nodeptr[i-order/2 -1] = root->children.nodeptr[i];
            newNode->key[i-order/2 -1] = root->key[i];
            root->children.nodeptr[i] = NULL;
        }
        newNode->children.nodeptr[i-order/2-1] = root->children.nodeptr[i];

        while (i >= 0 && maxTime(parent->key[i], root->key[0]) < 0)
        {
            parent->key[i+1] = parent->key[i];
            parent->children.nodeptr[i+2] = parent->children.nodeptr[i+1];
            i--;
        }

        i++;
        printf ("%d", i);
        parent->key[i] = root->key[order/2];
        parent->children.nodeptr[i] = root;
        parent->children.nodeptr[i+1] = newNode;

    }
    if (root->parent->activeKeys == order + 1)
    {
        printf("here comes the recursion");
        root->parent = splitBPlusTreeNode(root->parent);
    }
    return root->parent;
}

BPlusTreeNode *splitDataNode(BPlusTreeNode *root, int dataNodeIndex)//flight to be inserted
{

    DataNode *temp = root->children.dataptr[0];
    Flight *ptr = NULL;

    printf ("Split");
    printf ("%d*%d*", root->children.dataptr[dataNodeIndex]->size, dataNodeIndex);
    Flight *flightPtr = root->children.dataptr[dataNodeIndex]->lptr;
    for (int j = 0; j < root->activeKeys; j++)
    {
        printf ("%d:%d ", root->key[j].Hour, root->key[j].Min);
    }
    printf ("\n");

    flightPtr = root->children.dataptr[dataNodeIndex]->lptr;
    for (int j = 0; j < order/2; j++)
    {
        flightPtr = flightPtr->next;
    }
    
    Flight *child = flightPtr;
    
    int i = root->activeKeys-1;

    //insert key in the BPlusTreeNode
    while (i >= 0 && maxTime(root->key[i], child->departureTime) < 0)
    {
        root->key[i+1] = root->key[i];
        root->children.dataptr[i+2] = root->children.dataptr[i+1]; 
        i--;
    }
    i++;
    root->key[i] = child->departureTime;
    root->activeKeys++;
    for (int j = 0; j < root->activeKeys; j++)
    {
        printf ("%d:%d ", root->key[j].Hour, root->key[j].Min);
    }
    printf("\n");

    //now split the dataNode
    DataNode *newDataNode = createDataNode();
    newDataNode->size = 2;
    int j = 0;
    ptr = root->children.dataptr[dataNodeIndex]->lptr;
    Flight *newPtr; //to traverse newDataNode
    while (j < order/2)
    {
        ptr = ptr->next;
        j++;
    }

    Flight *prev = ptr;
    ptr = ptr->next;
    prev->next = NULL;
    root->children.dataptr[dataNodeIndex]->size = 3;

    newPtr = newDataNode->lptr = ptr;
    
    while (ptr != NULL)
    {
       newPtr->next = ptr->next;
       ptr = ptr->next;
       newPtr = newPtr->next;
    }

    newDataNode->next = root->children.dataptr[dataNodeIndex]->next;
    if (newDataNode->next != NULL)
    {
        DataNode *temp = newDataNode->next;
        temp->prev = newDataNode; 
    }
    root->children.dataptr[dataNodeIndex]->next = newDataNode;
    newDataNode->prev = root->children.dataptr[dataNodeIndex];
 
    printf ("%d",root->children.dataptr[i+1]==NULL );
    root->children.dataptr[i+1] = newDataNode;

    while (temp != NULL)
    {
        ptr = temp->lptr;
        printf ("size: %d  \n",temp->size );
        while (ptr != NULL)
        {
            printf ("%d:%d ", ptr->departureTime.Hour, ptr->departureTime.Min);
            ptr = ptr->next;
        }
        temp = temp->next;
        printf ("\n");
    }

    if (root->activeKeys == order+1)
    {
        root = splitBPlusTreeNode(root);
    }
    return root;
}

BPlusTreeNode *insertInbPlusTree(BPlusTreeNode *root, Flight *newNode)
{
    if (root == NULL)
    {
        root = createTreeNode();
        root->isLeaf = 1;
        root->activeKeys = 1;
        for (int i = 0; i <= order+1; i++)
        {
            root->children.dataptr[i] = NULL;
        }
        root->children.dataptr[0] = createDataNode();
        root->children.dataptr[0]->lptr = insertInPlaneList(root->children.dataptr[0]->lptr, newNode);
        root->children.dataptr[0]->size = 1;
        root->parent = NULL;
        root->key[0] = newNode->departureTime;
    }

    else if (root->isLeaf == 1)
    {
        int i = 0;

        // Finding appropriate position
        while (i < root->activeKeys && maxTime(newNode->departureTime, root->key[i]) < 0)
            i++;

        if (root->children.dataptr[i] == NULL)
        {
            root->children.dataptr[i] = createDataNode();
            root->children.dataptr[i-1]->next = root->children.dataptr[i];
            root->children.dataptr[i]->prev = root->children.dataptr[i-1];
        }

        root->children.dataptr[i]->lptr = insertInPlaneList(root->children.dataptr[i]->lptr, newNode);
    
        Flight *ptr = root->children.dataptr[i]->lptr;
        DataNode *temp = root->children.dataptr[0];
        root->children.dataptr[i]->size++;

        if (root->children.dataptr[i]->size == order+1 ) // DataNode is FULL
        {
            printf("Splitting\n");
            root = splitDataNode (root,i);
            printf ("returned");
            // Split Data BPlusTreeNode
        }
        while (temp != NULL)
        {
            ptr = temp->lptr;
            printf ("size: %d  \n",temp->size );
            while (ptr != NULL)
            {
                printf ("%d:%d ", ptr->departureTime.Hour, ptr->departureTime.Min);
                ptr = ptr->next;
            }
            temp = temp->next;
            printf ("\n");
        }
        printf ("******************Printing the tree*********************");
        printTree (root);
    }

    else
    {
        int i = 0;
        while (i < root->activeKeys && maxTime(newNode->departureTime, root->key[i]) == (-1))
            i++;

        if (i == root->activeKeys)
            root->children.nodeptr[i] = insertInbPlusTree(root->children.nodeptr[i], newNode);
    }
    return root;
}

BPlusTreeNode *mergeNodes(BPlusTreeNode* iNode, BPlusTreeNode* jNode)
{
    printf("\nInside mergeNodes\n");     
    
    //Merging i and j Node to form a combined i Node
    //Merging the leaf Nodes
    
    if(iNode->isLeaf==1)
    {            
        int k=0, index=0;   
        // k is iterator and index is used to fill entries in temp 
        //Copying nodes of iNode to temp so as to compact them
        
        printf("From i : ");
        while(k < iNode->activeKeys)
        {
            if(iNode->children.dataptr[k]!=NULL)
            {
                iNode->children.dataptr[index] = iNode->children.dataptr[k];
                printf("%d ",iNode->children.dataptr[index]->size);
                index++;
            }
            k++;
        }

        k=0;
        index--;

        //Adding nodes of jNode to temp
        printf("\nFrom j : ");
        while(k < jNode->activeKeys)
        {
            if(jNode->children.dataptr[k]!=NULL)
            {
                if(k==0)
                {
                    iNode->children.dataptr[index]->lptr->next = jNode->children.dataptr[k]->lptr;
                    iNode->children.dataptr[index]->size = iNode->children.dataptr[index]->size + jNode->children.dataptr[k]->size;
                    if(iNode->children.dataptr[index]->size > order)
                    {
                        Flight *firstFlight;
                        firstFlight = iNode->children.dataptr[index]->lptr;
                        iNode->children.dataptr[index]->lptr = iNode->children.dataptr[index]->lptr->next;
                        firstFlight->next = NULL;

                        iNode->children.dataptr[index-1]->lptr->next = firstFlight;
                    }
                }
                else
                {
                    iNode->children.dataptr[index] = jNode->children.dataptr[k];
                }
                printf("%d=%d ",iNode->children.dataptr[index]->size,jNode->children.dataptr[k]->size);
                index++;
            }
            k++;
        }
        iNode->activeKeys = index - 1;
        printf("\nTemp is ready \n");
        
        //Updating keys of temp
        k=0;
        Time newKey;

        while(k < iNode->activeKeys && iNode->children.dataptr[k]!=NULL && iNode->children.dataptr[k]->lptr!=NULL)
        {
            printf("%d",iNode->children.dataptr[k]->size);
            printf(" %d ",k);            
            
            if(iNode->children.dataptr[k]->lptr->next==NULL)
            {
                newKey = iNode->children.dataptr[k]->lptr->departureTime;
                printf("If cleared\n");
            }
            
            else
            {
                printf("Else ");
                Flight* lastFlight;
                lastFlight = iNode->children.dataptr[k]->lptr->next;
                printf("Inside else");    
                while(lastFlight->next!=NULL)
                {
                    lastFlight = lastFlight->next;
                    printf(" Inside while");
                }

                newKey = lastFlight->departureTime;
                printf(" else passed");
            }
            printf(" %d key updated\n",k);
            iNode->key[k] = newKey;
            k++;
        }
        printf("Keys updated\n");

        if(iNode->parent==jNode->parent)
        {
            iNode->parent->activeKeys--;
            if(iNode->parent->activeKeys==1)
            {
                printf("Need to merge the parents");
                //iNode->parent = mergeParents();
            }
        }
    }    
    free(jNode);
    
    printf("Merge successful!!\n");
    return iNode;
}

BPlusTreeNode *mergeParents(BPlusTreeNode* iNode, BPlusTreeNode* jNode)
{        
    //Merging i and j Node to form a combined i Node
    //Merging the parent Nodes
    
    if(iNode->isLeaf==1)
    {            
        int k=0, index=0;   
        // k is iterator and index is used to fill entries in iNode 
        //Compacting nodes of iNode
        
        while(k < iNode->activeKeys)
        {
            if(iNode->children.nodeptr[k]!=NULL)
            {
                iNode->children.nodeptr[index] = iNode->children.nodeptr[k];
                index++;
            }
            k++;
        }

        k=0;
        index--;

        //Adding nodes of jNode
        while(k < jNode->activeKeys)
        {
            if(jNode->children.nodeptr[k]!=NULL)
            {
                if(k==0)
                    iNode->children.nodeptr[index] = mergeNodes(iNode->children.nodeptr[index],jNode->children.nodeptr[k]);
                
                else
                    iNode->children.nodeptr[index] = jNode->children.nodeptr[k];
                index++;
            }
            k++;
        }

        iNode->activeKeys = index - 1;
        
        
        //Updating keys of iNode
        k=0;
        Time newKey;

        while(k < iNode->activeKeys && iNode->children.nodeptr[k]!=NULL)
        {
            newKey = iNode->children.nodeptr[k]->key[iNode->activeKeys - 1];
            iNode->key[k] = newKey;
        }

        if(iNode->parent==jNode->parent)
        {
            iNode->parent->activeKeys--;
            if(iNode->parent->activeKeys==1)
            {
                printf("Need to merge the parents");
                // iNode->parent = mergeParents();
            
                if(iNode->parent!=NULL && iNode->parent->activeKeys >= 2)
                {
                    int i = 0;
                    while(i < iNode->parent->activeKeys && iNode->parent->children.nodeptr[i]!=iNode)
                        i++;

                    if(i < iNode->parent->activeKeys)
                    {
                        //merge with right 
                        int j = i+1;
            
                        BPlusTreeNode *iPNode, *jPNode;
                        iPNode = iNode->parent->children.nodeptr[i];
                        jPNode = iNode->parent->children.nodeptr[j];
                        printf(" Merging with right neighbour ");
                        int k = 0;
                        printf("\nFrom j : ");
                        while(k < jNode->activeKeys)
                        {
                            printf("%d ",jNode->children.dataptr[k]->size);
                            k++;
                        }        
                        iNode = mergeNodes(iPNode,jPNode);
                    }
                    else if(i>0)
                    {
                        //merge with left neighbour
                        int j = i-1;

                        BPlusTreeNode *iPNode, *jPNode;
                        iPNode = iNode->parent->children.nodeptr[i];
                        jPNode = iNode->parent->children.nodeptr[j];
                        printf(" Merging with Left neighbour ");
                        iNode = mergeNodes(jPNode,iPNode); 
                    }
                }
            }    
        }
    }    
    free(jNode);
    
    printf("Parent Merge successful!!\n");
    return iNode;
}

void BorrowFromSibling(BPlusTreeNode *root, int i)
{
    int found=0;
    int j = i-1;
    //checking left sibling
    
    if(root->children.dataptr[j]!=NULL && root->children.dataptr[j]->size > 1)
    {
        //Sibling found
        printf("Left Sibling found = %d \n", root->children.dataptr[j]->lptr->flightID);
        found = 1;
    }

    if(!found) // Checking Right Siblings
    {
        j = i+1;
        
        if(root->children.dataptr[j]!=NULL && root->children.dataptr[j]->size > 1)
        {
            //Sibling found
            printf("Right Sibling found = %d\n", root->children.dataptr[j]->lptr->flightID);
            found = 1;
        }
    }

    if(found)
    {
        Flight *borrowedFlight,*prev;
        borrowedFlight = root->children.dataptr[j]->lptr->next;
        prev = root->children.dataptr[j]->lptr;

        while(borrowedFlight->next!=NULL)
        {
            prev = borrowedFlight;
            borrowedFlight = borrowedFlight->next;
        }
        
        prev->next = NULL;
        borrowedFlight->prev = NULL;
        root->children.dataptr[j]->size--;
        root->children.dataptr[i]->lptr = borrowedFlight;
        root->children.dataptr[i]->size = 1;
        printf("Borrowed Successfully ");
        
        //updating key
        root->key[(j+1)/2] = borrowedFlight->departureTime;
        if(j < i)   //borrow from left so drop a minute in key
        {
            root->key[(j+1)/2].Min--;
            if(root->key[(j+1)/2].Min < 0)
            {
                root->key[(j+1)/2].Min = root->key[(j+1)/2].Min + 60;
                root->key[(j+1)/2].Hour--;
                if(root->key[(j+1)/2].Hour < 0)
                    root->key[(j+1)/2].Hour = root->key[(j+1)/2].Hour + 24;
            }
        }
    }

    else //Delete the node
    {
        if(root->children.dataptr[i]->next != NULL) root->children.dataptr[i]->next->prev = root->children.dataptr[i]->prev;
        if(root->children.dataptr[i]->prev != NULL) root->children.dataptr[i]->prev->next = root->children.dataptr[i]->next; 
        
        free(root->children.dataptr[i]);
        root->children.dataptr[i]=NULL;
    }            
}

BPlusTreeNode *deletionInbplusTree(BPlusTreeNode *root, Flight *node)
{
    if(root!=NULL)
    {
        if(root->isLeaf==1)
        {
            //Finding the suitable dataNode
            int i = 0;
            while (i < root->activeKeys && maxTime(node->departureTime, root->key[i]) == -1) //increment till flight time greater than key
                i++;

            Flight* dataFlights;
            dataFlights = root->children.dataptr[i]->lptr;

            //Deleting the flight
            while(dataFlights!=NULL && dataFlights->flightID!=node->flightID)
                dataFlights = dataFlights->next;

            if(dataFlights==NULL) printf("\nNo such Flight exists\n");
            else    
            {
                //Normal Deletion
                if(dataFlights->prev!=NULL) dataFlights->prev->next = dataFlights->next;
                if(dataFlights->next!=NULL) dataFlights->next->prev = dataFlights->prev;
                
                //If first flight of a dataNode is deleted
                if(root->children.dataptr[i]->lptr==dataFlights) root->children.dataptr[i]->lptr=dataFlights->next; 
                printf("\nFlight CANCELLED\n");
                free(dataFlights);
                root->children.dataptr[i]->size--;

                //If dataNode becomes empty
                if(root->children.dataptr[i]->size==0)
                {
                    printf("DataNode is empty\n");

                    // if(root->children.dataptr[i]->next != NULL) root->children.dataptr[i]->next->prev = root->children.dataptr[i]->prev;
                    // if(root->children.dataptr[i]->prev != NULL) root->children.dataptr[i]->prev->next = root->children.dataptr[i]->next; 
                    // free(root->children.dataptr[i]);
                    // root->children.dataptr[i]=NULL;

                    BorrowFromSibling(root,i); 
                    
                    if (root->children.dataptr[i]==NULL)
                    {
                        //Merge with neighbour leaf
                        printf("Merging Nodes\n");
                        
                        if(root->parent!=NULL && root->parent->activeKeys >= 2)
                        {
                            i = 0;
                            while(i < root->parent->activeKeys && root->parent->children.nodeptr[i]!=root)
                                i++;

                            if(i < root->parent->activeKeys)
                            {
                                //merge with right 
                                int j = i+1;
            
                                BPlusTreeNode *iNode, *jNode;
                                iNode = root->parent->children.nodeptr[i];
                                jNode = root->parent->children.nodeptr[j];
                                printf(" Merging with right neighbour ");
                                int k = 0;
                                printf("\nFrom j : ");
                                while(k < jNode->activeKeys)
                                {
                                    printf("%d ",jNode->children.dataptr[k]->size);
                                    k++;
                                }        
                                root = mergeNodes(iNode,jNode);
                            }
                            else if(i>0)
                            {
                                //merge with left neighbour
                                int j = i-1;

                                BPlusTreeNode *iNode, *jNode;
                                iNode = root->parent->children.nodeptr[i];
                                jNode = root->parent->children.nodeptr[j];
                                printf(" Merging with Left neighbour ");
                                root = mergeNodes(jNode,iNode); 
                            }
                        }
                    }
                }
            }
        }

        else 
        {
            //Finding the suitable pointer
            //printf("Traverse to Leaf");
            
            int i = 0;
            while (i < root->activeKeys && maxTime(node->departureTime, root->key[i]) == (-1))
                i++;
            if(root->children.nodeptr[i]==NULL) printf("No such Flight\n"); 
            else root->children.nodeptr[i] = deletionInbplusTree(root->children.nodeptr[i], node);
        }
    }
    
    return root;
}

Bucket *createBucket(Flight *flight)
{
    Bucket *new = (Bucket *)malloc(sizeof(Bucket));
    new->beginningETA.Hour = flight->ETA.Hour;
    new->endETA.Hour = flight->ETA.Hour;
    new->beginningETA.Min = 0;
    new->endETA.Min = 59;
    new->next = NULL;
    new->root = NULL;
    new->isLeaf = 1;
    return new;
}

Bucket *insert(Bucket *firstBucket, Flight *flight)
{
    // Find an appropriate Bucket
    Bucket *tempBucketPtr = firstBucket;
    Bucket *prevBucketPtr = NULL;

    if (tempBucketPtr == NULL)
    {
        firstBucket = createBucket(flight);
        tempBucketPtr = firstBucket;
        printf("created first bucket");
    }
    
    else
    {
        while (tempBucketPtr != NULL && maxTime(tempBucketPtr->endETA, flight->ETA) > 0)
        {
            prevBucketPtr = tempBucketPtr;
            tempBucketPtr = tempBucketPtr->next;
        }

        if (tempBucketPtr != NULL && maxTime(flight->ETA, tempBucketPtr->endETA) >= 0 && maxTime(tempBucketPtr->beginningETA, flight->ETA) >= 0)
        {
            // bucket found
            printf("\nBucket Found\n");
        }
        else
        {
            printf("\nNew Bucket\n");
            Bucket *newBucket = createBucket(flight);
            newBucket->next = tempBucketPtr;
            if (tempBucketPtr == firstBucket)
            {
                if (maxTime(tempBucketPtr->beginningETA, newBucket->beginningETA) < 0)
                {
                    // printf ("Insert at start");
                    firstBucket = newBucket;
                }
            }
            else
            {
                prevBucketPtr->next = newBucket;
            }
            tempBucketPtr = newBucket;
        }
        tempBucketPtr->root = insertInbPlusTree(tempBucketPtr->root, flight);
    }
    
    Bucket *BucketPtr = firstBucket;
    return firstBucket;
}

Flight *searchForFlight (BPlusTreeNode *root, int ID)
{
    if (root->isLeaf != 1)
    {
        return searchForFlight(root->children.nodeptr[0], ID);
    }
    DataNode *Node = root->children.dataptr[0];
    while (Node != NULL)
    {
        Flight *flightPtr = Node->lptr;
        while (flightPtr != NULL)
        {
            if (flightPtr->flightID == ID)
            {
                printf ("Flight: Departure %d:%d and Arrival %d:%d ",flightPtr->departureTime.Hour, flightPtr->departureTime.Min, flightPtr->ETA.Hour, flightPtr->ETA.Min);
                return flightPtr;
            }
            flightPtr = flightPtr->next;
        }
        Node = Node->next;
    }
    return NULL;
}

Flight *checkStatus (Bucket *firstBucket, int ID)
{
    Bucket *bucketPtr = firstBucket;
    
    int found = 0;
    Flight *flight;
    while (bucketPtr != NULL && found == 0)
    {
        //search inside the bucket
        //this needs to be taken care
        if (bucketPtr->root != NULL)
        {
            flight = searchForFlight(bucketPtr->root, ID);
            if (flight != NULL)
            {
                found = 1;
            }
        }
        bucketPtr = bucketPtr->next;
    }
    if (found == 0)
    {
        printf ("Flight not found");
    }
    return flight;
}

Bucket *cancelFlight(Bucket *firstBucket)
{
    int ID;
    printf ("Enter the ID of the flight to be cancelled : ");
    scanf ("%d", &ID);
    Flight *flight = checkStatus (firstBucket,ID);
    Bucket *bucketPtr = firstBucket;
    Bucket *prev = firstBucket;
    while (bucketPtr != NULL && maxTime(flight->ETA, bucketPtr->endETA) < 0) 
    {
        prev = bucketPtr;
        bucketPtr = bucketPtr->next;
    }
    if (bucketPtr == NULL)
    {
        printf ("Flight Doesn't exist");
    }
    else
    {
        bucketPtr->root = deletionInbplusTree(bucketPtr->root, flight);
    }
    if (bucketPtr->root == NULL)
    {
        prev->next = bucketPtr->next;
        if (bucketPtr == prev) firstBucket = prev;
        free (bucketPtr);
    }
    return firstBucket;
}

void rangeSearch(Bucket *firstBucket)
{
    Time start, end;
    printf ("Enter the start time: ");
    scanf ("%d:%d", &start.Hour,&start.Min);
    printf ("Enter the end time: ");
    scanf ("%d:%d", &end.Hour,&end.Min);

    if (maxTime (start, end) < 1) printf ("Invalid");

    Bucket *bucketPtr = firstBucket;
    while (bucketPtr != NULL )
    {
        BPlusTreeNode *root = bucketPtr->root;
        int flag = 0;

        while (root != NULL && root->isLeaf != 1 && flag == 0)
        {
            int i = 0;
            while ( i < root->activeKeys && maxTime(root->key[i], start) > 1) i++;
            if ( root->children.nodeptr[i] != NULL)
            {
                root = root->children.nodeptr[i];
            }
            else flag = 1;
        }
        
        if (root != NULL && flag == 0)
        {
            DataNode *Node = root->children.dataptr[0];

            while (Node != NULL && flag == 0)
            {
                Flight *flightPtr = Node->lptr;
                while (flightPtr != NULL && maxTime(flightPtr->departureTime, start) > 0) flightPtr = flightPtr->next;
                while ( flightPtr != NULL && maxTime(flightPtr->departureTime, end) >= 0) 
                {
                    printf ("Flight Id: %d, Departure Time %d:%d\n", flightPtr->flightID, flightPtr->departureTime.Hour, flightPtr->departureTime.Min);
                    flightPtr = flightPtr->next;
                }
                Node = Node->next;
            }
        }
        bucketPtr = bucketPtr->next;
    }
}

void printtree(Bucket* firstBucket)
{
    Bucket* bucketPtr = firstBucket;
    int counter = 1;
    DataNode *Node;
    while (bucketPtr != NULL)
    {
        //search inside the bucket
        //this needs to be taken care
        
        if (bucketPtr->root != NULL)
        {
            if (bucketPtr->root->isLeaf != 1)
            {
                BPlusTreeNode* temp;
                temp = bucketPtr->root;
                while(temp!=NULL && temp->isLeaf != 1) temp = temp->children.nodeptr[0];
                if(temp!=NULL)Node = temp->children.dataptr[0]; 
                else Node = NULL;
            }
            else Node = bucketPtr->root->children.dataptr[0];    
        }

        while (Node != NULL)
        {
            printf("Data Node entries:\n");
            Flight *flightPtr = Node->lptr;
            while (flightPtr != NULL)
            {
                visit(flightPtr);
                flightPtr = flightPtr->next;
            }
            Node = Node->next;
            printf("End of Data Node\n\n");
        }
        
        bucketPtr = bucketPtr->next;
    }
}

void showMenu(Bucket *firstBucket)
{
    int option;
    do
    {
        printf("\n\n1 - Insert a Flight \n2 - Cancel Flight \n3 - Check Status \n4 - To search a range of flights\n5 - Print all Flights\nPress any other key to exit\n\n");
        printf("Select option : ");
        scanf("%d", &option);
        Time temp, deptTime, ETA;
        Flight *flight;

        switch (option)
        {
        case 1:
            printf("Enter details for the flight to be added :\n");
            int ID;
            printf("Flight ID : ");
            scanf("%d", &ID);
            printf("Departure time : \nhr = ");
            scanf("%d", &deptTime.Hour);
            printf("min = ");
            scanf("%d", &deptTime.Min);

            printf("Expected Arrival time : \nhr = ");
            scanf("%d", &ETA.Hour);
            printf("min = ");
            scanf("%d", &ETA.Min);

            if (deptTime.Hour > 23 || deptTime.Min > 59 ||
                ETA.Hour >23 || ETA.Min >59 ) printf ("Invalid Time");
            else 
            {
                Flight *newNode;
                newNode = createNode(ID, deptTime, ETA);
                firstBucket = insert(firstBucket, newNode);
                printf("Flight Added Successfully.\n");
            }
            break;

        case 2:
            firstBucket = cancelFlight(firstBucket);
            break;

        case 3:
            printf ("Enter flightID : ");
            int flightID;
            scanf ("%d", &flightID);
            flight = checkStatus(firstBucket,flightID);
            break;
        case 4:
            rangeSearch(firstBucket);
            break;
        case 5:
            printtree(firstBucket);
        }
    } while (option > 0 && option < 6);
}

int main()
{
    int tempFlightID;
    Time tempDepartureTime, tempETA;
    FILE *fptr = fopen("Data.csv", "r");

    Flight *planeNode; // temp pointer to link nodes in list

    Bucket *firstBucket;
    firstBucket = NULL;
    char line[100];
    while (fgets(line, 100, fptr) != NULL)
    {
        sscanf(line, "%d,%d,%d,%d,%d", &tempFlightID, &(tempETA.Hour), &(tempETA.Min), &(tempDepartureTime.Hour), &(tempDepartureTime.Min));
        planeNode = createNode(tempFlightID, tempDepartureTime, tempETA);
        firstBucket = insert(firstBucket, planeNode);
        printf ("\n");
    }
    showMenu(firstBucket);
    return 0;
}
