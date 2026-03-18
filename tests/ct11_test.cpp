#include <gtest/gtest.h>
#include "ProbingHashTable.h"

// =============================================================================
// Hash Function Tests (3 points)
// =============================================================================

TEST(HashFunction, ConsistentResults) {
    ProbingHashTable table;
    size_t h1 = table.hash("Alice");
    size_t h2 = table.hash("Alice");
    EXPECT_EQ(h1, h2) << "Same key must always produce the same hash";
}

TEST(HashFunction, WithinRange) {
    ProbingHashTable table;   // capacity = 7
    std::string keys[] = {"Alice", "Bob", "Charlie", "Diana", "Eve",
                          "Frank", "Grace", "Hank", "Ivy", "Jack"};
    for (const auto& key : keys) {
        size_t h = table.hash(key);
        EXPECT_GE(h, 0u);
        EXPECT_LT(h, 7u) << "Hash of \"" << key << "\" out of range";
    }
}

TEST(HashFunction, DifferentKeysCanDiffer) {
    ProbingHashTable table;
    // Not all keys will differ with capacity 7, but "Alice" and "Bob" should
    // This test just verifies the hash function isn't returning a constant
    size_t h1 = table.hash("Alice");
    size_t h2 = table.hash("Bob");
    size_t h3 = table.hash("Charlie");
    // At least two of the three should differ
    EXPECT_TRUE(h1 != h2 || h1 != h3 || h2 != h3)
        << "Hash function appears to return the same value for all keys";
}

// =============================================================================
// Probing — Insert Tests (4 points)
// =============================================================================

TEST(ProbingInsert, BasicInsert) {
    ProbingHashTable table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.is_empty());

    table.insert("Alice", 95);
    EXPECT_EQ(table.size(), 1);
    EXPECT_FALSE(table.is_empty());

    table.insert("Bob", 82);
    table.insert("Charlie", 91);
    EXPECT_EQ(table.size(), 3);
}

TEST(ProbingInsert, UpdateExistingKey) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.insert("Alice", 99);     // update, not a second entry
    EXPECT_EQ(table.size(), 1);

    int* val = table.search("Alice");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 99);
}

TEST(ProbingInsert, HandlesCollisionsViaProbing) {
    ProbingHashTable table;
    // Insert multiple entries — some will collide and probe forward
    table.insert("Alice", 95);
    table.insert("Diana", 78);
    table.insert("Eve", 88);
    EXPECT_EQ(table.size(), 3);

    // All should be searchable despite potential collisions
    int* a = table.search("Alice");
    int* d = table.search("Diana");
    int* e = table.search("Eve");
    ASSERT_NE(a, nullptr);
    ASSERT_NE(d, nullptr);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(*a, 95);
    EXPECT_EQ(*d, 78);
    EXPECT_EQ(*e, 88);
}

TEST(ProbingInsert, ManyInserts) {
    ProbingHashTable table;
    // Insert enough entries to trigger resize
    table.insert("A", 1);
    table.insert("B", 2);
    table.insert("C", 3);
    table.insert("D", 4);
    table.insert("E", 5);
    table.insert("F", 6);
    EXPECT_EQ(table.size(), 6);

    // All entries should be searchable
    for (int i = 0; i < 6; ++i) {
        std::string key(1, 'A' + i);
        int* val = table.search(key);
        ASSERT_NE(val, nullptr) << "Key \"" << key << "\" not found";
        EXPECT_EQ(*val, i + 1);
    }
}

// =============================================================================
// Probing — Search Tests (4 points)
// =============================================================================

TEST(ProbingSearch, FindsExistingKey) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.insert("Bob", 82);
    table.insert("Charlie", 91);

    int* val = table.search("Bob");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 82);
}

TEST(ProbingSearch, ReturnsNullForMissing) {
    ProbingHashTable table;
    table.insert("Alice", 95);

    EXPECT_EQ(table.search("Frank"), nullptr);
    EXPECT_EQ(table.search(""), nullptr);
}

TEST(ProbingSearch, EmptyTable) {
    ProbingHashTable table;
    EXPECT_EQ(table.search("Alice"), nullptr);
}

