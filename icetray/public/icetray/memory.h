#ifndef MEMORY_H
#define MEMORY_H

#include <cstdlib>
#include <map>
#include <string>

namespace memory {
    /**
     * Get the amount of memory currently allocated for each label
     */
    std::map<std::string, size_t> get_extents();

    /**
     * Set the current label
     */
    void set_label(std::string);
}

#endif // MEMORY_H