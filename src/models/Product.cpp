#include "models/Product.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace {

bool isBlank(const std::string& value) {
    return std::all_of(
        value.begin(),
        value.end(),
        [](unsigned char character) {
            return std::isspace(character);
        }
    );
}

} // namespace

namespace inventory::domain {

Product::Product(
    std::int64_t id,
    std::string sku,
    std::string name,
    std::int64_t priceInCents,
    std::int32_t initialStock
)
    : id_(id),
      sku_(std::move(sku)),
      name_(std::move(name)),
      priceInCents_(priceInCents),
      stockQuantity_(initialStock) {

    validateId(id_);
    validateSku(sku_);
    validateName(name_);
    validatePrice(priceInCents_);
    validateStock(stockQuantity_);
}

std::int64_t Product::id() const noexcept {
    return id_;
}

const std::string& Product::sku() const noexcept {
    return sku_;
}

const std::string& Product::name() const noexcept {
    return name_;
}

std::int64_t Product::priceInCents() const noexcept {
    return priceInCents_;
}

std::int32_t Product::stockQuantity() const noexcept {
    return stockQuantity_;
}

bool Product::isActive() const noexcept {
    return active_;
}

bool Product::canFulfill(std::int32_t quantity) const noexcept {
    return active_ &&
           quantity > 0 &&
           stockQuantity_ >= quantity;
}

void Product::rename(std::string newName) {
    validateName(newName);
    name_ = std::move(newName);
}

void Product::changePrice(std::int64_t newPriceInCents) {
    validatePrice(newPriceInCents);
    priceInCents_ = newPriceInCents;
}

void Product::increaseStock(std::int32_t quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument(
            "Stock increase quantity must be greater than zero."
        );
    }

    stockQuantity_ += quantity;
}

void Product::decreaseStock(std::int32_t quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument(
            "Stock decrease quantity must be greater than zero."
        );
    }

    if (!canFulfill(quantity)) {
        throw std::runtime_error(
            "Insufficient stock or inactive product."
        );
    }

    stockQuantity_ -= quantity;
}

void Product::activate() noexcept {
    active_ = true;
}

void Product::deactivate() noexcept {
    active_ = false;
}

void Product::validateId(std::int64_t id) {
    if (id <= 0) {
        throw std::invalid_argument(
            "Product ID must be greater than zero."
        );
    }
}

void Product::validateSku(const std::string& sku) {
    if (sku.empty() || isBlank(sku)) {
        throw std::invalid_argument(
            "Product SKU cannot be empty."
        );
    }

    if (sku.length() > 50) {
        throw std::invalid_argument(
            "Product SKU cannot exceed 50 characters."
        );
    }
}

void Product::validateName(const std::string& name) {
    if (name.empty() || isBlank(name)) {
        throw std::invalid_argument(
            "Product name cannot be empty."
        );
    }

    if (name.length() > 120) {
        throw std::invalid_argument(
            "Product name cannot exceed 120 characters."
        );
    }
}

void Product::validatePrice(std::int64_t priceInCents) {
    if (priceInCents < 0) {
        throw std::invalid_argument(
            "Product price cannot be negative."
        );
    }
}

void Product::validateStock(std::int32_t stockQuantity) {
    if (stockQuantity < 0) {
        throw std::invalid_argument(
            "Product stock cannot be negative."
        );
    }
}

} // namespace inventory::domain
