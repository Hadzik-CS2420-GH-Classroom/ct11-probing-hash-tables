# CT11 — Implementation Diagrams

Code-block diagrams referenced from `ProbingHashTable.cpp`.

---

## 1. Tombstone Pattern — Why DELETED, Not EMPTY
*`ProbingHashTable.cpp::remove()` — why marking EMPTY instead of DELETED would break search*

![Tombstone Pattern — Why DELETED, Not EMPTY](svgs/tombstone_pattern.svg)

---

## 2. Primary Clustering — Why Probing Slows Down
*`ProbingHashTable.cpp::insert()` — why occupied runs grow and slow things down*

![Primary Clustering — Why Probing Slows Down](svgs/primary_clustering.svg)

---

## 3. Constructor — Allocating the Slot Array
*`ProbingHashTable.cpp::ProbingHashTable()` — code + stack frame + initial heap state after new HashSlot[capacity_]*

![Constructor — Allocating the Slot Array](svgs/constructor_allocation.svg)

---

## 4. insert() — Three Probe Outcomes
*`ProbingHashTable.cpp::insert()` — code alongside the three slot states and what each does*

![insert() — Three Probe Outcomes](svgs/insert_implementation.svg)

---

## 5. search() — Three Slot Outcomes
*`ProbingHashTable.cpp::search()` — code alongside the three slot states: EMPTY stops, OCCUPIED checks key, DELETED keeps probing*

![search() — Three Slot Outcomes](svgs/search_implementation.svg)

---

## 6. remove() — Three Slot Outcomes
*`ProbingHashTable.cpp::remove()` — code alongside the three slot states: EMPTY stops, OCCUPIED marks DELETED, keeps probing past tombstones*

![remove() — Three Slot Outcomes](svgs/remove_implementation.svg)

---

## 7. resize() — Rehash and Clear Tombstones
*`ProbingHashTable.cpp::resize()` — code alongside the three steps: save old, rehash OCCUPIED, free old*

![resize() — Rehash and Clear Tombstones](svgs/resize_implementation.svg)
