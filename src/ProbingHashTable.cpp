// =============================================================================
// ProbingHashTable — Linear Probing (Closed Hashing / Open Addressing)
// =============================================================================
//
// - Scenario: a professor's grade book
// - Student names are the KEYS, integer grades are the VALUES
// - When two names hash to the same slot, the entry probes forward
//   to the next open slot (linear probing: index + 1, wrapping around)
// - Removed entries become DELETED tombstones so probe sequences aren't broken
//

#include "ProbingHashTable.h"
#include <iostream>

// =============================================================================
// Helper — next_prime
// =============================================================================
//
// - Returns the smallest prime >= n
// - Used by resize() to pick a new capacity that reduces clustering
//
int ProbingHashTable::next_prime(int n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) ++n;            // start with an odd number
    while (true) {
        bool is_prime = true;
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) { is_prime = false; break; }
        }
        if (is_prime) return n;
        n += 2;
    }
}

// ---------------------------------------------------------------------------
// 1. Constructor
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_table_overview.png — flat array, all slots EMPTY
//
// ! DISCUSSION: The slot array — new HashSlot[capacity_]
//   - new allocates a contiguous array of HashSlot structs on the heap
//   - HashSlot's default constructor sets every slot to EMPTY with key="" and value=0
//   - no () needed here — the default constructor handles initialization
//   - contrast with chaining: ChainNode*[capacity_]() needed explicit zero-init
//
// ! DISCUSSION: Why start with a PRIME capacity (7)?
//   - same reason as chaining: prime has no common factors with hash values
//   - modulo by a prime spreads keys more uniformly across slots
//   - especially important for probing — poor distribution causes clustering
//
ProbingHashTable::ProbingHashTable(int capacity)
    : size_(0), capacity_(capacity) {
    table_ = new HashSlot[capacity_];   // default constructor sets all to EMPTY
}

// ---------------------------------------------------------------------------
// 2. Destructor
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Much simpler than chaining — just delete the array.
//   - chaining needed a double loop: walk each bucket's chain, delete each node
//   - probing has no chains — all data lives directly in the flat array
//   - delete[] frees the entire contiguous block of HashSlots
//   - std::string members in each HashSlot are automatically destroyed
//
ProbingHashTable::~ProbingHashTable() {
    delete[] table_;
}

// ---------------------------------------------------------------------------
// 3. hash() — custom hash function
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/linear_probing_insert.png — hash gives starting index
//
// ! DISCUSSION: Same algorithm as ChainingHashTable — the hash function
//   doesn't care how collisions are resolved.
//   - Step 1 (multiply-and-add): loop through each character, doing
//     hash_value = hash_value * 31 + char — builds a large number unique to this key
//   - Step 2 (modulo): hash_value % capacity_ maps that number into [0, capacity_)
//   - the hash function's job is to pick a STARTING index
//   - what happens next (chaining vs probing) is the collision strategy's job
//
size_t ProbingHashTable::hash(const std::string& key) const {
    // Step 1: multiply-and-add — build a large number from the key
    size_t hash_value = 0;
    for (char c : key) {
        hash_value = hash_value * 31 + c;       // ? each char shifts and adds
    }
    // Step 2: modulo — map into a valid slot index [0, capacity_)
    return hash_value % capacity_;
}

// ---------------------------------------------------------------------------
// 4. insert() — add or update a key-value pair
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/linear_probing_insert.png — probe forward on collision
// ? SEE DIAGRAM: images/primary_clustering.png — why probing gets slower as table fills
//
// ! DISCUSSION: Check load factor BEFORE inserting (not after, like chaining).
//   - probing cannot work when the table is full — there's nowhere to probe to
//   - if we insert first and resize after, we might try to insert into a full table
//   - chaining can insert first because chains can always grow (just add a node)
//   - probing must ensure there's room BEFORE attempting the probe sequence
//
// ! DISCUSSION: Three outcomes when probing forward:
//   - EMPTY or DELETED slot → place the new entry here, set to OCCUPIED
//   - OCCUPIED slot with the SAME key → update the value (no duplicate keys)
//   - OCCUPIED slot with a DIFFERENT key → keep probing: (index + 1) % capacity_
//
void ProbingHashTable::insert(const std::string& key, int value) {
    // Resize BEFORE inserting if load factor would exceed threshold
    if (static_cast<double>(size_ + 1) / capacity_ > MAX_LOAD_FACTOR) {
        resize();
    }

    size_t index = hash(key);

    // ? Linear probe: step forward one slot at a time, wrapping around
    while (true) {
        // ── Case 1: EMPTY or DELETED — place entry here ──
        if (table_[index].status == SlotStatus::EMPTY ||
            table_[index].status == SlotStatus::DELETED) {
            table_[index].key = key;
            table_[index].value = value;
            table_[index].status = SlotStatus::OCCUPIED;
            ++size_;
            return;
        }

        // ── Case 2: OCCUPIED with same key — update value ──
        if (table_[index].key == key) {
            table_[index].value = value;        // key exists — update its value
            return;                             // done, size doesn't change
        }

        // ── Case 3: OCCUPIED with different key — keep probing ──
        index = (index + 1) % capacity_;        // ? wrap around to slot 0 if needed
    }
}

