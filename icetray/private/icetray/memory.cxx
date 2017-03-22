#include <cstdlib>
#include <cassert>
#include <map>
#include <stdint.h>
#include <cstring>
#include <string>
#include <new>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "memory.h"

namespace {
    const size_t MAX_STRING_LENGTH = 1024;
}

namespace memory_map {
    /**
     * djb2 hash function. good for strings
     */
    size_t
    djb2_hash(const char* str)
    {
        char* s = (char*)str;
        size_t hash = 5381;
        while (*s != '\0')
        {
            hash = ((hash << 5) + hash) + *s;
            s++;
        }

        return hash;
    }

    /**
     * A bucket for the MemoryMap
     */
    struct Bucket
    {
        char* key;
        size_t value;
    };

    /**
     * Implement a basic map to store overall memory tracking info.
     *
     * Uses only malloc() and free() to avoid new/delete
     * entangelement with the rest of memory tracking.
     *
     * Key = char*
     * Value = size_t
     */
    class MemoryMap
    {
      public:
        MemoryMap();
        ~MemoryMap();

        // add memory
        void add(const char*, size_t);

        // subtract memory
        void subtract(const char*, size_t);

        // export to a regular map
        std::map<std::string, size_t> get_extents();

      private:
        /**
         * Use open addressing hash table behind the scenes
         */
        Bucket* buckets;
        size_t num_buckets;
        boost::mutex lock;

        // resize the map - not thread-safe
        void resize_buckets(size_t new_size);

        // find a bucket - not thread-safe
        Bucket& find_bucket(const char*);

    };

    // public methods

    MemoryMap::MemoryMap() : num_buckets(1024)
    {
        buckets = (Bucket*)calloc(num_buckets,sizeof(Bucket));
    }

    MemoryMap::~MemoryMap()
    {
        // remember to clear the key of the active buckets
        while(num_buckets > 0)
        {
            num_buckets--;
            if (buckets[num_buckets].key != NULL)
                free(buckets[num_buckets].key);
        }
        free(buckets);
    }

    void
    MemoryMap::add(const char* str, size_t s)
    {
        boost::lock_guard<boost::mutex> guard(lock);
        Bucket& b = find_bucket(str);
        if (b.key == NULL)
        {
            // handle the case of the new bucket
            b.key = strdup(str);
            b.value = s;
        }
        else
            b.value += s;
    }

    void
    MemoryMap::subtract(const char* str, size_t s)
    {
        boost::lock_guard<boost::mutex> guard(lock);
        Bucket& b = find_bucket(str);
        assert(b.key != NULL);
        b.value -= s;
    }

    std::map<std::string,size_t>
    MemoryMap::get_extents()
    {
        std::string s;
        std::map<std::string,size_t> ret;
        boost::lock_guard<boost::mutex> guard(lock);

        for (size_t i=0;i<num_buckets;i++)
        {
            if (buckets[i].key != NULL)
            {
                s.assign(buckets[i].key);
                ret[s] = buckets[i].value;
            }
        }

        return ret;
    }

    // private methods

    void
    MemoryMap::resize_buckets(size_t new_size)
    {
        // make new buckets
        Bucket* new_buckets = (Bucket*)calloc(new_size,sizeof(Bucket));
        assert(new_buckets != NULL);

        // copy over old data
        char* str;
        size_t hash;
        for(;num_buckets > 0;num_buckets--)
        {
            // find the new bucket location
            str = buckets[num_buckets].key;
            if (str == NULL)
                continue;
            hash = djb2_hash(str)%new_size;
            while(new_buckets[hash].key != NULL &&
                  strncmp(str,new_buckets[hash].key,MAX_STRING_LENGTH) != 0)
            {
                hash++;
                if (hash >= new_size)
                    hash = 0;
            }
            // copy the bucket over
            memcpy(&new_buckets[hash],&buckets[num_buckets],sizeof(Bucket));
        }

        // delete old buckets
        free(buckets);

        // swap class data
        num_buckets = new_size;
        buckets = new_buckets;
    }

    Bucket&
    MemoryMap::find_bucket(const char* str)
    {
        size_t hash = djb2_hash(str)%num_buckets;
        size_t orig_hash = hash;

        while(buckets[hash].key != NULL &&
              strncmp(str,buckets[hash].key,MAX_STRING_LENGTH) != 0)
        {
            hash++;
            if (hash >= num_buckets)
                hash = 0;
            if ((num_buckets-hash-orig_hash)*100/num_buckets > 50)
            {
                // we've traversed over half the buckets
                // trigger a resize event
                resize_buckets(num_buckets*2);
                orig_hash = hash = djb2_hash(str)%num_buckets;
            }
        }

        return buckets[hash];
    }
} // namespace memory_map

namespace pointer_map {
    /**
     * Knuth's Multiplicative Method hash function. Good for pointers
     */
    size_t
    pointer_hash(const void* p)
    {
        // multiply by golden ratio of pointer size
#if INTPTR_MAX == INT64_MAX
        return ((uint64_t)p) * 11400714819323198485ull;
#elif INTPTR_MAX == INT32_MAX
        return ((uint32_t)p) * 2654435761u;
#elif INTPTR_MAX == INT16_MAX
        return ((uint16_t)p) * 106039u;
#else
#error Unknown pointer size
#endif
    }

    /**
     * Hold the label and size for a memory allocation
     */
    struct MemData
    {
        char* str;
        size_t size;
    };

    /**
     * A bucket for the PointerMap
     */
    struct PointerBucket
    {
        PointerBucket* next;
        void* key;
        MemData value;
    };

