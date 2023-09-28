#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef int Elem_t;
typedef unsigned long long canary_t;

#define ON_OFF 1
#define HASH_ON 1

#define STACK_DUMP(stk, canary) Stack_Dump(stk, canary, (char*)__FILE__, __LINE__, __func__);
#define stack_t "%d "

#ifdef ON_OFF
    #define size_canary (2 * sizeof ( canary_t ))
#else
    #define size_canary 0
#endif

enum Errors : unsigned int
{
    NO_ERROR                = 0,
    DATA_NULL               = 1 << 0,
    CAPACITY_ZERO           = 1 << 1,
    STK_NULL                = 1 << 2,
    CAPASITY_TOO_SMALL      = 1 << 3,
    DATA_WAS_RUBBISHED      = 1 << 4,
    STK_WAS_RUBBISHED       = 1 << 6,
    LEFT_CANARY_DATA_DIE    = 1 << 7,
    RIGHT_CANARY_DATA_DIE   = 1 << 8,
    LEFT_CANARY_STK_DIE     = 1 << 9,
    RIGHT_CANARY_STK_DIE    = 1 << 10,
    HASH_WAS_CHANGED        = 1 << 11
};


void Put_canary(struct Stack* stk, struct Canary* canary);
void Canareyca_Protection(struct Stack* stk, struct Canary* canary);
int Hash_Protection(struct Stack* stk);
int Calculate_Hash(struct Stack* stk);
struct Stack* Stack_Ctor( int capacity, struct ERRORS* ERR, struct Canary* canary);
int Stack_Dtor(struct Stack* stk, struct Canary* canary);
int Stack_Push(struct Stack* stk, Elem_t val, struct Canary* canary);
int Stack_Pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary);
int Stack_Dump(struct Stack* stk, struct Canary* canary, char* file , int line, const char* func);
int Stack_Realloc(struct Stack* stk, struct Canary* canary);
int Stack_Realloc_Press(struct Stack* stk, struct Canary* canary);
unsigned int StackErr(struct Stack* stk, struct Canary* canary);
void Print_data(struct Stack stk);
void Cycle_push(struct Stack* stk, Elem_t* val, struct Canary* canary);
void Cycle_pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary);
void Strinput(char* input_str);
int Cmp_two_str(char* input_str, char* answer);
void Clean_buf();



struct Stack
{
    Elem_t* data;
    int size;
    int capacity;
    Elem_t poizon;
    Elem_t hash;
    Elem_t last_hash;
    unsigned int Code_err;
};

struct Canary
{
    canary_t* left_canary_data;
    canary_t* right_canary_data;
    canary_t* left_canary_stk;
    canary_t* right_canary_stk;
};

struct ERRORS
{
    Errors err;
};



struct Stack* Stack_Ctor( int capacity, struct ERRORS* err, struct Canary* canary)
{
    assert(canary);

    struct Stack* stk = (struct Stack*) calloc( sizeof ( struct Stack ) + size_canary, sizeof ( char ));

    assert(stk);

    #ifdef ON_OFF
    stk = (Stack*)((canary_t*)stk + 1);
    #endif

    stk -> capacity = capacity;

    Canareyca_Protection(stk, canary);

    stk -> size = 0;
    stk -> poizon = -10000;

    #ifdef HASH_ON
    stk -> last_hash = Calculate_Hash(stk);
    #endif

    for (int i = 0; i < stk -> capacity; i++)
    {
        *(stk -> data + i) = stk -> poizon;
    }

    if (StackErr(stk, canary) != NO_ERROR)
    {
        STACK_DUMP(stk, canary)
        return 0;
    }
    else
    {
    STACK_DUMP(stk, canary)
    }

    return stk;
}

int Stack_Push(struct Stack* stk, Elem_t val, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    if (StackErr(stk, canary) != NO_ERROR)
    {
        STACK_DUMP(stk, canary)
        return 0;
    }

    if(stk -> size == stk -> capacity)
    {
        Stack_Realloc(stk, canary);
    }

    stk -> data[stk -> size ++] = val;

    #ifdef HASH_ON
    stk -> last_hash = Calculate_Hash(stk);
    #endif

    if (StackErr(stk, canary) != NO_ERROR)
    {
        STACK_DUMP(stk, canary)
        return 0;
    }
    else
    {
    STACK_DUMP(stk, canary)
    }

    return 0;
}

int Stack_Pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    if (StackErr(stk, canary) != NO_ERROR)
    {
        STACK_DUMP(stk, canary)
        return 0;
    }

    if (stk -> size <= (stk -> capacity) / 2)
    {
        Stack_Realloc_Press(stk, canary);
    }

    (stk -> size)--;
    *(Ret_val) = stk -> data[stk -> size];

    stk -> data[stk -> size ] = stk -> poizon;

    #ifdef HASH_ON
    stk -> last_hash = Calculate_Hash(stk);
    #endif

    if (StackErr(stk, canary) != NO_ERROR)
    {
        STACK_DUMP(stk, canary)
        return 0;
    }
    else
    {
    STACK_DUMP(stk, canary)
    }

    return 0;
}

