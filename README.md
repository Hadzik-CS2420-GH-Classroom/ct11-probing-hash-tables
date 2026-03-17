# CT11 — Probing Hash Tables

## Overview

An in-class code-together activity introducing **linear probing (closed hashing / open addressing)** — the second collision resolution strategy after CT10's separate chaining. Students build a `ProbingHashTable` that stores entries directly in a flat array, resolves collisions by scanning forward one slot at a time, and uses **tombstone deletion** to preserve probe sequences. The activity covers load factor monitoring with a lower threshold (0.75 vs chaining's 1.0) and resizing with rehashing that clears tombstones. The scenario continues the professor's grade book: student names are keys, integer grades are values.

## Learning Objectives

- Implement **linear probing (closed hashing)**: collisions resolved by scanning forward to the next open slot in a flat array
- Explain why probing uses a **lower load factor threshold** (0.75 vs 1.0) — primary clustering makes probe sequences longer as the table fills
- Implement **tombstone deletion**: removed entries are marked DELETED rather than EMPTY so probe sequences for other keys are not broken
- Explain the **three slot states** (EMPTY, OCCUPIED, DELETED) and how each affects insert, search, and remove operations
- Implement **resize with tombstone cleanup**: only OCCUPIED entries are rehashed, clearing all tombstones for a clean table
- Compare **probing vs chaining** trade-offs: cache locality vs clustering sensitivity, flat array vs pointer chains

## Files

| File | Focus | TODOs |
|---|---|---|
| `ProbingHashTable.cpp` | Hash function, linear probing insert/search, tombstone remove, resize | 0 (given) |
| `main.cpp` | Grade book demo: insert, search, tombstone remove, resize | 8 |

## Supporting Files

| File | Purpose |
|---|---|
| `HashSlot.h` | `HashSlot` struct (`std::string key`, `int value`, `SlotStatus status`) and `SlotStatus` enum (`EMPTY`, `OCCUPIED`, `DELETED`) |
| `ProbingHashTable.h` | Class with `HashSlot*` flat array, `size_`, `capacity_`, `MAX_LOAD_FACTOR = 0.75` |

## Teaching Order

### 1. `HashSlot.h` — The slot structure

Walk through the HashSlot struct and SlotStatus enum. Explain why three states are needed.

1. **SlotStatus enum** — EMPTY (never used), OCCUPIED (live entry), DELETED (tombstone)
2. **HashSlot struct** — key, value, status fields with default constructor setting EMPTY

### 2. `ProbingHashTable.h` — Class declaration

Compare the header to CT10's ChainingHashTable.h. Highlight the differences.

1. **HashSlot* vs ChainNode\*\*** — flat array of structs vs array of linked list pointers
2. **MAX_LOAD_FACTOR = 0.75** — lower threshold than chaining's 1.0 due to clustering
3. **Same interface** — insert, search, remove, hash, load_factor, resize, print

### 3. `ProbingHashTable.cpp` — Linear probing implementation

Walk through the complete implementation. All code is given — students follow along.

1. **Constructor** — allocate `HashSlot` array; default constructor sets every slot to EMPTY
2. **Destructor** — simple `delete[] table_` (no chains to walk, contrast with chaining)
3. **hash()** — same multiply-and-add algorithm as CT10; hash function doesn't care about collision strategy
4. **insert()** — hash the key; resize BEFORE inserting if load factor would exceed 0.75; probe forward with `(index + 1) % capacity_` until finding EMPTY, DELETED, or matching key
5. **search()** — hash the key; probe forward: OCCUPIED + match = found, DELETED = keep probing, EMPTY = not found
6. **remove()** — find the key; set status to DELETED (tombstone); explain why EMPTY would break probe chains
7. **load_factor()** — same formula, lower threshold; discuss clustering impact at high load factors
8. **resize()** — save old array; allocate new prime-sized array; rehash only OCCUPIED entries (tombstones cleared)
9. **print()** — display each slot: entry, [empty], or [deleted]