    /**
     * Implement a basic map to store tracking on each allocation.
     *
     * Uses only malloc() and free() to avoid new/delete
     * entangelement with the rest of memory tracking.
     *
     * Key = void*
     * Value = MemData
     */
    class PointerMap
    {
      public:
        PointerMap();
        ~PointerMap();

        // push a value (add to map)
        void push(const void*, MemData);

        // pop a value (remove and get data)
        MemData pop(const void*);

      private:
        /**
         * Use separate chaining hash table behind the scenes
         */
        PointerBucket** buckets;
        size_t num_buckets;
        int64_t allocs_since_resize;
        boost::mutex lock;

        // resize the map - not thread safe
        void resize_buckets(size_t new_size);
    };

    // public methods

    PointerMap::PointerMap() : num_buckets(1024), allocs_since_resize(0)
    {
        buckets = (PointerBucket**)calloc(num_buckets,sizeof(PointerBucket*));
    }

    PointerMap::~PointerMap()
    {
        // remember to clear the key of the active buckets
        PointerBucket* b;
        PointerBucket* b_next;
        for(size_t i=0;i<num_buckets;i++)
        {
            b = buckets[i];
            while (b != NULL)
            {
                b_next = b->next;
                if (b->value.str != NULL)
                    free(b->value.str);
                free(b);
                b = b_next;
            }
        }
        free(buckets);
    }

    void
    PointerMap::push(const void* k, MemData data)
    {
        size_t hash = pointer_hash(k)%num_buckets;
        PointerBucket* b = (PointerBucket*)malloc(sizeof(PointerBucket));
        boost::lock_guard<boost::mutex> guard(lock);

        b->next = buckets[hash];
        b->key = const_cast<void*>(k);
        b->value = data;
        buckets[hash] = b;
        allocs_since_resize++;
        if (allocs_since_resize > int64_t(num_buckets))
            resize_buckets(num_buckets*2);
    }

    MemData
    PointerMap::pop(const void* k)
    {
        size_t hash = pointer_hash(k)%num_buckets;
        MemData ret;
        boost::lock_guard<boost::mutex> guard(lock);

        PointerBucket* b = buckets[hash];
        if (b == NULL)
        {
            // not found
            ret.str = NULL;
        }
        else if (b->key == k)
        {
            ret = b->value;
            buckets[hash] = b->next;
            allocs_since_resize--;
        }
        else
        {
            PointerBucket* b_prev = b;
            b = b->next;
            while (b != NULL)
            {
                if (b->key == k)
                {
                    ret = b->value;
                    b_prev->next = b->next;
                    allocs_since_resize--;
                    break;
                }
                b = b->next;
            }
            if (b == NULL)
                ret.str = NULL;
        }
        if (allocs_since_resize*(-4)/3 > int64_t(num_buckets))
            resize_buckets(num_buckets/2);
        return ret;
    }

    // private methods

    void
    PointerMap::resize_buckets(size_t new_size)
    {
        // make new buckets
        PointerBucket** new_buckets = (PointerBucket**)calloc(new_size,sizeof(PointerBucket*));
        assert(new_buckets != NULL);

        // copy over old data
        size_t hash;
        PointerBucket* b;
        PointerBucket* b_next;
        for(size_t i=0;i<num_buckets;i++)
        {
            b = buckets[i];
            while (b != NULL)
            {
                b_next = b->next;
                // move bucket
                hash = pointer_hash(b->key)%new_size;
                b->next = new_buckets[hash];
                new_buckets[hash] = b;
                // go to next bucket
                b = b_next;
            }
        }

        // delete old buckets
        free(buckets);

        // swap class data
        num_buckets = new_size;
        buckets = new_buckets;
    }
} // namespace pointer_map

namespace memory {

    // default label
    char* default_label = NULL;
    boost::mutex lock;

    // memory maps
    memory_map::MemoryMap tracking_data;
    pointer_map::PointerMap pointers;

    void*
    malloc_override(size_t size)
    {
        // skip tracking if we've disabled it
        if (default_label == NULL)
            return malloc(size);

        // allocate the memory
        void* p = malloc(size);
        if (p == NULL)
            return NULL;

        // save pointer info
        boost::lock_guard<boost::mutex> guard(lock);
        pointer_map::MemData d;
        d.str = strdup(default_label);
        d.size = size;
        pointers.push(p,d);

        // update tracking data
        tracking_data.add(default_label, size);

        return p;
    }

    void
    free_override(void* p)
    {
        if (p == NULL)
            return;

        // get pointer data
        pointer_map::MemData d = pointers.pop(p);
        if (d.str != NULL)
            tracking_data.subtract(d.str, d.size);

        // free the memory
        free(p);
    }

    // public methods

    std::map<std::string, size_t>
    get_extents()
    {
        boost::lock_guard<boost::mutex> guard(lock);
        // don't track us while we make the extent map
        char* old_label = default_label;
        default_label = NULL;

        std::map<std::string, size_t> ret = tracking_data.get_extents();

        default_label = old_label;
        return ret;
    }

    void
    set_label(std::string l)
    {
        boost::lock_guard<boost::mutex> guard(lock);
        if (default_label != NULL)
            free(default_label);
        default_label = strdup(l.c_str());
    }

} // namespace memory


// override the global new and delete operators

#ifdef MEMORY_TRACKING

void*
operator new(size_t size)
{
    void* p = memory::malloc_override(size);
    if (p == NULL)
        throw std::bad_alloc();
    return p;
}

void
operator delete(void* p) throw()
{
    memory::free_override(p);
}

void*
operator new[](size_t size)
{
    void* p = memory::malloc_override(size);
    if (p == NULL)
        throw std::bad_alloc();
    return p;
}

void
operator delete[](void* p) throw()
{
    memory::free_override(p);
}

#endif
