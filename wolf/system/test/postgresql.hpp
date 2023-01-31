#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_POSTGRESQL)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <system/db/w_postgresql.hpp>

using wolf::system::pgdb::w_pgdb_conection;
using wolf::system::pgdb::w_pgdb_execresult;
using wolf::system::pgdb::w_pgdb_queryresult;
using wolf::system::pgdb::w_pgdb_record;

#define POSTGRESQL_SERVER_ADDRESS "172.26.218.210"

BOOST_AUTO_TEST_CASE(postgresql_client_connection)
{
    // example: connect to database by url string.
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void> {
            constexpr auto conn_info_str = "postgresql://root@" POSTGRESQL_SERVER_ADDRESS ":26257/defaultdb";
            auto conn = w_pgdb_conection::make(conn_info_str);

            BOOST_LEAF_CHECK(conn);

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );

    // example: connect to database by parameter pairs.
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void> {
            auto conn = w_pgdb_conection::make(
                            std::pair{"host", POSTGRESQL_SERVER_ADDRESS},
                            std::pair{"port", "26257"},
                            std::pair{"user", "root"},
                            std::pair{"password", ""},
                            std::pair{"dbname", "defaultdb"}
                        );
            BOOST_LEAF_CHECK(conn);

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );
}

BOOST_AUTO_TEST_CASE(postgresql_client_commands_and_queries)
{
    struct User {
        std::string name = "";
        int age = 0;
    };

    constexpr auto conn_info_str = "postgresql://root@" POSTGRESQL_SERVER_ADDRESS ":26257/defaultdb";

    const User users[] = {
        User{ .name = "Lizard",   .age = 12 },
        User{ .name = "Haxorer",  .age = 15 },
        User{ .name = "BlueRose", .age = 16 },
        User{ .name = "Cthulhu",  .age = 18 }
    };

    auto conn = w_pgdb_conection::make(conn_info_str);

    BOOST_REQUIRE(conn);

    // example: create a table to operate on
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {
            auto res = conn->execute(
                "CREATE TABLE IF NOT EXISTS Users("
                "   id SERIAL NOT NULL PRIMARY KEY,"
                "   name STRING UNIQUE NOT NULL,"
                "   age INT NOT NULL"
                ");"
            );
            BOOST_LEAF_CHECK(res);

            res->done();

            // clean up before insertions.
            BOOST_LEAF_CHECK(conn->execute("DELETE FROM Users;"));

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );

    // example: inserts
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {
            BOOST_REQUIRE(std::size(users) > 0);

            for (const auto& user : users) {
                auto res = conn->execute(
                    "INSERT INTO Users(name, age) VALUES($1, $2);",
                    user.name,
                    user.age
                );
                BOOST_LEAF_ASSERT(res);
            }

            // make sure error on uniqe name is caught.
            auto res = conn->execute(
                "INSERT INTO Users(name, age) VALUES($1, $2);",
                users[0].name,
                users[0].age
            );
            BOOST_REQUIRE(!res);

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );

    // example: query, multiple rows, buffered (first fetches all then iterates)
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {   
            auto res = conn->stream_query("SELECT name, age FROM Users;");
            BOOST_LEAF_CHECK(res);

            BOOST_CHECK(!res->is_empty());

            BOOST_REQUIRE(res->field_count() == 2);
            BOOST_REQUIRE(res->size() == std::size(users));

            BOOST_CHECK(res->field_name_at(0) == std::string("name"));
            BOOST_CHECK(res->field_name_at(1) == std::string("age"));

            BOOST_LEAF_CHECK(res->on_each([&, counter=0](w_pgdb_record record) mutable -> boost::leaf::result<void> {
                User user;

                BOOST_REQUIRE(counter < std::size(users));

                BOOST_LEAF_CHECK(record.scan(&user.name, &user.age));

                BOOST_CHECK(user.name == users[counter].name);
                BOOST_CHECK(user.age == users[counter].age);

                ++counter;

                return {};
            }));

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );

    // example: query, single result
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {
            auto res = conn->query("SELECT version();");
            BOOST_LEAF_CHECK(res);

            BOOST_LEAF_CHECK(res->on_single([](w_pgdb_record record) -> boost::leaf::result<void> {
                auto version_str = record.at(0);
                BOOST_REQUIRE(version_str);

                std::cout << "postgresql test: database version is:" << version_str << std::endl;

                return {};
            }));

            return {};
        },
        [](const w_trace &p_trace) {
            std::cout << "got error :" << p_trace << std::endl;
            BOOST_REQUIRE(false);
        },
        [] {
            std::cout << "got an error!" << std::endl;
            BOOST_ERROR(false);
        }
    );
}

#endif  // WOLF_TEST