### 4. `main.cpp` — Professor's Grade Book (8 TODOs)

1. **Insert students** — add 5 students with grades into the probing table
2. **Print** — display the slot layout; notice primary clustering (consecutive occupied slots)
3. **Search** — look up Charlie (found) and Frank (not found — hits EMPTY)
4. **Update** — update Bob's grade via insert with existing key
5. **Remove** — remove Diana; slot becomes [deleted] tombstone
6. **Search past tombstone** — search for Eve probes past Diana's tombstone
7. **Resize** — insert Frank and Grace; triggers resize at 0.75 threshold; tombstones cleared
8. **Final print** — clean table after resize: no tombstones, lower load factor, new positions

## CCD Coverage

Maps CCD 2.6 (Hash Tables) topics to specific sections in this activity.

| CCD Topic | Where It's Covered |
|---|---|
| **Hash tables** — purpose, structure, key-value mapping | `ProbingHashTable.h` (class declaration), `HashSlot.h` (slot struct), `main.cpp` TODOs 1-3 (grade book usage) |
| **Hash algorithms** — hash functions, modulo mapping, distribution | `ProbingHashTable.cpp` section 3 (hash — same algorithm as CT10), DISCUSSION: hash function is independent of collision strategy |
| **Array based (closed hashing)** — linear probing, quadratic probing | `ProbingHashTable.cpp` sections 4-6 (insert/search/remove with linear probing), `main.cpp` TODOs 1-8 (full probing demo); quadratic probing mentioned in DISCUSSION but not implemented |
| **Optimizing hash tables** — load factor, resizing, choosing hash functions | `ProbingHashTable.cpp` sections 7-8 (load_factor + resize); DISCUSSION covers 0.75 threshold, primary clustering, tombstone cleanup during resize; `main.cpp` TODO 7 (resize demo) |

## Key Concepts

- **Linear probing**: on collision, scan forward one slot at a time (`(index + 1) % capacity`) until finding an open slot; all data lives in the array itself
- **Primary clustering**: consecutive occupied slots grow and merge, making collisions more likely and probe sequences longer
- **Three slot states**: EMPTY (never used, safe to stop), OCCUPIED (live entry), DELETED (tombstone — keep probing past this)
- **Tombstone deletion**: removed entries are marked DELETED, not EMPTY, so probe sequences for other keys remain intact
- **Lower load factor**: probing uses 0.75 threshold (vs chaining's 1.0) because clustering worsens as the table fills
- **Resize clears tombstones**: only OCCUPIED entries are rehashed into the new table — tombstones simply disappear
- **Flat array vs chains**: probing has better cache locality (contiguous memory) but is more sensitive to load factor

## Grading (30 points)

| Category | Points | What is tested |
|---|---|---|
| Build | 2 | Project compiles without errors |
| `hash()` | 3 | Consistent results, values within range |
| Probing insert | 4 | Linear probing finds open slot, handles duplicates, handles collisions |
| Probing search | 4 | Finds existing keys, returns nullptr for missing, handles collisions |
| Probing remove (tombstone) | 5 | Sets tombstone, doesn't break probe sequences, returns false for missing |
| Probing resize | 4 | Triggers on high load, all entries survive, tombstones cleared |
| Tombstone correctness | 5 | Search probes past tombstone, insert reuses tombstone slot, multiple tombstones |
| Print | 3 | Handles empty table, shows entries, shows tombstones |

## Comment Conventions

Uses [Better Comments](https://marketplace.visualstudio.com/items?itemName=OmarRwemi.BetterComments) for VS 2022:

| Prefix | Color | Purpose |
|---|---|---|
| `// !` | Important (red) | `DISCUSSION:` teaching notes for instructor walkthrough |
| `// ?` | Question (blue) | `SEE DIAGRAM:` references to visual aids |
| `// TODO:` | Task (orange) | Student exercises (main branch) |
