#include "models/Product.hpp"
#include "services/InventoryService.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using inventory::application::InventoryService;
using inventory::domain::Product;

namespace {

Product createProduct(
    std::int64_t id,
    std::string sku,
    std::string name,
    std::int64_t priceInCents,
    std::int32_t stock
) {
    return Product(
        id,
        std::move(sku),
        std::move(name),
        priceInCents,
        stock
    );
}

} // namespace

TEST(InventoryServiceTest, RegistersProductSuccessfully) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "LAPTOP-001",
            "Business Laptop",
            2599999,
            10
        )
    );

    EXPECT_TRUE(
        service.containsProduct("LAPTOP-001")
    );

    EXPECT_EQ(
        service.productCount(),
        1
    );
}

TEST(InventoryServiceTest, RejectsDuplicateSku) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "LAPTOP-001",
            "Business Laptop",
            2599999,
            10
        )
    );

    EXPECT_THROW(
        service.addProduct(
            createProduct(
                2,
                "LAPTOP-001",
                "Another Laptop",
                1999999,
                5
            )
        ),
        std::invalid_argument
    );
}

TEST(InventoryServiceTest, ReturnsProductBySku) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "MONITOR-001",
            "Professional Monitor",
            749999,
            15
        )
    );

    const Product& product =
        service.getProduct("MONITOR-001");

    EXPECT_EQ(
        product.name(),
        "Professional Monitor"
    );

    EXPECT_EQ(
        product.stockQuantity(),
        15
    );
}

TEST(InventoryServiceTest, ThrowsWhenProductDoesNotExist) {
    InventoryService service;

    EXPECT_THROW(
        service.getProduct("UNKNOWN-SKU"),
        std::out_of_range
    );
}

TEST(InventoryServiceTest, RestocksProductCorrectly) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "KEYBOARD-001",
            "Mechanical Keyboard",
            249999,
            10
        )
    );

    service.restockProduct(
        "KEYBOARD-001",
        5
    );

    EXPECT_EQ(
        service.getProduct(
            "KEYBOARD-001"
        ).stockQuantity(),
        15
    );
}

TEST(InventoryServiceTest, RemovesStockCorrectly) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "MOUSE-001",
            "Wireless Mouse",
            129999,
            20
        )
    );

    service.removeStock(
        "MOUSE-001",
        6
    );

    EXPECT_EQ(
        service.getProduct(
            "MOUSE-001"
        ).stockQuantity(),
        14
    );
}

TEST(InventoryServiceTest, DeactivatesAndActivatesProduct) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "HEADSET-001",
            "Professional Headset",
            189999,
            8
        )
    );

    service.deactivateProduct(
        "HEADSET-001"
    );

    EXPECT_FALSE(
        service.getProduct(
            "HEADSET-001"
        ).isActive()
    );

    service.activateProduct(
        "HEADSET-001"
    );

    EXPECT_TRUE(
        service.getProduct(
            "HEADSET-001"
        ).isActive()
    );
}

TEST(InventoryServiceTest, TracksMultipleProducts) {
    InventoryService service;

    service.addProduct(
        createProduct(
            1,
            "LAPTOP-001",
            "Business Laptop",
            2599999,
            10
        )
    );

    service.addProduct(
        createProduct(
            2,
            "MONITOR-001",
            "Professional Monitor",
            749999,
            15
        )
    );

    service.addProduct(
        createProduct(
            3,
            "KEYBOARD-001",
            "Mechanical Keyboard",
            249999,
            25
        )
    );

    EXPECT_EQ(
        service.productCount(),
        3
    );
}
