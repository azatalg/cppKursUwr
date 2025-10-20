#include "list.h"
DECLARE_LIST(List2);

int main() {
    auto& list = CreateList2();
    list.RunAll();
}