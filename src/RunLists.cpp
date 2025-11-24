#include "list.h"
DECLARE_LIST(List5);

int main() {
    auto& list = CreateList5();
    list.RunAll();
}