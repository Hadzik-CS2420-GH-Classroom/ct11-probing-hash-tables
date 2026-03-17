#pragma once

#include <string>

// ---------------------------------------------------------------------------
// HashSlot — one entry in a probing hash table
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing_table_overview.png — big picture: flat array of HashSlots
// ? SEE DIAGRAM: images/slot_status_enum.png — the three states: EMPTY, OCCUPIED, DELETED
//
// ! DISCUSSION: Why three states instead of two? (see slot_status_enum diagram)
//   Removing an entry can't leave EMPTY — it would break probe chains.
//   DELETED (tombstone) tells search: "keep going, don't stop here."
//

enum class SlotStatus { EMPTY, OCCUPIED, DELETED };

struct HashSlot {
    std::string key;        // the lookup key (e.g. student name)
    int value;              // the stored data (e.g. grade)
    SlotStatus status;      // tracks whether this slot is empty, occupied, or deleted

    HashSlot() : key{""}, value{0}, status{SlotStatus::EMPTY} {}
};
