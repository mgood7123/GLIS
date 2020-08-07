//
// Created by konek on 7/19/2019.
//

#include "../../../include/Windows/Kernel/WindowsAPIKernel.h"

Kernel::Kernel() {
    puts("Kernel constructor");
    fflush(stdout);
    this->table = new WindowsAPITable();
}

Kernel::Kernel(const Kernel &x) {
    puts("Kernel copy constructor");
    fflush(stdout);
    if (table == nullptr) {
        table = new WindowsAPITable();
    }
    if (x.table != nullptr) {
        puts("Kernel copy constructor - copying table");
        fflush(stdout);
        table->table = x.table->table;
        puts("Kernel copy constructor - copied table");
        fflush(stdout);
    }
}

Kernel::Kernel(Kernel &&x) {
    puts("Kernel move constructor");
    fflush(stdout);
}

Kernel & Kernel::operator=(const Kernel &x) const {
    puts("Kernel copy assignment");
    fflush(stdout);
    return *const_cast<Kernel*>(this);
}

Kernel & Kernel::operator=(Kernel &&x) const {
    puts("Kernel move assignment");
    fflush(stdout);
    return *const_cast<Kernel*>(this);
}

Kernel::~Kernel() {
    puts("Kernel Destructor");
    fflush(stdout);
    delete this->table;
}
