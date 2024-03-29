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

int BucketId = 1000;

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
        printf("\n\nFlight ID is %d\n", ptr->flightID);
        printf("Flight Departure = %d:%d\n\n", ptr->departureTime.Hour, ptr->departureTime.Min);
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
        newNode->children.dataptr[i] = NULL;
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
    printf ("split BPlusTreeNode\n");
    int i;
    DataNode *temp = root->children.dataptr[5];
    Flight *ptr = temp->lptr;

    //create new Node
    BPlusTreeNode *newNode = createTreeNode();
    newNode->activeKeys = order/2;
    newNode->isLeaf = 1;
    for (i = order/2 + 1; i <= order; i++)
    {
        newNode->children.dataptr[i-order/2 -1] = root->children.dataptr[i];
        newNode->key[i-order/2 -1] = root->key[i];
        root->children.dataptr[i] = NULL;
    }
    printf ("%d", i);
    newNode->children.dataptr[i-order/2-1] = root->children.dataptr[i];

    newNode->activeKeys = root->activeKeys = order/2;
    root->children.dataptr[order/2]->next = newNode->children.dataptr[0];
    newNode->children.dataptr[0]->prev = root->children.dataptr[order/2];
    if (root->parent == NULL)
    {
        BPlusTreeNode *newRoot = createTreeNode();
        newNode->parent = root->parent = newRoot;
        newRoot->key[0] = root->key[order/2];
        // printf ("%d:%d ", newRoot->key[0].Hour, newRoot->key[0].Min);
        newRoot->isLeaf = 0;
        newRoot->children.nodeptr[0] = root;
        newRoot->children.nodeptr[1] = newNode;
        newRoot->parent = NULL;
        newRoot->activeKeys = 1;
        printTree(newRoot);
    }

    else
    {
        printf ("split nonroot");
        BPlusTreeNode *parent = root->parent;
        i = parent->activeKeys-1;
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
        parent->activeKeys++;
        printf ("\n");
        for (i = 0; i < parent->activeKeys; i++)
        {
            printf ("%d:%d ", parent->key[i].Hour, parent->key[i].Min);
        }
        printf ("\n");
        for (i = 0; i <= parent->activeKeys; i++)
        {
            if (parent->children.nodeptr[i] != NULL)
            {
                BPlusTreeNode *traversalnode = parent->children.nodeptr[i];
                for (int j = 0; j < traversalnode->activeKeys; j++)
                {
                    printf ("%d:%d ",traversalnode->key[j]);
                }
                for (int j = 0; j <= traversalnode->activeKeys; j++)
                {
                    printf ("Now print respective dataNodes: \n");
                    DataNode *dataNodeTraversal = traversalnode->children.dataptr[j];
                    if (dataNodeTraversal != NULL)
                    {
                        printf ("flighttraversal\n");
                        Flight *flightTraversal = dataNodeTraversal->lptr;
                        while (flightTraversal != NULL)
                        {
                            printf ("%d:%d ", flightTraversal->departureTime.Hour, flightTraversal->departureTime.Min);
                            flightTraversal = flightTraversal->next;
                        }
                        printf ("\n");
                    } 
                }
                printf ("\n");
            }
        }
        if (parent->activeKeys == order+1)
        {
            printf ("here comes the recursion");
            parent = splitBPlusTreeNode(parent);
        }
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
        printf("NULL");
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
        printf("Else part");
        int i = 0;
        while (i < root->activeKeys && maxTime(newNode->departureTime, root->key[i]) == (-1))
            i++;

        if (i == root->activeKeys)
            root->children.nodeptr[i] = insertInbPlusTree(root->children.nodeptr[i], newNode);
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

int searchForFlight (BPlusTreeNode *root, int ID)
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
            printf ("%d:%d ",flightPtr->departureTime.Hour, flightPtr->departureTime.Min);
            if (flightPtr->flightID == ID)
            {
                printf ("The flight departed at %d:%d and is expected to arrive at %d:%d ",flightPtr->departureTime.Hour, flightPtr->departureTime.Min, flightPtr->ETA.Hour, flightPtr->ETA.Min);
                return 1;
            }
            flightPtr = flightPtr->next;
        }
        Node = Node->next;
    }
    return 0;
}

void checkStatus (Bucket *firstBucket)
{
    Bucket *bucketPtr = firstBucket;
    printf ("Enter flightID: ");
    int ID;
    scanf ("%d", &ID);
    int found = 0;
    while (bucketPtr != NULL && found == 0)
    {
        //search inside the bucket
        //this needs to be taken care
        if (bucketPtr->root != NULL)
        {
            if (searchForFlight(bucketPtr->root, ID))
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
}

void showMenu(Bucket *firstBucket)
{
    int option;
    do
    {
        printf("\n\n1 - Set Current Time\n2 - Insert a Flight \n3 - Cancel Flight \n4 - Check Status \n5 - Rearrange Buckets\n6 - Show Bucket List\nPress any other key to exit\n\n");
        printf("Select option : ");
        scanf("%d", &option);
        Time temp, deptTime, ETA;

        switch (option)
        {
        case 1:
            printf("Enter time: ");
            scanf("%d", &temp.Hour);
            scanf("%d", &temp.Min);
            if (temp.Hour > 23 || temp.Min > 59 ) printf ("invalid time");
            else
                // firstBucket = setCurrentTime(firstBucket, temp);
            break;

        case 2:
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

        case 3:
            printf ("to be implemented"); 
            // firstBucket = cancelFlight(firstBucket);
            break;

        case 4:
            printf ("to be implemented"); 
            checkStatus(firstBucket);
            break;

        case 5:
            printf ("to be implemented"); 
            // rearrangeBuckets(firstBucket);
            break;

        case 6:
            printf ("to be implemented"); 
            // showBucketList(firstBucket);
        }
    } while (option > 0 && option < 7);
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
        printf ("inserted");
        printf ("\n");
    }
    printf ("##outside##");
    showMenu(firstBucket);
    return 0;
}