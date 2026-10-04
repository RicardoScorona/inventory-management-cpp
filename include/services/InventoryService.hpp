#pragma once

#include "models/Product.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace inventory::application {

class InventoryService {
public:
    void addProduct(domain::Product product);

    [[nodiscard]] bool containsProduct(
        const std::string& sku
    ) const noexcept;

    [[nodiscard]] domain::Product& getProduct(
        const std::string& sku
    );

    [[nodiscard]] const domain::Product& getProduct(
        const std::string& sku
    ) const;

    void restockProduct(
        const std::string& sku,
        std::int32_t quantity
    );

    void removeStock(
        const std::string& sku,
        std::int32_t quantity
    );

    void activateProduct(
        const std::string& sku
    );

    void deactivateProduct(
        const std::string& sku
    );

    [[nodiscard]] std::vector<domain::Product>
    listProducts() const;

    [[nodiscard]] std::size_t productCount() const noexcept;

private:
    std::unordered_map<std::string, domain::Product> products_;
};

} // namespace inventory::application
