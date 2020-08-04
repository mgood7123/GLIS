#include "atlas_base.hpp"

template <typename K, typename KSUB, typename VSUB> struct atlas : atlas_base<K,atlas_base<KSUB,VSUB>> {
    typedef uint64_t __atlasIndexCountType;
    typedef std::optional<__atlasIndexCountType> AtlasIndexCount;
    typedef std::optional<int> AtlasIndex;
    typedef std::optional<K*> AtlasKey;
    typedef std::optional<VSUB*> AtlasValue;

    AtlasIndexCount indexOfKeyOrValue(const K & key, const atlas_base<KSUB,VSUB> & value) {
        auto a = this->indexOfKey(key);
        if (a.has_value()) return a;
        if (this->keys != nullptr)
            for (int i = 0; i < this->index_count; ++i)
                if (value == value)
                    return i;
        return std::nullopt;
    }
    
    AtlasIndexCount indexOfKeyOrValue(const K & key, const VSUB & value) {
        if (this->keys != nullptr)
            for (int i = 0; i < this->index_count; ++i)
                if (*this->keys[i].key == key) return i;
                else if (this->keys[i].value->indexOfKey(key).has_value())
                    return i;
        return std::nullopt;
    }
};