int Stack_Realloc(struct Stack* stk, struct Canary* canary )
{
    assert(stk);
    assert(canary);

    #ifdef ON_OFF
        stk -> data = (Elem_t*)((canary_t*) (stk -> data) - 1);
    #endif

    stk -> data = (Elem_t*) realloc( stk -> data, (stk -> capacity) * 2 * sizeof(Elem_t) + size_canary);
    stk -> capacity = stk -> capacity * 2;

    #ifdef ON_OFF
    Put_canary(stk, canary);
    #endif

    for (int i = stk -> size; i < stk -> capacity; i++)
    {
        stk -> data[i] = stk -> poizon;
    }

    return 0;
}

int Stack_Realloc_Press(struct Stack* stk, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    #ifdef ON_OFF
        stk -> data = (Elem_t*)((canary_t*) (stk -> data) - 1);
    #endif

    stk -> data = (Elem_t*) realloc( stk -> data, (stk -> capacity) / 2 * sizeof(Elem_t) + size_canary);

    stk -> capacity = stk -> capacity / 2;

    #ifdef ON_OFF
    Put_canary(stk, canary);
    #endif

    return 0;
}

int Stack_Dump(struct Stack* stk, struct Canary* canary, char* file , int line, const char* func)
{
    assert(stk);
    #ifdef HASH_ON
    printf("\n\nFile: %s \nin: %d row \nFunction: %s \nhash = %d \nlast_hash = %d\n", file, line, func, stk -> hash, stk -> last_hash);
    #else
    printf("\n\nFile: %s \nin: %d row \nFunction: %s \n", file, line, func);
    #endif

    stk -> Code_err = StackErr(stk, canary);

    if (stk -> Code_err & CAPASITY_TOO_SMALL)
    {
        printf("ERROR: capacity < size\n");
    }
    if (stk -> Code_err & CAPACITY_ZERO)
    {
        printf("ERROR: capacity = 0\n");
    }
    if (stk -> Code_err & DATA_NULL)
    {
        printf("ERROR: data = NULL\n");
    }
    if (stk -> Code_err & STK_NULL)
    {
        printf("ERROR: stk = NULL\n");
    }

    #ifdef ON_OFF
    if (stk -> Code_err & LEFT_CANARY_DATA_DIE)
    {
        printf("LEFT_CANARY_DETA_DIE\n");
    }
    if (stk -> Code_err & RIGHT_CANARY_DATA_DIE)
    {
        printf("RIGHT_CANARY_DATA_DIE\n");
    }
    if (stk -> Code_err & LEFT_CANARY_STK_DIE)
    {
        printf("LEFT_CANARY_STK_DIE\n");
    }
    if (stk -> Code_err & RIGHT_CANARY_STK_DIE)
    {
        printf("RIGHT_CANARY_STK_DIE\n");
    }
    #endif

    #ifdef HASH_ON
    if (stk -> Code_err & HASH_WAS_CHANGED)
    {
        printf("HASH_WAS_CHANGED\n");
    }
    #endif

    printf("================================\n");
    printf("Stac[%p] \n{ \n    size = %d \n    capacity = %d \n    data[%p] \n        { \n", stk, stk ->size, stk -> capacity, stk -> data);

    #ifdef ON_OFF
    printf("\tLEFT CANARY = %llu\n", *(canary -> left_canary_data));
    #endif

    for (int i = 0; i < stk -> capacity; i++)
    {
        if (i < stk -> size)
        {
            printf("        *[%d] = "stack_t"\n", i, stk -> data[i]);
        }
        else
        {
            printf("         [%d] = NAN (Poizon)\n", i);
        }
    }

    #ifdef ON_OFF
    printf("\tRIGHT CANARY = %llu\n", *(canary -> right_canary_data));
    #endif

    printf("        } \n    } \n");
    printf("================================\n");

    return 0;
}

int Stack_Dtor(struct Stack* stk, struct Canary* canary)
{
    //free(stk -> data);
    assert(stk);
    assert(canary);

    Elem_t Ret_val = 0;

    for (int i = 0; i < stk-> size + 1; i++)
    {
        Stack_Pop(stk, &Ret_val, canary);
    }

    return 0;
}



int Calculate_Hash(struct Stack* stk)
{
    assert(stk);

    stk -> hash = 0;

    for (int i = 0 ; i < stk -> size; i++)
    {
        stk -> hash += *(stk -> data + i);
    }

    stk -> hash = stk -> hash * (stk -> size) * (stk -> capacity);

    return stk -> hash;
}

void Put_canary(struct Stack* stk, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    #ifdef ON_OFF
    canary -> left_canary_data =(canary_t*)  stk -> data;
    canary -> right_canary_data = ((canary_t*)  (stk -> data + stk -> capacity));
    canary -> left_canary_stk = ((canary_t*) stk) - 1;
    canary -> right_canary_stk = (canary_t*)((char*) stk + sizeof(struct Stack));                              //pizdec nugno razobratca
    *(canary -> left_canary_data) = 1111111;
    *(canary -> right_canary_data ) = 1111111;
    *(canary -> left_canary_stk) = 1111111;
    *(canary -> right_canary_stk ) = 1111111;

    stk -> data = (Elem_t*) ((canary_t*)(stk -> data) + 1);
    #endif
}

