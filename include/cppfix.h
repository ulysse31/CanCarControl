#ifndef __CPPFIX_H__
# define __CPPFIX_H__

__extension__ typedef int __guard __attribute__((mode (__DI__)));

void	*operator new(size_t size);
void	operator delete(void * ptr);
void	*xmalloc(size_t size);

#endif // ! __CPPFIX_H__
