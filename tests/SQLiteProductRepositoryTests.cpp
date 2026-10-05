#include "models/Product.hpp"
#include "repositories/SQLiteProductRepository.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using inventory::domain::Product;
using inventory::repository::SQLiteProductRepository;

namespace {

constexpr const char* TEST_DATABASE =
    "inventory_repository_test.db";

class SQLiteProductRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        removeDatabase();
    }

    void TearDown() override {
        removeDatabase();
    }

private:
    static void removeDatabase() {
        std::error_code errorCode;

        std::filesystem::remove(
            TEST_DATABASE,
            errorCode
        );
    }
};

} // namespace

TEST_F(
    SQLiteProductRepositoryTest,
    SavesAndFindsProductBySku
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    const Product product(
        1,
        "LAPTOP-001",
        "Business Laptop",
        2599999,
        10
    );

    repository.save(product);

    const auto storedProduct =
        repository.findBySku("LAPTOP-001");

    ASSERT_TRUE(storedProduct.has_value());

    EXPECT_EQ(storedProduct->id(), 1);
    EXPECT_EQ(
        storedProduct->sku(),
        "LAPTOP-001"
    );
    EXPECT_EQ(
        storedProduct->name(),
        "Business Laptop"
    );
    EXPECT_EQ(
        storedProduct->priceInCents(),
        2599999
    );
    EXPECT_EQ(
        storedProduct->stockQuantity(),
        10
    );
    EXPECT_TRUE(
        storedProduct->isActive()
    );
}

TEST_F(
    SQLiteProductRepositoryTest,
    FindsProductById
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    repository.save(
        Product(
            10,
            "MONITOR-001",
            "Professional Monitor",
            749999,
            15
        )
    );

    const auto product =
        repository.findById(10);

    ASSERT_TRUE(product.has_value());

    EXPECT_EQ(
        product->sku(),
        "MONITOR-001"
    );
}

TEST_F(
    SQLiteProductRepositoryTest,
    ReturnsAllStoredProducts
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    repository.save(
        Product(
            1,
            "LAPTOP-001",
            "Business Laptop",
            2599999,
            10
        )
    );

    repository.save(
        Product(
            2,
            "KEYBOARD-001",
            "Mechanical Keyboard",
            249999,
            25
        )
    );

    const auto products =
        repository.findAll();

    ASSERT_EQ(
        products.size(),
        2
    );

    EXPECT_EQ(
        products[0].id(),
        1
    );

    EXPECT_EQ(
        products[1].id(),
        2
    );
}

TEST_F(
    SQLiteProductRepositoryTest,
    UpdatesStoredProduct
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    Product product(
        1,
        "MOUSE-001",
        "Wireless Mouse",
        129999,
        20
    );

    repository.save(product);

    product.rename(
        "Professional Wireless Mouse"
    );

    product.changePrice(
        149999
    );

    product.decreaseStock(
        5
    );

    repository.update(product);

    const auto updatedProduct =
        repository.findBySku(
            "MOUSE-001"
        );

    ASSERT_TRUE(
        updatedProduct.has_value()
    );

    EXPECT_EQ(
        updatedProduct->name(),
        "Professional Wireless Mouse"
    );

    EXPECT_EQ(
        updatedProduct->priceInCents(),
        149999
    );

    EXPECT_EQ(
        updatedProduct->stockQuantity(),
        15
    );
}

TEST_F(
    SQLiteProductRepositoryTest,
    PersistsInactiveProductState
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    Product product(
        1,
        "HEADSET-001",
        "Professional Headset",
        189999,
        8
    );

    product.deactivate();

    repository.save(product);

    const auto storedProduct =
        repository.findBySku(
            "HEADSET-001"
        );

    ASSERT_TRUE(
        storedProduct.has_value()
    );

    EXPECT_FALSE(
        storedProduct->isActive()
    );
}

TEST_F(
    SQLiteProductRepositoryTest,
    RemovesProductBySku
) {
    SQLiteProductRepository repository(
        TEST_DATABASE
    );

    repository.save(
        Product(
            1,
            "KEYBOARD-001",
            "Mechanical Keyboard",
            249999,
            25
        )
    );

    ASSERT_TRUE(
        repository.existsBySku(
            "KEYBOARD-001"
        )
    );

    repository.removeBySku(
        "KEYBOARD-001"
    );

    EXPECT_FALSE(
        repository.existsBySku(
            "KEYBOARD-001"
        )
    );

    EXPECT_FALSE(
        repository.findBySku(
            "KEYBOARD-001"
        ).has_value()
    );
}
