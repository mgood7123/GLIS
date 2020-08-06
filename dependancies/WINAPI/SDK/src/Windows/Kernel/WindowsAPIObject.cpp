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
    if (object.flags & ObjectFlagAutoDeallocateResource) {
        // if resource points to a class that has a destructor, it will not be called
//        delete object.resource;
    }
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

bool Object::compare(const Object &lhs, const Object &rhs) {
    if (lhs.type != rhs.type) return false;
    if (lhs.handles != rhs.handles) return false;
    if (lhs.flags != rhs.flags) return false;
    if (lhs.name == nullptr && rhs.name == nullptr) {}
    if (lhs.name != nullptr && rhs.name != nullptr) {
        if (strcmp(lhs.name, rhs.name) != 0) return false;
    }
    return lhs.resource.data == rhs.resource.data;
}

bool Object::operator!=(const Object &rhs) { return !compare(*this, rhs); }

bool Object::operator==(const Object &rhs) { return compare(*this, rhs); }
