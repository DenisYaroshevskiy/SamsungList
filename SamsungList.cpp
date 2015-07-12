// SamsungList.cpp : Defines the entry point for the console application.
//


#include "SamsungList.h"
#include <conio.h>

using namespace Samsung;

int main()
{
    List<int> x{ 1, 2, 3, 4, 5 };
    x.dbg_print();
    x.reverse();
    x.dbg_print();
    x.remove(x.get_head().get_next());
    x.dbg_print();
    x.loop(x.get_head().get_next());
    std::cout << std::boolalpha << static_cast<bool>(x.has_loops()) << std::endl;
    x.dbg_print();
    _getch();
	return 0;
}

