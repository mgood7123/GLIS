//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H

#include "../WindowsAPIDefinitions.h"
#include <cstring>

typedef DWORD ObjectType;
typedef DWORD ObjectFlag;

extern const ObjectType ObjectTypeNone;
extern const ObjectType ObjectTypeProcess;
extern const ObjectType ObjectTypeThread;
extern const ObjectType ObjectTypeWindow;

extern const ObjectFlag ObjectFlagNone;
extern const ObjectFlag ObjectFlagAutoDeallocateResource;

class myany {
protected:
    class dummybase {
    public:
        virtual ~dummybase() = default;
    };
    template<typename T> class dummyderive : public dummybase {
    protected:
        T data2;
    public:
        dummyderive(T &&x) : data2(std::forward<T>(x)) {}
    };
public:

    dummybase *data = nullptr;

    template<typename T> myany &operator=(T &&what) {
        data = new dummyderive<T>(std::forward<T>(what));
        return *this;
    }
    ~myany() {
        if (data != nullptr) delete data;
    }
};

typedef std::reference_wrapper<std::unique_ptr<myany>> ResourceType;

typedef class Object {
    public:
        Object();

        ~Object();

        ObjectType type;
        char *name;
        ObjectFlag flags;
        int handles;
        std::unique_ptr<myany> r = std::make_unique<myany>(myany());
        ResourceType resource = r;

        void clean();

        void clean(Object &object);

        void init();

        void init(Object &object);

        Object &operator=(const Object &object);

        void inherit(const Object &object);

        inline bool operator==(const Object &rhs) { return compare(*this, rhs); }

        inline bool operator!=(const Object &rhs) { return !compare(*this, rhs); }

        static bool compare(const Object &lhs, const Object &rhs);

} Object;

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
