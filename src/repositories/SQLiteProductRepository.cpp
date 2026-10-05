#include "repositories/SQLiteProductRepository.hpp"

#include <sqlite3.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using StatementPtr =
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

[[noreturn]] void throwDatabaseError(
    sqlite3* database,
    const std::string& message
) {
    throw std::runtime_error(
        message + ": " + sqlite3_errmsg(database)
    );
}

StatementPtr prepareStatement(
    sqlite3* database,
    const char* sql
) {
    sqlite3_stmt* rawStatement = nullptr;

    if (
        sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &rawStatement,
            nullptr
        ) != SQLITE_OK
    ) {
        throwDatabaseError(
            database,
            "Failed to prepare SQL statement"
        );
    }

    return StatementPtr(
        rawStatement,
        sqlite3_finalize
    );
}

void bindText(
    sqlite3* database,
    sqlite3_stmt* statement,
    int index,
    const std::string& value
) {
    if (
        sqlite3_bind_text(
            statement,
            index,
            value.c_str(),
            -1,
            SQLITE_TRANSIENT
        ) != SQLITE_OK
    ) {
        throwDatabaseError(
            database,
            "Failed to bind text parameter"
        );
    }
}

void executeStatement(
    sqlite3* database,
    sqlite3_stmt* statement
) {
    if (sqlite3_step(statement) != SQLITE_DONE) {
        throwDatabaseError(
            database,
            "Failed to execute SQL statement"
        );
    }
}

inventory::domain::Product productFromRow(
    sqlite3_stmt* statement
) {
    const auto* skuText =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 1)
        );

    const auto* nameText =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 2)
        );

    inventory::domain::Product product(
        sqlite3_column_int64(statement, 0),
        skuText != nullptr ? skuText : "",
        nameText != nullptr ? nameText : "",
        sqlite3_column_int64(statement, 3),
        sqlite3_column_int(statement, 4)
    );

    const bool active =
        sqlite3_column_int(statement, 5) != 0;

    if (!active) {
        product.deactivate();
    }

    return product;
}

} // namespace

namespace inventory::repository {

SQLiteProductRepository::SQLiteProductRepository(
    const std::string& databasePath
) {
    openDatabase(databasePath);
    createSchema();
}

SQLiteProductRepository::~SQLiteProductRepository() {
    closeDatabase();
}

void SQLiteProductRepository::openDatabase(
    const std::string& databasePath
) {
    if (
        sqlite3_open(
            databasePath.c_str(),
            &database_
        ) != SQLITE_OK
    ) {
        const std::string errorMessage =
            database_ != nullptr
                ? sqlite3_errmsg(database_)
                : "Unknown SQLite error";

        closeDatabase();

        throw std::runtime_error(
            "Failed to open database: " +
            errorMessage
        );
    }

    sqlite3_extended_result_codes(
        database_,
        1
    );

    sqlite3_busy_timeout(
        database_,
        5000
    );
}

void SQLiteProductRepository::createSchema() {
    constexpr const char* sql = R"sql(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY,
            sku TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            price_in_cents INTEGER NOT NULL
                CHECK(price_in_cents >= 0),
            stock_quantity INTEGER NOT NULL
                CHECK(stock_quantity >= 0),
            active INTEGER NOT NULL DEFAULT 1
                CHECK(active IN (0, 1))
        );
    )sql";

    char* errorMessage = nullptr;

    if (
        sqlite3_exec(
            database_,
            sql,
            nullptr,
            nullptr,
            &errorMessage
        ) != SQLITE_OK
    ) {
        const std::string message =
            errorMessage != nullptr
                ? errorMessage
                : "Unknown SQLite error";

        sqlite3_free(errorMessage);

        throw std::runtime_error(
            "Failed to create database schema: " +
            message
        );
    }
}

void SQLiteProductRepository::save(
    const domain::Product& product
) {
    constexpr const char* sql = R"sql(
        INSERT INTO products (
            id,
            sku,
            name,
            price_in_cents,
            stock_quantity,
            active
        )
        VALUES (?, ?, ?, ?, ?, ?);
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    sqlite3_bind_int64(
        statement.get(),
        1,
        product.id()
    );

    bindText(
        database_,
        statement.get(),
        2,
        product.sku()
    );

    bindText(
        database_,
        statement.get(),
        3,
        product.name()
    );

    sqlite3_bind_int64(
        statement.get(),
        4,
        product.priceInCents()
    );

    sqlite3_bind_int(
        statement.get(),
        5,
        product.stockQuantity()
    );

    sqlite3_bind_int(
        statement.get(),
        6,
        product.isActive() ? 1 : 0
    );

    executeStatement(
        database_,
        statement.get()
    );
}

