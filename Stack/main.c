#include <stdio.h>
#include <windows.h>
#include <assert.h>




//********* Стек ***********************************
#define MAX_STACK_SIZE 100
#define I_AM_NO_A_VALID_RESULT 9999
typedef int StackElement;

struct Stack {
    StackElement arr[MAX_STACK_SIZE];
    StackElement index_last;
    StackElement index_first;
};

// Проинициализировать внутреннее содержимое структуры стека
void stack_init(struct Stack* s) {
    assert(s != NULL);
    s->index_last = 0;
    s->index_first = 0;
    for(int i = 0; i < MAX_STACK_SIZE; i++){
        s->arr[i] = 0;
    }
}

// Вернуть количество элементов в стеке
size_t stack_size(struct Stack* s) {
    assert(s != NULL);
    return s->index_last - s->index_first;
}

// Вернуть значение на верхушке стека, без изменения верхушки
StackElement stack_peek(struct Stack* s) {
    assert(s != NULL);
    if(s->index_last){
        return (s->arr[s->index_last - 1]);
    }
    return I_AM_NO_A_VALID_RESULT;
}

// Поместить значение x на верхушку стека
void stack_push(struct Stack* s, StackElement x) {
    assert(s != NULL);
    assert(s->index_last < MAX_STACK_SIZE - 1);
    s->arr[(s->index_last)++] = x;

}

// Достать значение с верхушки стека
StackElement stack_pop(struct Stack* s) {
    assert(s != NULL);
    assert(s->index_last > 0);
    return s->arr[--(s->index_last)];
}

//********* Тесты *******************************************
#define check_expr(expr, expected) {\
	int result = (expr); \
	if (result != (expected)) {\
		printf("Ожидал:\t '" #expr "'\t to == '%d', получил '%d' \t(функция %s в строке %d).\n", \
			   (expected), result, __FUNCTION__, __LINE__);\
		error_count++;\
	}\
}

int error_count = 0;

void test_stack() {
    printf("\nТест 1\n");
    {
        struct Stack empty_stack;
        stack_init(&empty_stack);
        check_expr(stack_size(&empty_stack), 0);
    }
    printf("\nТест 2\n");
    {
        struct Stack simple_stack;
        stack_init(&simple_stack);

        stack_push(&simple_stack, 5);
        check_expr(stack_peek(&simple_stack), 5);
        check_expr(stack_size(&simple_stack), 1);
    }
    printf("\nТест 3\n");
    {
        struct Stack push_pop_stack;
        stack_init(&push_pop_stack);

        stack_push(&push_pop_stack, 5);
        check_expr(stack_pop(&push_pop_stack), 5);
    }
    printf("\nТест 4\n");
    {
        struct Stack complex_stack;
        stack_init(&complex_stack);
        for (int i = 1; i <= 80; i++) {
            stack_push(&complex_stack, i);
        }
        for (int i = 80; i > 40; i--) {
            stack_pop(&complex_stack);

        }
        check_expr(stack_size(&complex_stack), 40);
        check_expr(stack_peek(&complex_stack), 40);
        check_expr(stack_pop(&complex_stack), 40);
    }
}

//****************************************************************
int main() {
    SetConsoleOutputCP(CP_UTF8);
    test_stack();

    if (error_count) {
        printf("\n\n!!!!!!!!!! Обнаружены ошибки (см. выше), исправьте и попробуйте снова !!!!!!!!!!!!!!!\n");
    } else {
        printf("\n\n************ Все тесты прошли *************\n");
    }
    return error_count ? 1 : 0;
}