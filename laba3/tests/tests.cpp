#include <gtest/gtest.h>
#include "../lib/Sandpile.h"

TEST(GrainSimulatorTest, ImportDataDoesNotThrow) {
    GrainSimulator sim(5, 5);
    EXPECT_NO_THROW(sim.importData("input/test_input.tsv"));
}

TEST(GrainSimulatorTest, ExecuteDoesNotCrash) {
    GrainSimulator sim(5, 5);
    sim.importData("input/test_input.tsv");
    EXPECT_NO_THROW(sim.execute());
}

TEST(GrainSimulatorTest, GridIsNonEmptyAfterExecute) {
    GrainSimulator sim(5, 5);
    sim.importData("input/test_input.tsv");
    sim.execute();
    const auto& grid = sim.getGrid();
    bool hasGrains = false;
    for (const auto& row : grid)
        for (auto cell : row)
            if (cell > 0)
                hasGrains = true;
    EXPECT_TRUE(hasGrains);
}

TEST(GrainSimulatorTest, GridSizeIsCorrect) {
    GrainSimulator sim(7, 3);
    sim.importData("input/test_input.tsv");
    sim.execute();
    const auto& grid = sim.getGrid();
    EXPECT_EQ(grid.size(), 3);
    for (const auto& row : grid)
        EXPECT_EQ(row.size(), 7);
}

TEST(GrainSimulatorTest, StableAfterExecute) {
    GrainSimulator sim(10, 10);
    sim.importData("input/test_input.tsv");
    sim.execute();
    const auto& grid = sim.getGrid();
    for (const auto& row : grid)
        for (auto cell : row)
            EXPECT_LT(cell, 4);
}
