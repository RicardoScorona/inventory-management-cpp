#pragma once

#include <cstdint>
#include <string>

namespace inventory::domain {

class Product {
public:
    Product(
        std::int64_t id,
        std::string sku,
        std::string name,
        std::int64_t priceInCents,
        std::int32_t initialStock
    );

    [[nodiscard]] std::int64_t id() const noexcept;
    [[nodiscard]] const std::string& sku() const noexcept;
    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] std::int64_t priceInCents() const noexcept;
    [[nodiscard]] std::int32_t stockQuantity() const noexcept;
    [[nodiscard]] bool isActive() const noexcept;

    [[nodiscard]] bool canFulfill(
        std::int32_t quantity
    ) const noexcept;

    void rename(std::string newName);
    void changePrice(std::int64_t newPriceInCents);

    void increaseStock(std::int32_t quantity);
    void decreaseStock(std::int32_t quantity);

    void activate() noexcept;
    void deactivate() noexcept;

private:
    std::int64_t id_;
    std::string sku_;
    std::string name_;
    std::int64_t priceInCents_;
    std::int32_t stockQuantity_;
    bool active_{true};

    static void validateId(std::int64_t id);
    static void validateSku(const std::string& sku);
    static void validateName(const std::string& name);
    static void validatePrice(std::int64_t priceInCents);
    static void validateStock(std::int32_t stockQuantity);
};

} // namespace inventory::domain
