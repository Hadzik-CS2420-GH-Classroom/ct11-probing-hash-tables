# CT11 — Probing Hash Table Diagrams

Diagrams in the order they appear in the code, from `HashSlot.h` → `ProbingHashTable.h` → `ProbingHashTable.cpp`.

---

## 1. Probing Hash Table — Flat Array of HashSlots
*`HashSlot.h` / `ProbingHashTable.h` — flat array of HashSlots (no chains, no pointers)*

![Probing Hash Table — Flat Array of HashSlots](svgs/probing_table_overview.svg)

---

## 2. SlotStatus — Three States
*`HashSlot.h` — the three states every slot can be in*

![SlotStatus — Three States](svgs/slot_status_enum.svg)

---

## 3. Probing vs. Chaining — Two Memory Layouts
*`ProbingHashTable.h` — why probing uses a flat array instead of pointer chains*

![Probing vs. Chaining — Two Memory Layouts](svgs/probing_vs_chaining.svg)

---

## 4. Cache Locality — Why Probing Is Faster in Practice
*`ProbingHashTable.h` — why contiguous memory beats pointer chasing*

![Cache Locality — Why Probing Is Faster in Practice](svgs/cache_locality.svg)

---

## 5. Load Factor Threshold — Why Resize at 0.75?
*`ProbingHashTable.h` — why probing resizes at 0.75, not 1.0*

![Load Factor Threshold — Why Resize at 0.75?](svgs/load_factor_threshold.svg)

---

## 6. Linear Probing — Insert "Diana"
*`ProbingHashTable.h` / `ProbingHashTable.cpp::insert()` — collision resolved by scanning forward*

![Linear Probing — Insert "Diana"](svgs/linear_probing_insert.svg)

---

## 7. Linear Probing — Search
*`ProbingHashTable.cpp::search()` — found vs. not found, probing past tombstones*

![Linear Probing — Search](svgs/linear_probing_search.svg)

---

## 8. Linear Probing — Remove
*`ProbingHashTable.cpp::remove()` — probe forward to find the key, then mark the slot DELETED*

![Linear Probing — Remove](svgs/linear_probing_remove.svg)

---

## 9. Tombstone Pattern — Why DELETED, Not EMPTY
*`ProbingHashTable.cpp::remove()` — why marking EMPTY instead of DELETED would break search*

![Tombstone Pattern — Why DELETED, Not EMPTY](svgs/tombstone_pattern.svg)

---

## 10. Primary Clustering — Why Probing Slows Down
*`ProbingHashTable.cpp::insert()` — why occupied runs grow and slow things down*

![Primary Clustering — Why Probing Slows Down](svgs/primary_clustering.svg)

---

## 11. Resize — Rehash OCCUPIED, Clear Tombstones
*`ProbingHashTable.cpp::resize()` — before/after growing the table and rehashing all entries*

![Resize — Rehash OCCUPIED, Clear Tombstones](svgs/probing_resize.svg)
