#ifndef UNIQUE_ID_GENERATOR_HPP
#define UNIQUE_ID_GENERATOR_HPP

#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <limits>

#include "sbpt_generated_includes.hpp"

class IdGenerator {
  private:
    unsigned int next_id = 0;
    std::unordered_set<unsigned int> used_ids;
    std::queue<unsigned int> reclaimed_ids;

  public:
    LogSection::LogMode log_mode = LogSection::LogMode::inherit;

  public:
    unsigned int get_id() {
        GlobalLogSection _("get_id");

        unsigned int id;

        if (!reclaimed_ids.empty()) {
            global_logger->debug("we are using a reclaimed id");
            id = reclaimed_ids.front();
            reclaimed_ids.pop();
        } else {
            global_logger->debug("we are incrementing to get an id");
            id = next_id++;
            // Wrap-around safety check
            if (next_id == std::numeric_limits<unsigned int>::max()) {
                global_logger->warn("we hit the numerical max id");
                next_id = 0;
            }
        }

        global_logger->debug("generated id: {}", id);

        used_ids.insert(id);
        return id;
    }

    void reclaim_id(unsigned int id) {
        GlobalLogSection _("reclaim_id");
        global_logger->debug("reclaiming id: {}", id);
        auto it = used_ids.find(id);
        if (it != used_ids.end()) {
            used_ids.erase(it);
            reclaimed_ids.push(id);
        }
    }

    bool is_used(unsigned int id) const { return used_ids.find(id) != used_ids.end(); }
};

// everything below is deprecated but existings for legacy reasons.

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
            throw std::invalid_argument("Invalid or already reclaimed ID: " + std::to_string(id_value));
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

    std::string to_string() const {
        std::ostringstream ss;
        ss << "Used IDs: [";
        std::vector<int> ids = get_used_ids();
        for (size_t i = 0; i < ids.size(); ++i) {
            ss << ids[i];
            if (i < ids.size() - 1) {
                ss << ", ";
            }
        }
        ss << "] | Used: " << get_used_percentage() << "%";
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const BoundedUniqueIDGenerator &generator) {
        return os << generator.to_string();
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
