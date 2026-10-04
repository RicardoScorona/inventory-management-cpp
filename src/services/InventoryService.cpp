#include "services/InventoryService.hpp"

#include <stdexcept>
#include <utility>

namespace inventory::application {

void InventoryService::addProduct(domain::Product product) {
    const std::string sku = product.sku();

    if (containsProduct(sku)) {
        throw std::invalid_argument(
            "A product with SKU '" + sku + "' already exists."
        );
    }

    products_.emplace(
        sku,
        std::move(product)
    );
}

bool InventoryService::containsProduct(
    const std::string& sku
) const noexcept {
    return products_.find(sku) != products_.end();
}

domain::Product& InventoryService::getProduct(
    const std::string& sku
) {
    auto iterator = products_.find(sku);

    if (iterator == products_.end()) {
        throw std::out_of_range(
            "Product with SKU '" + sku + "' was not found."
        );
    }

    return iterator->second;
}

const domain::Product& InventoryService::getProduct(
    const std::string& sku
) const {
    auto iterator = products_.find(sku);

    if (iterator == products_.end()) {
        throw std::out_of_range(
            "Product with SKU '" + sku + "' was not found."
        );
    }

    return iterator->second;
}

void InventoryService::restockProduct(
    const std::string& sku,
    std::int32_t quantity
) {
    getProduct(sku).increaseStock(quantity);
}

void InventoryService::removeStock(
    const std::string& sku,
    std::int32_t quantity
) {
    getProduct(sku).decreaseStock(quantity);
}

void InventoryService::activateProduct(
    const std::string& sku
) {
    getProduct(sku).activate();
}

void InventoryService::deactivateProduct(
    const std::string& sku
) {
    getProduct(sku).deactivate();
}

std::vector<domain::Product>
InventoryService::listProducts() const {
    std::vector<domain::Product> result;
    result.reserve(products_.size());

    for (const auto& [sku, product] : products_) {
        result.push_back(product);
    }

    return result;
}

std::size_t InventoryService::productCount() const noexcept {
    return products_.size();
}

} // namespace inventory::application