TEST(ProbingSearch, FindsAfterCollision) {
    ProbingHashTable table;
    // Insert entries that may collide — search should still find them
    table.insert("Alice", 95);
    table.insert("Bob", 82);
    table.insert("Charlie", 91);
    table.insert("Diana", 78);
    table.insert("Eve", 88);

    // All five should be findable
    EXPECT_NE(table.search("Alice"), nullptr);
    EXPECT_NE(table.search("Bob"), nullptr);
    EXPECT_NE(table.search("Charlie"), nullptr);
    EXPECT_NE(table.search("Diana"), nullptr);
    EXPECT_NE(table.search("Eve"), nullptr);
}

// =============================================================================
// Probing — Remove Tests (5 points)
// =============================================================================

TEST(ProbingRemove, SetsTombstone) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.insert("Bob", 82);

    EXPECT_TRUE(table.remove("Alice"));
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.search("Alice"), nullptr);

    // Bob should still be there
    int* val = table.search("Bob");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 82);
}

TEST(ProbingRemove, DoesNotBreakSearch) {
    ProbingHashTable table;
    // Insert entries that form a probe chain
    table.insert("Alice", 95);
    table.insert("Diana", 78);
    table.insert("Eve", 88);

    // Remove Diana (middle of potential probe chain)
    EXPECT_TRUE(table.remove("Diana"));

    // Eve should still be findable — search probes past tombstone
    int* val = table.search("Eve");
    ASSERT_NE(val, nullptr) << "Eve lost after removing Diana — tombstone broken!";
    EXPECT_EQ(*val, 88);

    // Alice should still be findable
    val = table.search("Alice");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 95);
}

TEST(ProbingRemove, ReturnsFalseForMissing) {
    ProbingHashTable table;
    table.insert("Alice", 95);

    EXPECT_FALSE(table.remove("Frank"));
    EXPECT_EQ(table.size(), 1);
}

TEST(ProbingRemove, RemoveFromEmptyTable) {
    ProbingHashTable table;
    EXPECT_FALSE(table.remove("Alice"));
    EXPECT_EQ(table.size(), 0);
}

TEST(ProbingRemove, RemoveThenInsertSameKey) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    EXPECT_TRUE(table.remove("Alice"));
    EXPECT_EQ(table.size(), 0);

    // Re-insert the same key — should reuse the tombstone slot
    table.insert("Alice", 99);
    EXPECT_EQ(table.size(), 1);

    int* val = table.search("Alice");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 99);
}

// =============================================================================
// Probing — Resize Tests (4 points)
// =============================================================================

TEST(ProbingResize, TriggersOnHighLoadFactor) {
    ProbingHashTable table;   // capacity 7, max load factor 0.75
    EXPECT_EQ(table.capacity(), 7);

    // Insert 5 entries: 5/7 = 0.71, still under 0.75
    table.insert("A", 1);
    table.insert("B", 2);
    table.insert("C", 3);
    table.insert("D", 4);
    table.insert("E", 5);
    EXPECT_EQ(table.capacity(), 7);    // 5/7 = 0.71, no resize yet

    // Insert 6th: (5+1)/7 = 0.86 > 0.75, triggers resize BEFORE inserting
    table.insert("F", 6);
    EXPECT_GT(table.capacity(), 7);
    EXPECT_EQ(table.size(), 6);
}

TEST(ProbingResize, AllEntriesSurvive) {
    ProbingHashTable table;
    std::string keys[] = {"A", "B", "C", "D", "E", "F"};
    for (int i = 0; i < 6; ++i) {
        table.insert(keys[i], (i + 1) * 10);
    }

    // All entries should be searchable after resize
    for (int i = 0; i < 6; ++i) {
        int* val = table.search(keys[i]);
        ASSERT_NE(val, nullptr) << "Key \"" << keys[i] << "\" lost after resize";
        EXPECT_EQ(*val, (i + 1) * 10);
    }
}

TEST(ProbingResize, TombstonesCleared) {
    ProbingHashTable table;   // capacity 7

    // Insert 4 entries
    table.insert("Alice", 95);
    table.insert("Bob", 82);
    table.insert("Charlie", 91);
    table.insert("Diana", 78);

    // Remove two — creates tombstones
    table.remove("Alice");
    table.remove("Bob");
    EXPECT_EQ(table.size(), 2);

    // Insert enough to trigger resize (need size+1 / capacity > 0.75)
    // size=2 after removes, capacity=7
    // Eve:   (2+1)/7=0.43 → no resize, size=3
    // Frank: (3+1)/7=0.57 → no resize, size=4
    // Grace: (4+1)/7=0.71 → no resize, size=5
    // Hank:  (5+1)/7=0.86 > 0.75 → resize triggered!
    table.insert("Eve", 88);
    table.insert("Frank", 73);
    table.insert("Grace", 96);
    table.insert("Hank", 65);

    EXPECT_GT(table.capacity(), 7);

    // After resize, all OCCUPIED entries should survive
    EXPECT_NE(table.search("Charlie"), nullptr);
    EXPECT_NE(table.search("Diana"), nullptr);
    EXPECT_NE(table.search("Eve"), nullptr);
    EXPECT_NE(table.search("Frank"), nullptr);
    EXPECT_NE(table.search("Grace"), nullptr);
    EXPECT_NE(table.search("Hank"), nullptr);

    // Removed entries should still be gone
    EXPECT_EQ(table.search("Alice"), nullptr);
    EXPECT_EQ(table.search("Bob"), nullptr);
}

