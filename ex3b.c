#include "stdio.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <math.h>
#define SIZE 510

//The coefficients of the polynomial
int arr[3] = {0};

//Struct to sent for thread
typedef struct
{
    double a, b;
    int t_arr[3];
    double result;

} integral;

void calculate_integral(double to, double from, int amount);
void error(char *string);
void *calculate_range(void *integ);
void polynomial(char *str);
void free_parameter(char *str, int i, int parmeter);

int main(int argc, char *argv[])
{
    //Initialization variables.
    int i = 0, from = 0, to = 0;
    char input[SIZE];
    char *func;
    int amount = 0;

    //Check if treads number legal.
    sscanf(argv[1], "%d", &amount);
    if ((amount < 0) || (amount == 0))
       error("Illegal Threads Number.\n");

    //Get input from user.
    fgets(input, SIZE, stdin);

    //Separate user input and insert to variables.
    char *token = strtok(input, ",");
    while (token != NULL)
    {
        if (i == 0) //Integral string
        {
            func = (char *)malloc(sizeof(char) * strlen(token));
            strcpy(func, token);
        }
        if (i == 1) //From
            sscanf(token, "%d", &from);
        if (i == 2) //to
            sscanf(token, "%d", &to);
        token = strtok(NULL, ",");
        i++;
    }
    polynomial(func);                     //Analyze integral string, and build arr[].
    calculate_integral(to, from, amount); //Calculate integral with threads.
    return 0;
}

void calculate_integral(double to, double from, int amount)
{
    //Initialization variables, threads array and struct array.
    double range = (to - from) / amount;
    int status;
    double *result;
    double sum = 0;
    pthread_t treads[amount];
    integral inte[amount];

    //Insert values to structs, then run the thread.
    for (int i = 0; i < amount; i++)
    {
        inte[i].a = from + (range * i);
        inte[i].b = inte[i].a + range;
        for (size_t j = 0; j < 3; j++)
            inte[i].t_arr[j] = arr[j];

        status = pthread_create(&treads[i], NULL, calculate_range, (void *)&inte[i]);
        if (status != 0)
            error("pthread_create failed.\n");
    }
    //Wait for all threads and sum the results.
    for (size_t i = 0; i < amount; i++)
    {
        pthread_join(treads[i], (void **)&result);
        sum += *result;
    }

    printf("%0.3f\n", sum);
}
//Tamar error function.
void error(char *string)
{
    perror(string);
    exit(-1);
}
//Calculate the integral in range.
void *calculate_range(void *integ)
{
    integral *s = (integral *)integ;
    double res = (((pow(s->b, 3) * s->t_arr[2]) / 3) + ((pow(s->b, 2) * s->t_arr[1]) / 2) + (s->b * s->t_arr[0])) -
                 (((pow(s->a, 3) * s->t_arr[2]) / 3) + ((pow(s->a, 2) * s->t_arr[1]) / 2) + (s->a * s->t_arr[0]));
    s->result = res;
    return (void *)&s->result;
}
 //Analyze integral string, and build arr[].
void polynomial(char *str)
{
    int parameter, flag = 0, x;
    char temp[SIZE];
    //Check cases.
    if (strstr(str, "^2") != NULL)
        arr[2] = 1;
    if (strstr(str, "^1") != NULL)
        arr[1] = 1;
    if ((arr[1] == 0) && (arr[2] == 0))
    {
        sscanf(str, "%d", &parameter);
        arr[0] = parameter;
    }

    //Since we know how our string looks, we can find the free variable, if exist.
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '+')// "+" is the delimeter.
        {
            if (arr[2] == 1 && flag == 0)
            {
                if (arr[1] == 1)
                {
                    flag = 1;
                    i++;
                    continue;
                }
                //Free variable found, insert to parameter
                int x;
                char temp[SIZE];
                x = strlen(str) - i + 1;
                temp[x + 1] = '\0';
                strncpy(temp, str + i, x);
                sscanf(temp, "%d", &parameter);
                arr[0] = parameter;
                break;
            }
            if (arr[1] == 1)
            {
                int x;
                char temp[SIZE];
                x = strlen(str) - i + 1;
                temp[x + 1] = '\0';
                strncpy(temp, str + i, x);
                sscanf(temp, "%d", &parameter);
                arr[0] = parameter;
                break;
            }
            else
            {
                int x;
                char temp[SIZE];
                x = strlen(str) - i + 1;
                temp[x + 1] = '\0';
                strncpy(temp, str + i, x);
                sscanf(temp, "%d", &parameter);
                arr[0] = parameter;
                break;
            }
        }
        i++;
    }
}
