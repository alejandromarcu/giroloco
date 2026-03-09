#include <alloc.h>
#include <stdio.h>
#include <conio.h>

class T {
  int magnitude [10000];
  int starfunc(void);
};

void ps ()
{
struct heapinfo hi;
hi.ptr = NULL;
printf( "   Size   Status\n" );
printf( "   ----   ------\n" );
while( heapwalk( &hi ) == _HEAPOK )
{
printf( "%7u", hi.size);
if (hi.in_use) printf ("usada\n");
else printf ("libre\n");
}
//    %s\n", hi.size, hi.in_use ? "used" : "free" );
printf ("\n\n");
}

void main ()
{
	T *t;
     T *a;
     void *b;
     clrscr ();
     printf ("%ld\n", farcoreleft ());
     ps ();
     t = new T;
     a = new T;
     b = malloc (100);
     printf ("%ld\n", farcoreleft ());
     ps ();
//     free (b);
     delete t;
     delete a;
     printf ("%ld\n", farcoreleft ());
     ps ();

}