#include "list.h"
DECLARE_LIST(List1);

int main() {
    auto& list = CreateList1();
    list.RunTask(2);
}