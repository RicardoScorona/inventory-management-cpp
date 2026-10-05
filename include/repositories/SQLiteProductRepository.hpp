#pragma once

#include "repositories/IProductRepository.hpp"

#include <cstdint>
#include <string>

struct sqlite3;

namespace inventory::repository {

class SQLiteProductRepository final : public IProductRepository {
public:
    explicit SQLiteProductRepository(
        const std::string& databasePath
    );

    ~SQLiteProductRepository() override;

    SQLiteProductRepository(
        const SQLiteProductRepository&
    ) = delete;

    SQLiteProductRepository& operator=(
        const SQLiteProductRepository&
    ) = delete;

    SQLiteProductRepository(
        SQLiteProductRepository&&
    ) = delete;

    SQLiteProductRepository& operator=(
        SQLiteProductRepository&&
    ) = delete;

    void save(
        const domain::Product& product
    ) override;

    [[nodiscard]] std::optional<domain::Product>
    findBySku(
        const std::string& sku
    ) const override;

    [[nodiscard]] std::optional<domain::Product>
    findById(
        std::int64_t id
    ) const override;

    [[nodiscard]] std::vector<domain::Product>
    findAll() const override;

    void update(
        const domain::Product& product
    ) override;

    void removeBySku(
        const std::string& sku
    ) override;

    [[nodiscard]] bool existsBySku(
        const std::string& sku
    ) const override;

private:
    sqlite3* database_{nullptr};

    void openDatabase(
        const std::string& databasePath
    );

    void createSchema();

    void closeDatabase() noexcept;
};

} // namespace inventory::repository
