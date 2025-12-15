#include "list.h"
DECLARE_LIST(List8);

i32 main(i32 argc, char** argv) {
    auto& list = CreateList8();
    list.InitArgs(argc, argv);
    list.RunAll();
}