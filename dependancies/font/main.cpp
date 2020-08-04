#include <iostream>
#include <memory>
#include <string.h>
#include <assert.h>
#include "WINAPI/SDK/include/Windows/Kernel/WindowsAPIKernel.h"

using namespace std;

struct font {
    Kernel font;

    struct atlas {
        int size = 0;
        atlas () {
            cout << "constructor" << endl;
        }
        ~atlas () {
            cout << "destructor" << endl;
        }
    };

    typedef std::pair <const char*, Kernel *> ATLAS_PAIR;

    void add_font(const char * id) {
        myany tmp;
        tmp = ATLAS_PAIR {id, new Kernel};
        std::unique_ptr<myany> atlasPair = std::make_unique<myany>(tmp);
        font.newObject(0, 0, atlasPair);
    }

    Object * find_font(const char * id) {
        size_t resource_len = strlen(id);
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        printf("font.table->Page.count() is %d\n", font.table->Page.count());
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr) {
                    const char * data = reinterpret_cast<ATLAS_PAIR *>(font.table->table[index]->resource.get().get())->first;
                    printf("testing %s\n", data);
                    if (!memcmp(data, id, resource_len))
                        return font.table->table[index];
                } else {
                    printf("font.table->table[%d] is nullptr\n", index);
                }
        }
        return nullptr;
    }

    ATLAS_PAIR * get_font(const char * id) {
        auto x = find_font(id);
        if (x == nullptr) return nullptr;
        return reinterpret_cast<ATLAS_PAIR *>(x->resource.get().get());
    }

    void remove_font(const char * id) {
        auto o = find_font(id);
//        auto font_ = static_cast<ATLAS_PAIR *>(o->resource);
//        delete font_->second;
        font.deleteObject(o);
    }

    Kernel * get_atlas(const char * id) {
        auto o = find_font(id);
        assert(o != nullptr);
        assert(o->resource.get() != nullptr);
        return reinterpret_cast<ATLAS_PAIR *>(o->resource.get()->data)->second;
    }

    void add_font_size(const char * id, int size) {
        auto atlas_ = get_atlas(id);
        myany tmp;
        tmp = atlas();
        std::unique_ptr<myany> atlas__ = std::make_unique<myany>(tmp);
        reinterpret_cast<atlas*>(atlas__.get())->size = size;
        atlas_->newObject(0, 0, atlas__);
    }

    void list_fonts() {
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr)
                    if (font.table->table[index]->resource.get()->data != nullptr)
                        printf("%s\n", reinterpret_cast<ATLAS_PAIR *>(font.table->table[index]->resource.get().get())->first);
        }
    }

    void list_sizes() {
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr)
                    if (font.table->table[index]->resource.get()->data != nullptr) {
                        ATLAS_PAIR * a = reinterpret_cast<ATLAS_PAIR *>(font.table->table[index]->resource.get().get());
                        Kernel *x = a->second;
                        if (x->table->Page.count() == 0) {
                            printf("font %s has no sizes created\n", a->first);
                        } else {
                            int page = 1;
                            size_t index = 0;
                            size_t page_size = x->table->page_size;
                            for (; page <= x->table->Page.count(); page++) {
                                index = ((page_size * page) - page_size);
                                for (; index < page_size * page; index++)
                                    if (x->table->table[index] != nullptr)
                                        if (x->table->table[index]->resource.get()->data != nullptr)
                                            printf("font %s has a size of %d\n", a->first, reinterpret_cast<atlas *>(x->table->table[index]->resource.get().get())->size);
                            }
                        }
                    }
        }
    }
};

int main(int argc, char **argv) {
    cout << "Hello, world!" << endl;
    font f;
    f.add_font("f1");
    f.add_font("f2");
    f.add_font_size("f1", 12);
    f.list_fonts();
    f.list_sizes();
    f.remove_font("f1");
    f.remove_font("f2");
    return 0;
}
