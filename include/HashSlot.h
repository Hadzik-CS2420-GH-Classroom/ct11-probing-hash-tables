#pragma once

#include <string>

// ---------------------------------------------------------------------------
// HashSlot — one entry in a probing hash table
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: Probing Hash Table — Flat Array of HashSlots  →  images/header_diagrams.md
// ? SEE DIAGRAM: SlotStatus — Three States                     →  images/header_diagrams.md
//
// ! DISCUSSION: Why three states instead of two? (see "Slot Status Enum" in images/header_diagrams.md)
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
