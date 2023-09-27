#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef int Elem_t;
typedef unsigned long long canary_t;

#define ON_OFF 1
#define STACK_DUMP(stk) Stack_Dump(stk,(char*)__FILE__, __LINE__, __func__);
#define stack_t "%d "

#if ON_OFF == 0
    #define size_canary (2 * sizeof ( canary_t ))
#else
    #define size_canary 0
#endif

enum Errors{
    NO_ERROR = 0,
    DATA_NULL = 1,
    CAPACITY_ZERO = 2,
    STK_NULL = 3,
    CAPASITY_TOO_SMALL = 4
};

void Put_canary(struct Stack* stk, struct Canary* canary, int on_off);
void Canareyca_Protection(struct Stack* stk, struct Canary* canary, int on_off);
int Check_Canareyka(struct Canary* canary, int on_off);
int Hash_Protection(struct Stack* stk);
int Calculate_Hash(struct Stack* stk);
struct Stack* Stack_Ctor( int capacity, struct ERRORS* ERR, struct Canary* canary, int on_off);
int Stack_Dtor(struct Stack* stk, struct Canary* canary);
int Stack_Push(struct Stack* stk, Elem_t val, struct Canary* canary);
int Stack_Pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary);
int Stack_Dump(struct Stack* stk, char* file , int line, const char* func);
int Stack_Realloc(struct Stack* stk, struct Canary* canary);
int Stack_Realloc_Press(struct Stack* stk, struct Canary* canary);
int StackErr(const Stack* stk);
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
    int* err;
};

int Calculate_Hash(struct Stack* stk)
{
    stk -> hash = 0;

    for (int i = 0 ; i < stk -> size; i++)
    {
        stk -> hash += *(stk -> data + i);
    }

    stk -> hash = stk -> hash * (stk -> size) * (stk -> capacity);

    return stk -> hash;
}

void Put_canary(struct Stack* stk, struct Canary* canary, int on_off)
{
    canary -> left_canary_data =(canary_t*)  stk -> data;
    canary -> right_canary_data = ((canary_t*)  (stk -> data + stk -> capacity)) + 1;
    canary -> left_canary_stk = ((canary_t*) stk) - 1;
    canary -> right_canary_stk = (canary_t*)((char*) stk + sizeof(struct Stack));                              //pizdec nugno razobratca
    *(canary -> left_canary_data) = 1111111;
    *(canary -> right_canary_data ) = 1111111;
    *(canary -> left_canary_stk) = 1111111;
    *(canary -> right_canary_stk ) = 1111111;

    stk -> data = (Elem_t*) ((canary_t*)(stk -> data) + 1);
}

void Canareyca_Protection(struct Stack* stk, struct Canary* canary, int on_off)                          // print canareyka protection
{
    if (on_off == 1)
    {
        stk -> data = ( Elem_t* ) calloc ( stk -> capacity *  sizeof ( Elem_t )  + (sizeof (canary_t) * 2), 1 );

        Put_canary(stk, canary, ON_OFF);

    }
    else
    {
        stk -> data = ( Elem_t* ) calloc ( stk -> capacity, sizeof ( Elem_t ) );
    }
}

int Check_Canareyka(struct Canary* canary, int on_off)
{
    if (ON_OFF == 0)
    {
        return 0;
    }
    else if (*(canary -> left_canary_data) != 1111111 || *(canary -> right_canary_data) != 1111111)
    {
        printf("\nDATA WAS RUBBISHED!!!!!!\n");
        //printf("left_canary_data: %llu, right_canary_data: %llu \n", *(canary -> left_canary_data), *(canary -> right_canary_data));
        return 1;
    }
    else if (*(canary -> left_canary_stk) != 1111111 || *(canary -> right_canary_stk) != 1111111)
    {
        printf("\nSTACK WAS RUBBISHED!!!!!!\n");
        return 1;
    }
    return 0;
}

int Hash_Protection(struct Stack* stk)
{
    Calculate_Hash(stk);

    if (stk -> hash == stk -> last_hash)
    {
        stk -> last_hash = stk -> hash;
        return 0;
    }
    else
    {
        printf("\nSTACK WAS CHANGED, HASH DOES NOT EQUAL PREVIOUS HASH!!!!\n SHIT!!!!\n last_hash was: %d\n", stk -> last_hash);

        stk -> last_hash = stk -> hash;
        return 1;
    }
}

void Clean_buf()
{
    while(getchar() != '\n')
        ;
}

struct Stack* Stack_Ctor( int capacity, struct ERRORS* ERR, struct Canary* canary, int on_off)
{
    //assert(stk -> data != NULL);
    struct Stack* stk = (struct Stack*) calloc( sizeof ( struct Stack ) + 2 * sizeof(canary_t), sizeof ( char ));

    if (on_off == 1)
    {
        stk = (Stack*)((canary_t*)stk + 1);
    }

    stk -> capacity = capacity;

    Canareyca_Protection(stk, canary, ON_OFF);

    stk -> size = 0;
    stk -> poizon = -10000;

