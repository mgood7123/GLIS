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
            cout << "atlas constructor" << endl << flush;
        }

        /* Copy constructor */
        atlas(const atlas &p2) {
            puts("atlas copy constructor");
            fflush(stdout);
            size = p2.size;
        }

        /* Move constructor */
        atlas(atlas &&p2) {
            puts("atlas move constructor");
            fflush(stdout);
            size = p2.size;
            p2.size = 0;
        }

        ~atlas () {
            cout << "atlas destructor" << endl << flush;
        }
    };

    typedef std::pair <const char*, ResourceType> ATLAS_PAIR;

    void add_font(const char * id) {
        font.newObject(0, 0, ATLAS_PAIR {id, ResourceType(new Kernel(), true)});
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
                    const char * data = font.table->table[index]->resource.get<ATLAS_PAIR>()->first;
                    if (!memcmp(data, id, resource_len))
                        return font.table->table[index];
                } else {
                    printf("font.table->table[%zu] is nullptr\n", index);
                }
        }
        return nullptr;
    }

    ATLAS_PAIR * get_font(const char * id) {
        auto o = find_font(id);
        if (o == nullptr) return nullptr;
        return o->resource.get<ATLAS_PAIR>();
    }

    void remove_font(const char * id) {
        auto o = find_font(id);
        if (o == nullptr) return;
        font.deleteObject(o);
    }

    Kernel * get_atlas(const char * id) {
        auto o = find_font(id);
        if (o == nullptr) return nullptr;
        return o->resource.get<ATLAS_PAIR>()->second.get<Kernel>();
    }

    void add_font_size(const char * id, int size) {
        Kernel* kernel = get_atlas(id);
        if (kernel == nullptr) return;
        atlas x = atlas();
        x.size = size;
        kernel->newObject(0, 0, x);
    }

    void list_fonts() {
        if (font.table->Page.count() == 0) {
            printf("no fonts have been added\n");
            return;
        }
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr) {
                    if (font.table->table[index]->resource.has_value())
                        printf("%s\n", font.table->table[index]->resource.get<ATLAS_PAIR>()->first);
                    else
                        printf("index %zu has no resource\n", index);
                }
        }
    }

    void list_sizes() {
        if (font.table->Page.count() == 0) {
            printf("no fonts have been added\n");
            return;
        }
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr) {
                    if (font.table->table[index]->resource.has_value()) {
                        ATLAS_PAIR *a = font.table->table[index]->resource.get<ATLAS_PAIR>();
                        Kernel *x = a->second.get<Kernel>();
                        if (x->table->Page.count() == 0) {
                            printf("font %s has no sizes created\n", a->first);
                        } else {
                            int page = 1;
                            size_t index = 0;
                            size_t page_size = x->table->page_size;
                            for (; page <= x->table->Page.count(); page++) {
                                index = ((page_size * page) - page_size);
                                for (; index < page_size * page; index++)
                                    if (x->table->table[index] != nullptr) {
                                        if (x->table->table[index]->resource.has_value())
                                            printf("font %s has a size of %d\n", a->first,
                                                   x->table->table[index]->resource.get<atlas>()->size);
                                        else
                                            printf("index %zu has no resource\n", index);
                                    }
                            }
                        }
                    } else
                        printf("index %zu has no resource\n", index);
                }
        }
    }
};

int main(int argc, char **argv) {
    AnyOpt x = AnyNullOpt; { int y = 5; x = y; } cout << x.get<int>()[0] << endl << flush;
    cout << "end" << endl << flush;
//    font f;
//    f.add_font("f1");
//    f.add_font("f2");
//    f.add_font("f3");
//    f.add_font_size("f1", 12);
//    f.add_font_size("f1", 16);
//    f.add_font_size("f3", 48);
//    f.list_fonts();
//    f.list_sizes();
//    f.remove_font("f2");
//    f.list_fonts();
//    f.list_sizes();
//    f.remove_font("f1");
//    f.list_fonts();
//    f.list_sizes();
//    f.remove_font("f3");
//    f.list_fonts();
//    f.list_sizes();
    return 0;
}