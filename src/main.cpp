#include "models/Product.hpp"
#include "services/InventoryService.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void printProduct(const inventory::domain::Product& product) {
    const double price =
        static_cast<double>(product.priceInCents()) / 100.0;

    std::cout
        << "SKU: " << product.sku() << '\n'
        << "Name: " << product.name() << '\n'
        << "Price: $"
        << std::fixed
        << std::setprecision(2)
        << price << '\n'
        << "Stock: " << product.stockQuantity() << '\n'
        << "Status: "
        << (product.isActive() ? "Active" : "Inactive")
        << "\n\n";
}

void printInventory(
    const inventory::application::InventoryService& inventoryService
) {
    const auto products = inventoryService.listProducts();

    std::cout << "\n====================================\n";
    std::cout << "          CURRENT INVENTORY         \n";
    std::cout << "====================================\n\n";

    if (products.empty()) {
        std::cout << "Inventory is empty.\n";
        return;
    }

    for (const auto& product : products) {
        printProduct(product);
    }

    std::cout
        << "Total products: "
        << inventoryService.productCount()
        << '\n';
}

} // namespace

int main() {
    try {
        inventory::application::InventoryService inventoryService;

        inventoryService.addProduct(
            inventory::domain::Product(
                1,
                "LAPTOP-001",
                "Business Laptop",
                2599999,
                10
            )
        );

        inventoryService.addProduct(
            inventory::domain::Product(
                2,
                "MONITOR-001",
                "27-inch Professional Monitor",
                749999,
                15
            )
        );

        inventoryService.addProduct(
            inventory::domain::Product(
                3,
                "KEYBOARD-001",
                "Mechanical Keyboard",
                249999,
                25
            )
        );

        inventoryService.restockProduct(
            "LAPTOP-001",
            5
        );

        inventoryService.removeStock(
            "KEYBOARD-001",
            3
        );

        inventoryService.deactivateProduct(
            "MONITOR-001"
        );

        printInventory(inventoryService);

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Application error: "
            << exception.what()
            << '\n';

        return 1;
    }
}
