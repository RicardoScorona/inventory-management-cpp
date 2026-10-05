#pragma once

#include "models/Product.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace inventory::repository {

class IProductRepository {
public:
    virtual ~IProductRepository() = default;

    virtual void save(
        const domain::Product& product
    ) = 0;

    [[nodiscard]] virtual std::optional<domain::Product>
    findBySku(
        const std::string& sku
    ) const = 0;

    [[nodiscard]] virtual std::optional<domain::Product>
    findById(
        std::int64_t id
    ) const = 0;

    [[nodiscard]] virtual std::vector<domain::Product>
    findAll() const = 0;

    virtual void update(
        const domain::Product& product
    ) = 0;

    virtual void removeBySku(
        const std::string& sku
    ) = 0;

    [[nodiscard]] virtual bool existsBySku(
        const std::string& sku
    ) const = 0;
};

} // namespace inventory::repository
