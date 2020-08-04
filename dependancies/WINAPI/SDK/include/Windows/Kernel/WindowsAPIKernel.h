//
// Created by konek on 7/19/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H

#include "../WindowsAPIDefinitions.h"
#include "WindowsAPIHandle.h"
#include "WindowsAPIObject.h"
#include "WindowsAPITable.h"
#include <cassert>
#include <cstdint>

class Kernel {
    public:
        Kernel();

        ~Kernel();

        static const DWORD FLAG_AUTO_DEALLOCATE_RESOURCE = 1;

        WindowsAPITable *table = nullptr;

        bool validateHandle(HANDLE hObject);

        HANDLE newHandle(ObjectType type);

        HANDLE newHandle(ObjectType type, ResourceType resource);

        Handle *getHandle(HANDLE handle);

        Object *newObject(ObjectType type, ObjectFlag flags);

        Object *newObject(ObjectType type, ObjectFlag flags, ResourceType resource);

        void deleteObject(Object *object);

        void deleteObject(Object &object);

        pid PID_LAST = 0;

};

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H
