//
// Created by konek on 7/18/2019.
//

#include <Windows/Kernel/WindowsAPIObject.h>
#include <Windows/Kernel/WindowsAPITable.h>
#include <Windows/Kernel/WindowsAPIKernel.h>

const ObjectType ObjectTypeNone = 0;
const ObjectType ObjectTypeProcess = 1;
const ObjectType ObjectTypeThread = 2;
const ObjectType ObjectTypeWindow = 3;

const ObjectFlag ObjectFlagNone = 0;
const ObjectFlag ObjectFlagAutoDeallocateResource = 1;

Object *Kernel::newObject(ObjectType type, ObjectFlag flags) {
    return this->table->add(type, flags, AnyNullOpt);
}

void Kernel::deleteObject(Object *object) {
    this->table->remove(object);
}

void Kernel::deleteObject(Object &object) {
    this->table->remove(object);
}

Object::Object() {
    this->init();
}

Object::~Object() {
    this->clean();
}

void Object::clean() {
    this->clean(*this);
}

void Object::clean(Object &object) {
    if (object.name != nullptr) memset(object.name, '\0', strlen(object.name));
    this->init(object);
}

void Object::init() {
    this->init(*this);
}

void Object::init(Object &object) {
    object.name = nullptr;
    object.type = ObjectTypeNone;
    object.flags = 0;
    object.handles = 0;
    object.resource = AnyNullOpt;
}

Object &Object::operator=(const Object &object) {
    this->inherit(object);
    return *this;
}

void Object::inherit(const Object &object) {
    if (object.name == nullptr) this->name = nullptr;
    else strncpy(this->name, object.name, strlen(object.name));
    this->flags = object.flags;
    this->type = object.type;
    this->resource = object.resource;
}

bool compare_const_char(const char * lhs, const char * rhs) {
    if (lhs == nullptr) return rhs == nullptr;
    if (rhs == nullptr) return lhs == nullptr;
    return strcmp(lhs, rhs) == 0;
}

bool Object::compare(const Object &lhs, const Object &rhs) const {
    return (lhs.type == rhs.type) &&
        (lhs.handles == rhs.handles) &&
        (lhs.flags == rhs.flags) &&
        (lhs.resource.data == rhs.resource.data) &&
        compare_const_char(lhs.name, rhs.name);
}

bool Object::operator!=(const Object &rhs) const { return !compare(*this, rhs); }

bool Object::operator==(const Object &rhs) const { return compare(*this, rhs); }
