#include "list.h"
DECLARE_LIST(List4);

int main() {
    auto& list = CreateList4();
    list.RunAll();
}