// ---------------------------------------------------------------------------
// 5. search() — find a value by key
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/linear_probing_search.png — found vs. not found
// ? SEE DIAGRAM: images/tombstone_pattern.png — probing past a DELETED tombstone
//
// ! DISCUSSION: Three slot states mean three different actions during search.
//   - OCCUPIED + matching key → FOUND — return pointer to the value
//   - OCCUPIED + different key → keep probing (this slot was taken by someone else)
//   - DELETED → keep probing! (someone WAS here — the key we want might be further)
//   - EMPTY → NOT FOUND — this slot was NEVER used, so the key can't be past it
//
// ! DISCUSSION: Why does DELETED mean "keep going" but EMPTY means "stop"?
//   - when a key was inserted, it probed past OCCUPIED slots until finding an open one
//   - if a slot on that path becomes DELETED, the key is still stored beyond it
//   - but EMPTY means no key was ever inserted past this point — safe to stop
//   - this is the fundamental reason tombstones exist
//
int* ProbingHashTable::search(const std::string& key) const {
    size_t index = hash(key);

    // ? Linear probe: step forward, wrapping around
    while (true) {
        // EMPTY — key was never inserted past this point
        if (table_[index].status == SlotStatus::EMPTY) {
            return nullptr;                     // key not in table
        }

        // OCCUPIED — check if this is our key
        if (table_[index].status == SlotStatus::OCCUPIED &&
            table_[index].key == key) {
            return &table_[index].value;        // found — return pointer to value
        }

        // OCCUPIED (different key) or DELETED — keep probing
        index = (index + 1) % capacity_;        // ? wrap around to slot 0 if needed
    }
}

// ---------------------------------------------------------------------------
// 6. remove() — mark a slot as DELETED (tombstone)
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/tombstone_pattern.png — before/after remove
//
// ! DISCUSSION: Why not set the slot to EMPTY?
//   - setting to EMPTY would break the probe sequence for keys that were
//     inserted AFTER this slot was occupied
//   - example: Alice is at slot 4, Diana at slot 5, Eve at slot 6
//   - if we EMPTY slot 5 (Diana), searching for Eve hashes to 4, probes to 5,
//     sees EMPTY, and incorrectly reports "not found" — Eve is still at 6!
//   - DELETED says "I'm not here anymore, but keep looking — someone might be past me"
//
// ! DISCUSSION: Tombstones accumulate over time, making probe sequences longer.
//   - resize() clears tombstones by only rehashing OCCUPIED entries
//   - this is one reason probing tables resize — not just to add capacity,
//     but to clean up tombstones and shorten probe sequences
//
bool ProbingHashTable::remove(const std::string& key) {
    size_t index = hash(key);

    while (true) {
        // EMPTY — key not in table
        if (table_[index].status == SlotStatus::EMPTY) {
            return false;
        }

        // OCCUPIED + matching key — set tombstone
        if (table_[index].status == SlotStatus::OCCUPIED &&
            table_[index].key == key) {
            table_[index].status = SlotStatus::DELETED;     // tombstone
            --size_;
            return true;
        }

        // OCCUPIED (different key) or DELETED — keep probing
        index = (index + 1) % capacity_;
    }
}

// ---------------------------------------------------------------------------
// 7. load_factor()
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_resize.png — before/after rehash
//
// ! DISCUSSION: load_factor = size / capacity (cast to double!).
//   - for probing, load factor CANNOT exceed 1.0 (every entry is in the array)
//   - we resize at 0.75 (not 1.0 like chaining) because clustering gets
//     exponentially worse as the table fills
//   - at 50% full, average probes ~1.5; at 75% full, ~2.5; at 90% full, ~5.5
//   - 0.75 is the sweet spot: good space utilization without excessive clustering
//
double ProbingHashTable::load_factor() const {
    return static_cast<double>(size_) / capacity_;
}

// ---------------------------------------------------------------------------
// 8. resize()
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_resize.png — before/after rehash
//
// ! DISCUSSION: Resize is O(n) — every OCCUPIED entry must be rehashed.
//   - save the old array, allocate a new larger prime array
//   - rehash only OCCUPIED entries — DELETED tombstones are SKIPPED
//   - this clears all tombstones, giving us a clean table
//   - after resize, probe sequences are shorter (lower load factor, no tombstones)
//
void ProbingHashTable::resize() {
    int old_capacity = capacity_;               // save old size before changing
    HashSlot* old_table = table_;               // save pointer to old array

    capacity_ = next_prime(old_capacity * 2);   // new capacity ~ 2x, always prime
    table_ = new HashSlot[capacity_];           // allocate new empty slot array
    size_ = 0;                                  // reset — insert() will re-increment

    // Rehash only OCCUPIED entries — tombstones (DELETED) are cleared
    for (int i = 0; i < old_capacity; ++i) {
        if (old_table[i].status == SlotStatus::OCCUPIED) {
            insert(old_table[i].key, old_table[i].value);  // rehash with new capacity_
        }
        // ? DELETED and EMPTY slots are simply not copied — tombstones cleared!
    }
    delete[] old_table;                         // free the old slot array
}

// ---------------------------------------------------------------------------
// 9. print() — display each slot's contents
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: print() reveals the internal structure of the probing table.
//   - shows which keys landed in which slots
//   - clustering is visible as consecutive OCCUPIED slots
//   - tombstones appear as [deleted] — helps students see the probe path
//   - after resize, notice: no tombstones, and keys moved to new positions
//
void ProbingHashTable::print() const {
    for (int i = 0; i < capacity_; ++i) {
        std::cout << "  [" << i << "]: ";
        if (table_[i].status == SlotStatus::OCCUPIED) {
            std::cout << "(" << table_[i].key << ", " << table_[i].value << ")";
        } else if (table_[i].status == SlotStatus::DELETED) {
            std::cout << "[deleted]";
        } else {
            std::cout << "[empty]";
        }
        std::cout << "\n";
    }
}
