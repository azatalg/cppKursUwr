#include "list.h"
DECLARE_LIST(List10);

i32 main(i32 argc, char** argv) {
    auto& list = CreateList10();
    list.InitArgs(argc, argv);
    list.RunAll();
}