void Canareyca_Protection(struct Stack* stk, struct Canary* canary)                          // print canareyka protection
{
    assert(stk);
    assert(canary);

    #ifdef ON_OFF
    stk -> data = ( Elem_t* ) calloc ( stk -> capacity *  sizeof ( Elem_t )  + (sizeof (canary_t) * 2), 1 );

    Put_canary(stk, canary);
    #else
    stk -> data = ( Elem_t* ) calloc ( stk -> capacity, sizeof ( Elem_t ) );
    #endif
}

int Hash_Protection(struct Stack* stk)
{
    assert(stk);

    #ifdef HASH_ON
    Calculate_Hash(stk);

    if (stk -> hash == stk -> last_hash)
    {
        stk -> last_hash = stk -> hash;
        return 0;
    }
    else
    {
        stk -> last_hash = stk -> hash;
        return 1;
    }
    #else
    return 1;
    #endif

}

void Clean_buf()
{
    while(getchar() != '\n')
        ;
}

void Print_data(struct Stack* stk)
{
    assert(stk);

    for (size_t i = 0; i < stk -> capacity; i++)
    {
        printf(stack_t, stk -> data[i]);
    }

    printf("\n");
}

void Strinput(char* input_str)
{
    int i = 0;
    char c = ' ';

    while ((c = getchar()) != '\n') {
        input_str[i] = c;
        i++;
    }

}

int Cmp_two_str(char* input_str, char* answer)
{
    int len = strlen(input_str);
    int i = 0;

    while (*(input_str + i) == *(answer + i))
    {
        i++;
    }
    if (i == len + 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Cycle_push(struct Stack* stk, Elem_t* val, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    while (true)
    {
        Clean_buf();

        printf("Do you want add anything? Print YES or NO. \n");
        char input_str[99] = {};
        char answer_yes[4] = "yes";
        char answer_YES[4] = "YES";

        Strinput(input_str);

        if (Cmp_two_str(input_str, answer_yes) || Cmp_two_str(input_str, answer_YES))
        {
            printf("Print value that you want to add: ");
            scanf("%d", val);
            Stack_Push(stk, *(val), canary);
            Print_data(stk);
        }
        else
        {
            printf("Thank you goodbye!!! \n");
            break;
        }
    }
}

void Cycle_pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    while (true)
    {
        Clean_buf();
        printf("Do you want pop anything? Print YES or NO. \n");
        char input_str[99] = {};
        char answer_yes[4] = "yes";
        char answer_YES[4] = "YES";

        Strinput(input_str);

        if (Cmp_two_str(input_str, answer_yes) || Cmp_two_str(input_str, answer_YES))
        {
            Stack_Pop(stk, Ret_val, canary);
            printf("Returned value: %d\n", *(Ret_val));
            Print_data(stk);
        }
        else
        {
            printf("Thank you goodbye!!! \n");
            break;
        }
    }
}

unsigned int StackErr(struct Stack* stk, struct Canary* canary)
{
    assert(stk);
    assert(canary);

    stk -> Code_err = NO_ERROR;

    if (!stk)
    {
        stk -> Code_err |= STK_NULL;
    }
    if (!(stk -> data))
    {
        stk -> Code_err |= DATA_NULL;
    }
    if (!stk -> capacity)
    {
        stk -> Code_err |= CAPACITY_ZERO;
    }
    if (stk -> capacity < stk -> size)
    {
        stk -> Code_err |= CAPASITY_TOO_SMALL;
    }

    #ifdef ON_OFF
    if (*(canary -> left_canary_data) != 1111111)
    {
        stk -> Code_err |= LEFT_CANARY_DATA_DIE;
    }
    if (*(canary -> right_canary_data) != 1111111)
    {
        stk -> Code_err |= RIGHT_CANARY_DATA_DIE;
    }
    if (*(canary -> left_canary_stk) != 1111111)
    {
        stk -> Code_err |= LEFT_CANARY_STK_DIE;
    }
    if (*(canary -> right_canary_stk) != 1111111)
    {
        stk -> Code_err |= RIGHT_CANARY_STK_DIE;
    }
    #endif

    #ifdef HASH_ON
    if (Hash_Protection(stk))
    {
        stk -> Code_err |= HASH_WAS_CHANGED;
    }
    #endif

    return stk -> Code_err;

}



int main()
{
Elem_t val = 0;
Elem_t Ret_val = 0;
int capacity = 0;
struct ERRORS ERR = {};
struct Canary canary = {};


printf("Print capacity of stack: ");
scanf(stack_t, &capacity);

struct Stack* stk = Stack_Ctor( capacity, &ERR, &canary);
Print_data(stk);

Cycle_push(stk, &val, &canary);
Cycle_pop(stk, &Ret_val, &canary);

Print_data(stk);

Stack_Dtor(stk, &canary);

Print_data(stk);
}

