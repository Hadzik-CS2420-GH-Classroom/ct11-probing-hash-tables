#pragma once

#include <string>

// ---------------------------------------------------------------------------
// HashSlot — one entry in a probing hash table
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_table_overview.png — big picture: flat array of HashSlots
// ? SEE DIAGRAM: images/slot_status_enum.png — the three states: EMPTY, OCCUPIED, DELETED
//
// ! DISCUSSION: Why do we need THREE states instead of just "has data" / "no data"?
//   - EMPTY means this slot has NEVER been used — a probe sequence can stop here
//   - OCCUPIED means this slot holds a live entry — check the key or keep probing
//   - DELETED means this slot USED to hold an entry that was removed (a "tombstone")
//   - if we only had EMPTY and OCCUPIED, removing an entry would leave a gap
//     that breaks probe sequences for keys inserted AFTER the removed one
//   - DELETED tells search: "someone was here — keep probing, don't stop"
//

enum class SlotStatus { EMPTY, OCCUPIED, DELETED };

struct HashSlot {
    std::string key;        // the lookup key (e.g. student name)
    int value;              // the stored data (e.g. grade)
    SlotStatus status;      // tracks whether this slot is empty, occupied, or deleted

    HashSlot() : key{""}, value{0}, status{SlotStatus::EMPTY} {}
};
