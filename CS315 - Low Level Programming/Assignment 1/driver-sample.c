#include <stdio.h>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

void sq_root_compute        (unsigned int n);
void sq_root_compute_varargs(unsigned int n1, ...);
void sq_root_compute_array  (int num_of_elements, unsigned int *array_of_elements);

void test0()
{
  printf("sq_root \n");
  sq_root_compute((unsigned int) 0);
  sq_root_compute((unsigned int) 1);
  sq_root_compute((unsigned int) 3);
  sq_root_compute((unsigned int) 16);
  sq_root_compute((unsigned int) 36);
  sq_root_compute((unsigned int) 99810);
}

void test1()
{
    printf("sq_root_compute_varargs \n");

    sq_root_compute_varargs
    (
      (unsigned int) 3,
      (unsigned int) 0
    );
}

void test2()
{
    printf("sq_root_compute_array \n");

    unsigned int a[1] = {3};
    unsigned int b[3] = {25, 169, 9810}; 

    sq_root_compute_array(ARRAY_SIZE(a), a);
    sq_root_compute_array(ARRAY_SIZE(b), b);
}


void test3()
{
    printf("sq_root_compute_varargs \n");

    sq_root_compute_varargs
    (
      (unsigned int) 3,
      (unsigned int) 25,
      (unsigned int) 169,
      (unsigned int) 9810, 
      (unsigned int) 1169,
      (unsigned int) 19810, 
      (unsigned int) 1369,
      (unsigned int) 99810, 
      (unsigned int) 0
    );

}


void test(int i)
{
    switch(i)
    {
        case 0:
          test0();
          break;
        case 1:
          test1();
          break;
        case 2:
          test2();
          break;
        case 3:
          test3();
          break;
        default:    // Run all tests
          test1();
          test2();
          test3();
    }
}