// =============================================================================
// Tombstone-Specific Tests (5 points)
// =============================================================================

TEST(Tombstone, SearchProbesPastTombstone) {
    ProbingHashTable table;

    // Insert entries that may form a probe chain
    table.insert("Alice", 95);
    table.insert("Diana", 78);
    table.insert("Eve", 88);

    // Remove Diana — creates a tombstone in the probe path
    table.remove("Diana");

    // Eve should still be found (search must probe past Diana's tombstone)
    int* val = table.search("Eve");
    ASSERT_NE(val, nullptr)
        << "Search for Eve failed after removing Diana — "
           "tombstone not handled correctly";
    EXPECT_EQ(*val, 88);
}

TEST(Tombstone, InsertReusesTombstoneSlot) {
    ProbingHashTable table;

    table.insert("Alice", 95);
    table.insert("Bob", 82);
    int old_capacity = table.capacity();

    // Remove Alice — creates tombstone
    table.remove("Alice");
    EXPECT_EQ(table.size(), 1);

    // Insert a new key — should be able to use the tombstone slot
    table.insert("Charlie", 91);
    EXPECT_EQ(table.size(), 2);

    // Table should not have resized (tombstone was reused, not using extra space)
    EXPECT_EQ(table.capacity(), old_capacity);

    // All entries should be findable
    EXPECT_NE(table.search("Charlie"), nullptr);
    EXPECT_NE(table.search("Bob"), nullptr);
    EXPECT_EQ(table.search("Alice"), nullptr);
}

TEST(Tombstone, MultipleTombstonesInProbeChain) {
    ProbingHashTable table;

    // Insert several entries
    table.insert("Alice", 95);
    table.insert("Bob", 82);
    table.insert("Charlie", 91);
    table.insert("Diana", 78);
    table.insert("Eve", 88);

    // Remove multiple entries to create several tombstones
    table.remove("Bob");
    table.remove("Diana");
    EXPECT_EQ(table.size(), 3);

    // Remaining entries should all still be findable
    EXPECT_NE(table.search("Alice"), nullptr);
    EXPECT_NE(table.search("Charlie"), nullptr);
    EXPECT_NE(table.search("Eve"), nullptr);

    // Removed entries should return nullptr
    EXPECT_EQ(table.search("Bob"), nullptr);
    EXPECT_EQ(table.search("Diana"), nullptr);
}

TEST(Tombstone, InsertAfterRemoveSameKey) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.remove("Alice");

    // Re-insert same key with different value
    table.insert("Alice", 100);
    EXPECT_EQ(table.size(), 1);

    int* val = table.search("Alice");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 100);
}

// =============================================================================
// Print Tests (3 points)
// =============================================================================

TEST(ProbingPrint, DoesNotCrashEmpty) {
    ProbingHashTable table;
    // Should print all [empty] slots without crashing
    testing::internal::CaptureStdout();
    table.print();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());
}

TEST(ProbingPrint, DoesNotCrashWithEntries) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.insert("Bob", 82);

    testing::internal::CaptureStdout();
    table.print();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_FALSE(output.empty());

    // Output should contain the inserted keys
    EXPECT_NE(output.find("Alice"), std::string::npos);
    EXPECT_NE(output.find("Bob"), std::string::npos);
}

TEST(ProbingPrint, ShowsTombstones) {
    ProbingHashTable table;
    table.insert("Alice", 95);
    table.remove("Alice");

    testing::internal::CaptureStdout();
    table.print();
    std::string output = testing::internal::GetCapturedStdout();

    // Output should show [deleted] for the tombstone
    EXPECT_NE(output.find("[deleted]"), std::string::npos);
}
