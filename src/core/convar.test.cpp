#include "core/convar.h"
#include "test/common.h"
#include "gtest/gtest.h"

constexpr const char* param0Name = "myParam";
constexpr const char* param1Name = "otherParam";

TEST(Convar, Basic) {
    // Check equality/comparison operators
    Airship::Convar<int> convarA(2);
    EXPECT_GT(convarA, 1);
    EXPECT_EQ(convarA, 2);
    EXPECT_LT(convarA, 3);

    convarA = -3;
    EXPECT_GT(convarA, -4);
    EXPECT_EQ(convarA, -3);
    EXPECT_LT(convarA, -2);

    Airship::Convar<std::string> convarB("Test string");
    EXPECT_GT(convarB, "S");
    EXPECT_EQ(convarB, "Test string");
    EXPECT_LT(convarB, "U");

    Airship::Convar<float> convarC(1.234);
    EXPECT_GT(convarC, 0.5);
    EXPECT_EQ(convarC, 1.234);
    EXPECT_LT(convarC, 2);

    Airship::Convar<bool> convarD(false);
    EXPECT_GE(convarD, false);
    EXPECT_EQ(convarD, false);
    EXPECT_LT(convarD, true);
}

TEST(Convar, Registry) {
    // Test the empty registry
    Airship::ConvarRegistry registry;
    EXPECT_EQ(registry.size(), 0);
    EXPECT_EQ(registry.read<int>(param0Name), std::nullopt);
    EXPECT_EQ(registry.read<bool>("InvalidConvarKey"), std::nullopt);


    // Test how values change when a key is registered
    Airship::Convar<int>* myParam = registry.RegisterKey(param0Name, 2);
    EXPECT_EQ(registry.size(), 1);
    ASSERT_EQ(registry.read<int>(param0Name).has_value(), true);
    EXPECT_EQ(*registry.read<int>(param0Name).value(), 2);
    EXPECT_EQ(registry.read<float>(param0Name), std::nullopt);
    EXPECT_EQ(registry.read<bool>("InvalidConvarKey"), std::nullopt);

    // Test adding a key to a non-empty registry
    Airship::Convar<bool>* otherParam = registry.RegisterKey(param1Name, true);
    EXPECT_EQ(registry.size(), 2);
    ASSERT_EQ(registry.read<int>(param0Name).has_value(), true);
    EXPECT_EQ(*registry.read<int>(param0Name).value(), 2);
    EXPECT_EQ(registry.read<float>(param0Name), std::nullopt);
    ASSERT_EQ(registry.read<bool>(param1Name).has_value(), true);
    EXPECT_EQ(*registry.read<bool>(param1Name).value(), true);

    // Test changes to the registry when a convar is changed
    *myParam = 3;
    ASSERT_EQ(registry.read<int>(param0Name).has_value(), true);
    EXPECT_EQ(*registry.read<int>(param0Name).value(), 3);

    // Test changes from registry getting back to convars
    *registry.read<int>(param0Name).value() = 7;
    EXPECT_EQ(*myParam, 7);
}
