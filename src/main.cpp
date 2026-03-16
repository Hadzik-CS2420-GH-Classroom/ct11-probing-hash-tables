// =============================================================================
// CT11: Probing Hash Table — Grade Book System
// =============================================================================
//
// - Scenario: a professor manages student grades using a probing hash table
// - Student names (strings) are KEYS, integer grades are VALUES
// - Linear probing resolves collisions by scanning forward to the next open slot
// - Removed entries become DELETED tombstones to preserve probe sequences
//

#include <iostream>
#include <string>
#include "ProbingHashTable.h"

int main() {
    std::cout << "=== CT11: Probing Hash Tables ===\n";
    std::cout << "=== Scenario: Professor's Grade Book ===\n\n";

    ProbingHashTable grade_book;
    std::cout << "Empty grade book: size=" << grade_book.size()
              << ", capacity=" << grade_book.capacity() << "\n\n";

    // -----------------------------------------------------------------------
    // TODO 1: Insert five student grades into the probing table
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Same five students as CT10 — but now collisions are
    //   resolved by probing forward instead of chaining.
    //   - when two keys hash to the same index, the second one moves forward
    //   - this creates "clusters" of consecutive occupied slots
    //
    std::cout << "Adding student grades...\n";
    grade_book.insert("Alice",   95);
    grade_book.insert("Bob",     82);
    grade_book.insert("Charlie", 91);
    grade_book.insert("Diana",   78);
    grade_book.insert("Eve",     88);
    std::cout << "  Inserted 5 students (size=" << grade_book.size() << ")\n";

    // -----------------------------------------------------------------------
    // TODO 2: Print the table to see the slot layout (notice clustering)
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Compare this output to CT10's chaining table.
    //   - chaining: collisions form linked lists at the SAME bucket index
    //   - probing: collisions push entries to ADJACENT slots (clustering)
    //   - notice consecutive occupied slots — this is primary clustering
    //
    std::cout << "\nGrade Book contents:\n";
    grade_book.print();
    std::cout << "  Load factor: " << grade_book.load_factor() << "\n";

    // -----------------------------------------------------------------------
    // TODO 3: Search for existing and missing students
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Search probes forward from the hash index.
    //   - for Charlie: hash gives a starting slot, probe until found
    //   - for Frank: probe until hitting an EMPTY slot — not in table
    //
    std::cout << "\nSearching for grades...\n";
    int* grade = grade_book.search("Charlie");
    if (grade) {
        std::cout << "  Charlie's grade: " << *grade << "\n";
    }

    grade = grade_book.search("Frank");
    if (grade) {
        std::cout << "  Frank's grade: " << *grade << "\n";
    } else {
        std::cout << "  Frank: not enrolled\n";
    }

    // -----------------------------------------------------------------------
    // TODO 4: Update Bob's grade (insert with existing key)
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Inserting a duplicate key updates the value — no duplicates.
    //   - same contract as chaining: keys are unique
    //   - insert probes to find Bob's slot, then overwrites the value
    //
    std::cout << "\nBob retook the exam...\n";
    grade_book.insert("Bob", 94);
    grade = grade_book.search("Bob");
    std::cout << "  Bob's updated grade: " << *grade << "\n";

    // -----------------------------------------------------------------------
    // TODO 5: Remove Diana — demonstrate tombstone pattern
    // -----------------------------------------------------------------------
    //
    // ? SEE DIAGRAM: images/tombstone_pattern.png — before/after remove
    //
    // ! DISCUSSION: Diana's slot becomes [deleted] — a tombstone.
    //   - the slot is NOT set to EMPTY — that would break probe sequences
    //   - any key that probed PAST Diana's slot during insert can still be found
    //   - look at the print output: Diana's slot shows [deleted]
    //
    std::cout << "\nDiana dropped the class...\n";
    grade_book.remove("Diana");
    std::cout << "  Size after removal: " << grade_book.size() << "\n";

    std::cout << "\nGrade book after removing Diana:\n";
    grade_book.print();

    // -----------------------------------------------------------------------
    // TODO 6: Search for Eve — probes past Diana's tombstone
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Eve's hash index may be before Diana's old slot.
    //   - search probes forward, hits Diana's [deleted] tombstone
    //   - DELETED means "keep probing" — search continues past the tombstone
    //   - finds Eve at the next OCCUPIED slot
    //   - if we had used EMPTY instead of DELETED, this search would fail!
    //
    std::cout << "\nSearching for Eve (probes past Diana's tombstone)...\n";
    grade = grade_book.search("Eve");
    if (grade) {
        std::cout << "  Eve's grade: " << *grade << " (found!)\n";
    } else {
        std::cout << "  Eve: not found (ERROR — tombstone broken!)\n";
    }

    // -----------------------------------------------------------------------
    // TODO 7: Insert Frank and Grace — trigger resize, tombstones cleared
    // -----------------------------------------------------------------------
    //
    // ? SEE DIAGRAM: images/probing_resize.png — before/after resize
    //
    // ! DISCUSSION: Adding two more students pushes load factor past 0.75.
    //   - before insert: size=4, capacity=7, load factor=0.57
    //   - inserting Frank: (4+1)/7 = 0.71 — still under 0.75, fits
    //   - inserting Grace: (5+1)/7 = 0.86 — exceeds 0.75, resize FIRST
    //   - resize picks next prime after 7*2=14, which is 17
    //   - only OCCUPIED entries are rehashed — Diana's tombstone is cleared
    //   - after resize: clean table, no tombstones, lower load factor
    //
    std::cout << "\nAdding Frank and Grace (will trigger resize)...\n";
    std::cout << "  Before: size=" << grade_book.size()
              << ", capacity=" << grade_book.capacity()
              << ", load_factor=" << grade_book.load_factor() << "\n";

    grade_book.insert("Frank", 73);
    grade_book.insert("Grace", 96);

    std::cout << "  After:  size=" << grade_book.size()
              << ", capacity=" << grade_book.capacity()
              << ", load_factor=" << grade_book.load_factor() << "\n";

    // -----------------------------------------------------------------------
    // TODO 8: Final print showing clean table after resize
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Notice the differences after resize.
    //   - capacity jumped from 7 to 17 (next prime after 14)
    //   - all entries moved to new positions (rehashed with % 17)
    //   - Diana's tombstone is GONE — only OCCUPIED entries were rehashed
    //   - load factor dropped significantly — plenty of room for new entries
    //   - clusters may have broken up — keys spread across more slots
    //
    std::cout << "\nFinal grade book (after resize):\n";
    grade_book.print();
    std::cout << "  Load factor: " << grade_book.load_factor() << "\n";

    std::cout << "\n=== CT11 Complete ===\n";
    return 0;
}