std::optional<domain::Product>
SQLiteProductRepository::findBySku(
    const std::string& sku
) const {
    constexpr const char* sql = R"sql(
        SELECT
            id,
            sku,
            name,
            price_in_cents,
            stock_quantity,
            active
        FROM products
        WHERE sku = ?;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    bindText(
        database_,
        statement.get(),
        1,
        sku
    );

    const int result =
        sqlite3_step(statement.get());

    if (result == SQLITE_ROW) {
        return productFromRow(
            statement.get()
        );
    }

    if (result == SQLITE_DONE) {
        return std::nullopt;
    }

    throwDatabaseError(
        database_,
        "Failed to query product by SKU"
    );
}

std::optional<domain::Product>
SQLiteProductRepository::findById(
    std::int64_t id
) const {
    constexpr const char* sql = R"sql(
        SELECT
            id,
            sku,
            name,
            price_in_cents,
            stock_quantity,
            active
        FROM products
        WHERE id = ?;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    sqlite3_bind_int64(
        statement.get(),
        1,
        id
    );

    const int result =
        sqlite3_step(statement.get());

    if (result == SQLITE_ROW) {
        return productFromRow(
            statement.get()
        );
    }

    if (result == SQLITE_DONE) {
        return std::nullopt;
    }

    throwDatabaseError(
        database_,
        "Failed to query product by ID"
    );
}

std::vector<domain::Product>
SQLiteProductRepository::findAll() const {
    constexpr const char* sql = R"sql(
        SELECT
            id,
            sku,
            name,
            price_in_cents,
            stock_quantity,
            active
        FROM products
        ORDER BY id;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    std::vector<domain::Product> products;

    while (true) {
        const int result =
            sqlite3_step(statement.get());

        if (result == SQLITE_ROW) {
            products.push_back(
                productFromRow(statement.get())
            );

            continue;
        }

        if (result == SQLITE_DONE) {
            break;
        }

        throwDatabaseError(
            database_,
            "Failed to query products"
        );
    }

    return products;
}

void SQLiteProductRepository::update(
    const domain::Product& product
) {
    constexpr const char* sql = R"sql(
        UPDATE products
        SET
            name = ?,
            price_in_cents = ?,
            stock_quantity = ?,
            active = ?
        WHERE sku = ?;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    bindText(
        database_,
        statement.get(),
        1,
        product.name()
    );

    sqlite3_bind_int64(
        statement.get(),
        2,
        product.priceInCents()
    );

    sqlite3_bind_int(
        statement.get(),
        3,
        product.stockQuantity()
    );

    sqlite3_bind_int(
        statement.get(),
        4,
        product.isActive() ? 1 : 0
    );

    bindText(
        database_,
        statement.get(),
        5,
        product.sku()
    );

    executeStatement(
        database_,
        statement.get()
    );

    if (sqlite3_changes(database_) == 0) {
        throw std::out_of_range(
            "Cannot update product because SKU '" +
            product.sku() +
            "' does not exist."
        );
    }
}

void SQLiteProductRepository::removeBySku(
    const std::string& sku
) {
    constexpr const char* sql = R"sql(
        DELETE FROM products
        WHERE sku = ?;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    bindText(
        database_,
        statement.get(),
        1,
        sku
    );

    executeStatement(
        database_,
        statement.get()
    );
}

bool SQLiteProductRepository::existsBySku(
    const std::string& sku
) const {
    constexpr const char* sql = R"sql(
        SELECT 1
        FROM products
        WHERE sku = ?
        LIMIT 1;
    )sql";

    auto statement =
        prepareStatement(database_, sql);

    bindText(
        database_,
        statement.get(),
        1,
        sku
    );

    const int result =
        sqlite3_step(statement.get());

    if (result == SQLITE_ROW) {
        return true;
    }

    if (result == SQLITE_DONE) {
        return false;
    }

    throwDatabaseError(
        database_,
        "Failed to check product existence"
    );
}

void SQLiteProductRepository::closeDatabase() noexcept {
    if (database_ != nullptr) {
        sqlite3_close(database_);
        database_ = nullptr;
    }
}

} // namespace inventory::repository
