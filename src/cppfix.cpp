#include "CanGlobal.h"

void	*xmalloc(size_t size)
{
  void	*ptr;

  if ((ptr = malloc(size)) == 0)
    {
      PRINTCMD("Could Not Allocate Memory, exiting...");
      exit (1);
    }
  return (ptr);
}
/*
int __cxa_guard_acquire(__guard *g)
{
  return !*(char *)(g);
}

void __cxa_guard_release(__guard *g)
{
  *(char *)g = 1;
}

void __cxa_guard_abort (__guard *)
{

}

void * operator new(size_t size)
{
  return xmalloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}
*/
