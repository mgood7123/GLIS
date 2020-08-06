//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H

#include "../WindowsAPIDefinitions.h"
#include "AnyOpt.h"
#include <cstring>

typedef DWORD ObjectType;
typedef DWORD ObjectFlag;

extern const ObjectType ObjectTypeNone;
extern const ObjectType ObjectTypeProcess;
extern const ObjectType ObjectTypeThread;
extern const ObjectType ObjectTypeWindow;

extern const ObjectFlag ObjectFlagNone;
extern const ObjectFlag ObjectFlagAutoDeallocateResource;

typedef AnyOpt ResourceType;

typedef class Object {
    public:
        Object();

        ~Object();

        ObjectType type;
        char *name;
        ObjectFlag flags;
        int handles;
        ResourceType resource;

        void clean();

        void clean(Object &object);

        void init();

        void init(Object &object);

        Object &operator=(const Object &object);

        void inherit(const Object &object);

        bool operator==(const Object &rhs);

        bool operator!=(const Object &rhs);

        static bool compare(const Object &lhs, const Object &rhs);

} Object;

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
