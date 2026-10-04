#include "models/Product.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

using inventory::domain::Product;

TEST(ProductTest, CreatesValidProduct) {
    const Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    EXPECT_EQ(product.id(), 1);
    EXPECT_EQ(product.sku(), "LAPTOP-001");
    EXPECT_EQ(product.name(), "Business Laptop");
    EXPECT_EQ(product.priceInCents(), 2599999);
    EXPECT_EQ(product.stockQuantity(), 10);
    EXPECT_TRUE(product.isActive());
}

TEST(ProductTest, RejectsNegativePrice) {
    EXPECT_THROW(
        Product(
            1,
            "LAPTOP-001",
            "Business Laptop",
            -100,
            10
        ),
        std::invalid_argument
    );
}

TEST(ProductTest, RejectsNegativeStock) {
    EXPECT_THROW(
        Product(
            1,
            "LAPTOP-001",
            "Business Laptop",
            2599999,
            -1
        ),
        std::invalid_argument
    );
}

TEST(ProductTest, IncreasesStockCorrectly) {
    Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    product.increaseStock(5);

    EXPECT_EQ(
        product.stockQuantity(),
        15
    );
}

TEST(ProductTest, DecreasesStockCorrectly) {
    Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    product.decreaseStock(4);

    EXPECT_EQ(
        product.stockQuantity(),
        6
    );
}

TEST(ProductTest, PreventsRemovingMoreStockThanAvailable) {
    Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    EXPECT_THROW(
        product.decreaseStock(15),
        std::runtime_error
    );
}

TEST(ProductTest, InactiveProductCannotFulfillOrders) {
    Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    product.deactivate();

    EXPECT_FALSE(
        product.canFulfill(1)
    );
}
