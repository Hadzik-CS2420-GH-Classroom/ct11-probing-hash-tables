#pragma once

#include <string>
#include "HashSlot.h"

// ---------------------------------------------------------------------------
// ProbingHashTable — closed hashing / open addressing (linear probing)
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_table_overview.png — flat array of HashSlots
// ? SEE DIAGRAM: images/linear_probing_insert.png — collision resolved by scanning forward
// ? SEE DIAGRAM: images/primary_clustering.png — occupied runs grow and merge
//
// ! DISCUSSION: Probing vs. Chaining — two different memory layouts.
//   - ChainingHashTable uses ChainNode** — an array of pointers to linked lists
//   - ProbingHashTable uses HashSlot* — a flat array of structs (no pointers, no chains)
//   - probing stores everything INSIDE the array itself — better cache locality
//   - trade-off: probing is more sensitive to high load factors (clustering)
//
// ! DISCUSSION: Why a LOWER load factor threshold (0.75 vs 1.0)?
//   - chaining can exceed 1.0 because chains can grow indefinitely
//   - probing CANNOT exceed 1.0 — every entry must fit in the array
//   - as the table fills, probe sequences get longer (primary clustering)
//   - at 0.75 full, average probes are still short (~2-3); at 0.9, they spike
//   - resizing at 0.75 keeps performance close to O(1)
//
// Key design decisions:
//   - std::string keys, int values (simple key-value pairs)
//   - Custom hash function (multiply-and-add with modulo) — same as CT10
//   - Prime table capacity to reduce clustering
//   - MAX_LOAD_FACTOR = 0.75 — resize before the table gets too full
//
class ProbingHashTable {
public:
    explicit ProbingHashTable(int capacity = 7);    // 7 is prime — reduces hash
                                                    // collisions vs a round number
                                                    // like 8 or 10 (see hash())
    ~ProbingHashTable();

    // Rule of 5: no copy or move (dynamic resource, not needed for this CT)
    ProbingHashTable(const ProbingHashTable&) = delete;
    ProbingHashTable& operator=(const ProbingHashTable&) = delete;
    ProbingHashTable(ProbingHashTable&&) = delete;
    ProbingHashTable& operator=(ProbingHashTable&&) = delete;

    void insert(const std::string& key, int value);  // add or update a key-value pair
    int* search(const std::string& key) const;        // find value by key (nullptr if missing)
    bool remove(const std::string& key);              // mark slot as DELETED (tombstone)

    // ! DISCUSSION: const after the parentheses — a promise not to modify the object.
    //   - search, hash, load_factor, print are const — they only READ data members
    //   - insert, remove, resize are NOT const — they CHANGE size_, table_, or capacity_
    //   - the compiler enforces this: a const method cannot modify any data member

    size_t hash(const std::string& key) const;        // convert key to slot index [0, capacity_)
    double load_factor() const;                       // size_ / capacity_ — fraction of table used
    void resize();                                    // grow to next prime capacity and rehash all entries
    void print() const;                               // display each slot's contents for debugging

    int size() const { return size_; }                // total live entries in the table
    int capacity() const { return capacity_; }        // number of slots in the array
    bool is_empty() const { return size_ == 0; }      // true when no entries stored

private:
    // ! DISCUSSION: A flat array of HashSlot structs (not pointers to chains).
    //   - each slot holds a key, value, and status directly in the array
    //   - no heap-allocated nodes, no next pointers — everything is contiguous
    //   - this is why probing has better cache performance than chaining
    HashSlot* table_;           // flat array of HashSlot structs
    int size_;                  // number of OCCUPIED entries (excludes DELETED)
    int capacity_;              // total number of slots

    // ? SEE DIAGRAM: images/probing_resize.png — before/after resize
    // ? load factor = size_ / capacity_ — triggers resize when exceeded
    static constexpr double MAX_LOAD_FACTOR = 0.75;
    static int next_prime(int n);
};
