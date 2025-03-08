#ifndef UNIQUE_ID_GENERATOR_HPP
#define UNIQUE_ID_GENERATOR_HPP

#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <iostream>

class IDGenerator {
  public:
    virtual int get_id() = 0;
    virtual void reclaim_id(int id) = 0;
    virtual ~IDGenerator() {}
};

class UniqueIDGenerator : public IDGenerator {
  public:
    int get_id() override {
        if (!reclaimed_ids.empty()) {
            int id = reclaimed_ids.front();
            reclaimed_ids.pop();
            used_ids.insert(id);
            return id;
        }
        int id = next_id++;
        used_ids.insert(id);
        return id;
    }

    void reclaim_id(int id_value) override {
        if (used_ids.find(id_value) == used_ids.end()) {
            throw std::invalid_argument("Invalid or already reclaimed ID");
        }
        used_ids.erase(id_value);
        reclaimed_ids.push(id_value);
    }

    std::vector<int> get_used_ids() const { return std::vector<int>(used_ids.begin(), used_ids.end()); }

    friend std::ostream &operator<<(std::ostream &os, const UniqueIDGenerator &generator) {
        os << "Used IDs: [";
        std::vector<int> ids = generator.get_used_ids();
        for (size_t i = 0; i < ids.size(); ++i) {
            os << ids[i];
            if (i < ids.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    int next_id = 0;
    std::unordered_set<int> used_ids;
    std::queue<int> reclaimed_ids;
};

class BoundedUniqueIDGenerator : public IDGenerator {
  public:
    explicit BoundedUniqueIDGenerator(int max_value) : max_value(max_value), next_id(0) {
        if (max_value <= 0) {
            throw std::invalid_argument("max_value must be greater than 0");
        }

        for (int i = 0; i < max_value; ++i) {
            available_ids.push(i);
        }
    }

    int get_id() override {
        if (available_ids.empty()) {
            throw std::runtime_error("Maximum ID limit reached");
        }

        int id = available_ids.front();
        available_ids.pop();
        used_ids.insert(id);
        return id;
    }

    void reclaim_id(int id_value) override {
        if (id_value < 0 || id_value >= max_value || used_ids.find(id_value) == used_ids.end()) {
            throw std::invalid_argument("Invalid or already reclaimed ID");
        }

        used_ids.erase(id_value);
        available_ids.push(id_value);
    }

    std::vector<int> get_free_ids() const {
        std::vector<int> free_ids;
        std::queue<int> temp_queue = available_ids;
        while (!temp_queue.empty()) {
            free_ids.push_back(temp_queue.front());
            temp_queue.pop();
        }
        return free_ids;
    }

    std::vector<int> get_used_ids() const { return std::vector<int>(used_ids.begin(), used_ids.end()); }

    double get_used_percentage() const { return (static_cast<double>(used_ids.size()) / max_value) * 100.0; }

    friend std::ostream &operator<<(std::ostream &os, const BoundedUniqueIDGenerator &generator) {
        os << "Used IDs: [";
        std::vector<int> ids = generator.get_used_ids();
        for (size_t i = 0; i < ids.size(); ++i) {
            os << ids[i];
            if (i < ids.size() - 1) {
                os << ", ";
            }
        }
        os << "] | Used: " << generator.get_used_percentage() << "%";
        return os;
    }

  private:
    int max_value;
    int next_id;
    std::queue<int> available_ids;
    std::unordered_set<int> used_ids;
};

/**
 * @brief A class for generating unique IDs.
 * @note This implementation is not thread-safe.
 */
class GlobalUIDGenerator {
  public:
    /**
     * @brief Retrieves the next unique ID.
     * @return A unique integer ID.
     */
    static int get_id();
    static int last_generated_id;

  private:
    static int current_id; ///< tracks the last generated id.
};

#endif // UNIQUE_ID_GENERATOR_HPP
