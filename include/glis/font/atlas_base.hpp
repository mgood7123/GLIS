template <typename K, typename V> struct atlas_base {
    struct key_map {
        K * key;
        V * value;
    } * keys = nullptr;

    typedef uint64_t __atlasIndexCountType;
    typedef std::optional<__atlasIndexCountType> AtlasIndexCount;
    typedef std::optional<int> AtlasIndex;
    typedef std::optional<K*> AtlasKey;
    typedef std::optional<V*> AtlasValue;

    __atlasIndexCountType index_count = 0;
    
    bool operator== (const atlas_base<K,V> & rhs) {
        puts("==");
        return true;
    }

    bool allocation_increment() {
        if (index_count == UINT64_MAX) return false;
        if (keys == nullptr) {
            keys = new key_map[1];
        } else {
            key_map * keys_tmp = new key_map[index_count+1];
            memcpy(keys_tmp, keys, sizeof(key_map)*index_count);
            delete[] keys;
            keys = keys_tmp;
        }
        index_count++;
        return true;
    }

    void allocation_decrement() {
        if (keys == nullptr) return;
        if (index_count == 1) {
            delete keys;
            keys = nullptr;
        } else {
            key_map *keys_tmp = new key_map[index_count - 1];
            memcpy(keys_tmp, keys, sizeof(key_map) * index_count - 1);
            delete[] keys;
            keys = keys_tmp;
        }
        index_count--;
    }

    __atlasIndexCountType __append(const K & key, const V & value) {
        keys[index_count-1].key = new K {key};
        keys[index_count-1].value = new V {value};
        return index_count-1;
    }

    __atlasIndexCountType __append(const K * key, const V * value) {
        keys[index_count-1].key = const_cast<K*>(key);
        keys[index_count-1].value = const_cast<V*>(value);
        return index_count-1;
    }

    AtlasIndexCount append(const K & key, const V & value) {
        if (!allocation_increment()) return std::nullopt;
        return __append(key, value);
    }

    AtlasIndexCount append(const K * key, const V * value) {
        if (!allocation_increment()) return std::nullopt;
        return __append(key, value);
    }

    bool containsIndex(const AtlasIndex & index) {
        return keys != nullptr && index.has_value() && index_count > index.value();
    }

    bool containsIndex(const int & index) {
        return keys != nullptr && index_count > index;
    }

    AtlasIndexCount indexOfKey(const AtlasKey & key) {
        if (key.has_value() && keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key.value())
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKey(const K & key) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (*keys[i].key == key)
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKey(K * key) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key)
                    return i;
        return std::nullopt;
    }

    bool containsKey(AtlasKey key) {
        return indexOfKey(key).has_value();
    }

    bool containsKey(const K & key) {
        return indexOfKey(key).has_value();
    }

    AtlasIndexCount indexOfValue(const AtlasValue & value) {
        if (value.has_value() && keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].value == value.value())
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfValue(const V & value) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (*keys[i].value == value)
                    return i;
        return std::nullopt;
    }

    bool containsValue(const AtlasValue & value) {
        return indexOfValue(value).has_value();
    }

    bool containsValue(const V & value) {
        return indexOfValue(value).has_value();
    }

    bool containsValue(V * value) {
        return indexOfValue(value).has_value();
    }

    AtlasIndexCount indexOfKeyAndValue(const AtlasKey & key, const AtlasValue & value) {
        if (key.has_value() && value.has_value() && keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key.value() && keys[i].value == value.value())
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKeyAndValue(const K & key, const V & value) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (*keys[i].key == key && *keys[i].value == value)
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKeyAndValue(const K * key, const V * value) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key && keys[i].value == value)
                    return i;
        return std::nullopt;
    }

    bool containsKeyAndValue(const AtlasKey & key, const AtlasValue & value) {
        return indexOfKeyAndValue(key, value).has_value();
    }

    bool containsKeyAndValue(const K & key, const V & value) {
        return indexOfKeyAndValue(key, value).has_value();
    }

    bool containsKeyAndValue(const K * key, const V * value) {
        return indexOfKeyAndValue(key, value).has_value();
    }

    AtlasIndexCount indexOfKeyOrValue(const AtlasKey & key, const AtlasValue & value) {
        printf("const AtlasKey &\n");
        if (key.has_value() && value.has_value() && keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key.value() || keys[i].value == value.value())
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKeyOrValue(const K & key, const V & value) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (*keys[i].key == key || *keys[i].value == value)
                    return i;
        return std::nullopt;
    }

    AtlasIndexCount indexOfKeyOrValue(const K * key, const V * value) {
        if (keys != nullptr)
            for (int i = 0; i < index_count; ++i)
                if (keys[i].key == key || keys[i].value == value)
                    return i;
        return std::nullopt;
    }

    bool containsKeyOrValue(const AtlasKey & key, const AtlasValue & value) {
        return indexOfKeyOrValue(key, value).has_value();
    }

    bool containsKeyOrValue(const K & key, const V & value) {
        return indexOfKeyOrValue(key, value).has_value();
    }

    bool containsKeyOrValue(const K * key, const V * value) {
        return indexOfKeyOrValue(key, value).has_value();
    }

    std::optional<key_map> at(const AtlasIndexCount & index) {
        __atlasIndexCountType idx = index.value();
        if (containsIndex(idx))
            // no need for reference wrappers since key_map members are pointers
            return keys[idx];
        return std::nullopt;
    }

    std::optional<key_map> at(const __atlasIndexCountType & index) {
        if (containsIndex(index))
            // no need for reference wrappers since key_map members are pointers
            return keys[index];
        return std::nullopt;
    }

    std::optional<K*> keyAt(const AtlasIndexCount & index) {
        __atlasIndexCountType idx = index.value();
        if (containsIndex(idx)) return keys[idx].key;
        return std::nullopt;
    }

    std::optional<K*> keyAt(const __atlasIndexCountType & index) {
        if (containsIndex(index))
            return keys[index].key;
        return std::nullopt;
    }

    std::optional<V*> valueAt(const AtlasIndexCount & index) {
        __atlasIndexCountType idx = index.value();
        if (containsIndex(idx)) return keys[idx].value;
        return std::nullopt;
    }

    std::optional<V*> valueAt(const __atlasIndexCountType & index) {
        if (containsIndex(index))
            return keys[index].value;
        return std::nullopt;
    }
};