    Calculate_Hash(stk);

    for (int i = 0; i < stk -> capacity; i++)
    {
        *(stk -> data + i) = stk -> poizon;
    }

    return stk;
}

int Stack_Dtor(struct Stack* stk, struct Canary* canary)
{
    //free(stk -> data);

    Elem_t Ret_val = 0;

    for (int i = 0; i < stk-> size + 1; i++)
    {
        Stack_Pop(stk, &Ret_val, canary);
    }

    return 0;
}

int Stack_Push(struct Stack* stk, Elem_t val, struct Canary* canary)
{
    if (StackErr(stk) != 0 || Check_Canareyka(canary, ON_OFF) != 0 || Hash_Protection(stk) != 0)
    {
        STACK_DUMP(stk)
        return 0;
    }

    if(stk -> size == stk -> capacity)
    {
        Stack_Realloc(stk, canary);
    }

    stk -> data[stk -> size++] = val;

    stk -> last_hash = Calculate_Hash(stk);

    return 0;
}

int Stack_Pop(struct Stack* stk, Elem_t* Ret_val, struct Canary* canary)  // mb error tyt potom
{
    if (StackErr(stk) != 0 || Check_Canareyka(canary, ON_OFF) != 0 || Hash_Protection(stk) != 0)
    {
        STACK_DUMP(stk)
        return 0;
    }

    if (stk -> size <= (stk -> capacity) / 2)
    {
        Stack_Realloc_Press(stk, canary);
    }
    (stk -> size)--;
    *(Ret_val) = stk -> data[stk -> size];

    stk -> data[stk -> size ] = stk -> poizon;

    stk -> last_hash = Calculate_Hash(stk);

    return 0;
}

int Stack_Realloc(struct Stack* stk, struct Canary* canary )
{
    if (ON_OFF == 1)
    {
        stk -> data = (Elem_t*)((canary_t*) (stk -> data) - 1);
    }

    stk -> data = (Elem_t*) realloc( stk -> data, (stk -> capacity) * 2 * sizeof(Elem_t) + 2 * sizeof(canary_t));
    stk -> capacity = stk -> capacity * 2;

    Put_canary(stk, canary, ON_OFF);

    for (int i = stk -> size; i < stk -> capacity; i++)
    {
        stk -> data[i] = stk -> poizon;
    }

    return 0;
}

int Stack_Realloc_Press(struct Stack* stk, struct Canary* canary)
{

    if (ON_OFF == 1)
    {
        stk -> data = (Elem_t*)((canary_t*) (stk -> data) - 1);
    }

    stk -> data = (Elem_t*) realloc( stk -> data, (stk -> capacity) / 2 * sizeof(Elem_t) + 2 * sizeof(canary_t));

    stk -> capacity = stk -> capacity / 2;

    Put_canary(stk, canary, ON_OFF);

    return 0;
}

int Stack_Dump(struct Stack* stk, char* file , int line, const char* func)
{
    printf("\n\nin file: %s \nin: %d row \nin function: %s \nhash = %d \nlast_hash = %d\n", file, line, func, stk -> hash, stk -> last_hash);

    int Code_err = StackErr(stk);

    if (Code_err / 1000 == 1)
    {
        printf("ERROR: capacity < size\n");
    }
    if (Code_err/100 == 11 || Code_err/100 == 1)
    {
        printf("ERROR: capacity = 0\n");
    }
    if (Code_err/10 == 1 || Code_err/10 == 11 || Code_err/10 == 101 || Code_err/10 == 111 )
    {
        printf("ERROR: data = NULL\n");
    }
    if (Code_err < 10 && Code_err > 0)
    {
        printf("ERROR: stk = NULL\n");
    }

    printf("Stack[%p] \n{ \n    size = %d \n    capacity = %d \n    data[%p] \n        { \n", stk, stk ->size, stk -> capacity, stk -> data);

    for (int i = 0; i < stk -> capacity; i++)
    {
        if (i < stk -> size)
        {
            printf("        *[%d] = %d\n", i, stk -> data[i]);
        }
        else
        {
            printf("         [%d] = NAN (Poizon)\n", i);
        }
    }
    printf("        } \n    } \n");

    return 0;
}

void Print_data(struct Stack* stk)
{
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

int StackErr(const Stack* stk)
{
    int Code_err = 0;
    if (!stk)
    {
        Code_err = Code_err + 1;
    }
    if (!(stk -> data))
    {
        Code_err = Code_err + 10;
    }
    if (!stk -> capacity)
    {
        Code_err = Code_err + 100;
    }
    if (stk -> capacity < stk -> size)
    {
        Code_err = Code_err + 1000;
    }
    return Code_err;
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

struct Stack* stk = Stack_Ctor( capacity, &ERR, &canary, ON_OFF);
Print_data(stk);

Cycle_push(stk, &val, &canary);
Cycle_pop(stk, &Ret_val, &canary);

Print_data(stk);

Stack_Dtor(stk, &canary);

Print_data(stk);
}

