#include "unique_id_generator.hpp"

int UniqueIDGenerator::current_id = 0;
int UniqueIDGenerator::last_generated_id = 0;

int UniqueIDGenerator::generate() {
    current_id += 1;
    last_generated_id = current_id;
    return current_id;
}
