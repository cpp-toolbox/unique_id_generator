#ifndef UNIQUE_ID_GENERATOR_HPP
#define UNIQUE_ID_GENERATOR_HPP

/**
 * @brief A class for generating unique IDs.
 * @note This implementation is not thread-safe.
 */
class UniqueIDGenerator {
  public:
    /**
     * @brief Retrieves the next unique ID.
     * @return A unique integer ID.
     */
    static int generate();
    static int last_generated_id;

  private:
    static int current_id; ///< tracks the last generated id.
};

#endif // UNIQUE_ID_GENERATOR_HPP
