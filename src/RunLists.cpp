#include "list.h"
DECLARE_LIST(List3);

int main() {
    auto& list = CreateList3();
    list.RunAll();
}