#include <glis/internal/context.hpp>

void GLIS_CLASS::setStructors(void (*constructor)(), void (*destructor)()) {
    this->constructor = constructor;
    this->destructor = destructor;
}
