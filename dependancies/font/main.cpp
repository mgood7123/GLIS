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
        constexpr atlas(const atlas &p2) {
            size = p2.size;
        }

        /* Move constructor */
        constexpr atlas(atlas &&p2) {
            size = p2.size;
            p2.size = 0;
        }

        atlas &operator=(const atlas &x) const {
            puts("atlas copy assignment");
            fflush(stdout);
            const_cast<atlas*>(this)->size = x.size;
            return *const_cast<atlas*>(this);
        }

        atlas &operator=(atlas &&x) const {
            puts("atlas move assignment");
            fflush(stdout);
            std::swap(const_cast<atlas*>(this)->size, x.size);
            return *const_cast<atlas*>(this);
        }

        ~atlas () {
            cout << "atlas destructor" << endl << flush;
        }
    };

    template <typename A, typename B, typename C> class triple {
    public:
        A first;
        B second;
        C third;
        triple(A first, B second, C third) {
            this->first = first;
            this->second = second;
            this->third = third;
        }
    };

    typedef triple <const char*, const char*, ResourceType> ATLAS_PAIR;

    void add_font(const char * id, const char * path) {
        font.newObject(0, 0, ATLAS_PAIR {id, path, ResourceType(new Kernel(), true)});
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
        return o->resource.get<ATLAS_PAIR>()->third.get<Kernel>();
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
                        Kernel *x = a->third.get<Kernel>();
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
    font f;
    f.add_font("f1", "/F1");
    f.add_font_size("f1", 12);
    f.add_font("f2", "/F2");
    f.add_font("f3", "/F3");
    f.add_font_size("f1", 16);
    f.add_font_size("f3", 48);
    f.list_fonts();
    f.list_sizes();
    f.remove_font("f2");
    f.list_fonts();
    f.list_sizes();
    f.remove_font("f1");
    f.list_fonts();
    f.list_sizes();
    f.remove_font("f3");
    f.list_fonts();
    f.list_sizes();
    return 0;
}