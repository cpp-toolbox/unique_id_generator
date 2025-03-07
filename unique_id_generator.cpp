#include "unique_id_generator.hpp"

int GlobalUIDGenerator::current_id = 0;
int GlobalUIDGenerator::last_generated_id = 0;

int GlobalUIDGenerator::get_id() {
    current_id += 1;
    last_generated_id = current_id;
    return current_id;
}
