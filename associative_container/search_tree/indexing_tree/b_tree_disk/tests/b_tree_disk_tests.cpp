//
// Created by Des Caldnd on 2/28/2025.
//

#include <b_tree_disk.hpp>
#include <iostream>
#include <filesystem>

// int main() {
//     // Создадим папку для хранения файлов дерева
//     const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree";
//     std::filesystem::create_directories(path);

//     // Создаём дерево с минимальным t = 2 (по умолчанию)
//     B_tree_disk<serializable_int, serializable_string> tree(path);

//     // Вставляем несколько элементов
//     tree.insert({ serializable_int(10), serializable_string("ten") });
//     tree.insert({ serializable_int(5),  serializable_string("five") });
//     tree.insert({ serializable_int(20), serializable_string("twenty") });
//     tree.insert({ serializable_int(15), serializable_string("fifteen") });
//     tree.insert({ serializable_int(30), serializable_string("thirty") });
//     tree.insert({ serializable_int(25), serializable_string("twenty-five") });

//     std::cout << "insert:\n";
//     for (int key : {5, 10, 15, 20, 25, 30}) {
//         auto val = tree.at(serializable_int(key));
//         if (val) {
//             std::cout << "  key " << key << " -> " << val->value << "\n";
//         } else {
//             std::cout << "  key " << key << " fot found\n";
//         }
//     }

//     std::cout << "iterator:\n";

//     auto e = tree.end();
//     for (auto it = tree.begin(); it != e; it++) {
//         std::cout << "  key " << (*it).first.value << " -> " << (*it).second.value << "\n";
//     }

//     // Удаляем пару
//     bool erased = tree.erase(serializable_int(10));
//     std::cout << "\nerase 10: " << (erased ? "success" : "fail") << "\n";

//     // Проверяем снова
//     std::cout << "\nafter erase:\n";
//     for (int key : {5, 10, 15, 20, 25, 30}) {
//         auto val = tree.at(serializable_int(key));
//         if (val) {
//             std::cout << "  key " << key << " -> " << val->value << "\n";
//         } else {
//             std::cout << "  key " << key << " not found\n";
//         }
//     }

//     return 0;
// }

#include "gtest/gtest.h"

#include <list>
#include <random>
#include <vector>
#include <b_tree_disk.hpp>
#include <client_logger_builder.h>


template<typename tvalue>
bool compare_obtain_results(
    std::vector<tvalue> const &expected,
    std::vector<tvalue> const &actual)
{
    if (expected.size() != actual.size())
    {
        return false;
    }

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (expected[i] != actual[i])
        {
            return false;
        }
    }

    return true;
}

logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    std::unique_ptr<logger_builder> builder(new client_logger_builder());

    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }

    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger *built_logger = builder->build();

    return built_logger;
}

template <typename tkey, typename tvalue>
struct test_data
{
    tkey key;
    tvalue value;
    size_t depth, index;

    test_data(size_t d, size_t i, tkey k, tvalue v) : depth(d), index(i), key(k), value(v) {}
};

template<typename tkey, typename tvalue, typename comp, size_t t>
bool infix_const_iterator_test(
    B_tree_disk<tkey, tvalue, comp, t> &tree,
    std::vector<test_data<tkey, tvalue>> &expected_result)
{
    auto end_infix = tree.end();
    auto it = tree.begin();

    for (auto item: expected_result)
    {
        auto data = *it;

        if ((*it).first != item.key || (*it).second != item.value)
        {
            return false;
        }

        ++it;
    }

    return true;
}

TEST(bTreePositiveTests, test0)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
    {
        { "b_tree_tests_logs.txt", logger::severity::trace }
    }));

    logger->trace("bTreePositiveTests.test0 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {

    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree0";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 1024> tree(path);

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test0 finished");
}

TEST(bTreePositiveTests, test1)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test1 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(1, 0, 1, std::string("a")),
        test_data<serializable_int, serializable_string>(1, 1, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(1, 2, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(0, 0, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(1, 0, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(1, 1, 27, std::string("f"))
    };
    
    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree1";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 3> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({27, std::string("f")});

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test1 finished");
}

TEST(bTreePositiveTests, test2)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test2 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(1, 0, 1,  std::string("a")),
        test_data<serializable_int, serializable_string>(1, 1, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(1, 2, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(1, 3, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(1, 4, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(0, 0, 24, std::string("g")),
        test_data<serializable_int, serializable_string>(1, 0, 45, std::string("k")),
        test_data<serializable_int, serializable_string>(1, 1, 100, std::string("f")),
        test_data<serializable_int, serializable_string>(1, 2, 101, std::string("j")),
        test_data<serializable_int, serializable_string>(1, 3, 193, std::string("l")),
        test_data<serializable_int, serializable_string>(1, 4, 456, std::string("h")),
        test_data<serializable_int, serializable_string>(1, 5, 534, std::string("m"))
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree2";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 5> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string("f")});
    tree.insert({24, std::string("g")});
    tree.insert({456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({45, std::string("k")});
    tree.insert({193, std::string("l")});
    tree.insert({534, std::string("m")});

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test2 finished");
}

TEST(bTreePositiveTests, test3)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test3 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(0, 0, 1, std::string("a")),
        test_data<serializable_int, serializable_string>(0, 1, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(0, 2, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(0, 3, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(0, 4, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(0, 5, 24, std::string("g")),
        test_data<serializable_int, serializable_string>(0, 6, 45, std::string("k")),
        test_data<serializable_int, serializable_string>(0, 7, 100, std::string("f")),
        test_data<serializable_int, serializable_string>(0, 8, 101, std::string("j")),
        test_data<serializable_int, serializable_string>(0, 9, 193, std::string("l")),
        test_data<serializable_int, serializable_string>(0, 10, 456, std::string("h")),
        test_data<serializable_int, serializable_string>(0, 11, 534, std::string("m"))
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree3";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 7> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string("f")});
    tree.insert({24, std::string("g")});
    tree.insert({456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({45, std::string("k")});
    tree.insert({193, std::string("l")});
    tree.insert({534, std::string("m")});

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test3 finished");
}

TEST(bTreePositiveTests, test4)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test4 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(1, 0, 1, std::string("a")),
        test_data<serializable_int, serializable_string>(1, 1, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(1, 2, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(0, 0, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(1, 0, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(1, 1, 24, std::string("g")),
        test_data<serializable_int, serializable_string>(1, 2, 45, std::string("k")),
        test_data<serializable_int, serializable_string>(0, 1, 100, std::string("f")),
        test_data<serializable_int, serializable_string>(1, 0, 101, std::string("j")),
        test_data<serializable_int, serializable_string>(1, 1, 193, std::string("l")),
        test_data<serializable_int, serializable_string>(1, 2, 456, std::string("h")),
        test_data<serializable_int, serializable_string>(1, 3, 534, std::string("m"))
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree4";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 3> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string("f")});
    tree.insert({24, std::string("g")});
    tree.insert({456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({45, std::string("k")});
    tree.insert({193, std::string("l")});
    tree.insert({534, std::string("m")});

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test4 finished");
}

TEST(bTreePositiveTests, test5)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test5 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(1, 0, 1, std::string("a")),
        test_data<serializable_int, serializable_string>(1, 1, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(1, 2, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(0, 0, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(1, 0, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(1, 1, 24, std::string("g")),
        test_data<serializable_int, serializable_string>(1, 2, 45, std::string("k")),
        test_data<serializable_int, serializable_string>(0, 1, 100, std::string("f")),
        test_data<serializable_int, serializable_string>(1, 0, 101, std::string("j")),
        test_data<serializable_int, serializable_string>(1, 1, 193, std::string("l")),
        test_data<serializable_int, serializable_string>(1, 2, 456, std::string("h")),
        test_data<serializable_int, serializable_string>(1, 3, 534, std::string("m"))
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree4";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 3> tree(path);

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test5 finished");
}

TEST(bTreePositiveTests, test6)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test6 started");

    std::vector<test_data<serializable_int, serializable_string>> expected_result =
    {
        test_data<serializable_int, serializable_string>(1, 0, 2, std::string("b")),
        test_data<serializable_int, serializable_string>(1, 1, 3, std::string("d")),
        test_data<serializable_int, serializable_string>(1, 2, 4, std::string("e")),
        test_data<serializable_int, serializable_string>(0, 0, 15, std::string("c")),
        test_data<serializable_int, serializable_string>(1, 0, 45, std::string("k")),
        test_data<serializable_int, serializable_string>(1, 1, 101, std::string("j")),
        test_data<serializable_int, serializable_string>(1, 2, 456, std::string("h")),
        test_data<serializable_int, serializable_string>(1, 3, 534, std::string("m"))
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree6";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 4> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string("f")});
    tree.insert({24, std::string("g")});
    tree.insert({456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({45, std::string("k")});
    tree.insert({193, std::string("l")});
    tree.insert({534, std::string("m")});

    auto first_disposed = std::move(tree.at(1));
    auto second_disposed = std::move(tree.at(100));
    auto third_disposed = std::move(tree.at(193));
    auto fourth_disposed = std::move(tree.at(24));

    tree.erase(1);
    tree.erase(100);
    tree.erase(193);
    tree.erase(24);

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    EXPECT_TRUE(first_disposed == std::string("a"));
    EXPECT_TRUE(second_disposed == std::string("f"));
    EXPECT_TRUE(third_disposed == std::string("l"));
    EXPECT_TRUE(fourth_disposed == std::string("g"));

    logger->trace("bTreePositiveTests.test6 finished");
}

TEST(bTreePositiveTests, test7)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test7 started");

    std::vector<serializable_string> expected_result =
    {
        std::string("g"),
        std::string("d"),
        std::string("e"),
        std::string(" "),
        std::string("l"),
        std::string("a"),
        std::string("b"),
        std::string("y")
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree7";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 5> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string(" ")});
    tree.insert({24, std::string("g")});
    tree.insert({-456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({-45, std::string("k")});
    tree.insert({-193, std::string("l")});
    tree.insert({534, std::string("m")});
    tree.insert({1000, std::string("y")});

    std::vector<serializable_string> actual_result =
    {
        tree.at(24).value(),
        tree.at(3).value(),
        tree.at(4).value(),
        tree.at(100).value(),
        tree.at(-193).value(),
        tree.at(1).value(),
        tree.at(2).value(),
        tree.at(1000).value()
    };

    EXPECT_TRUE(compare_obtain_results(expected_result, actual_result));

    logger->trace("bTreePositiveTests.test7 finished");
}

TEST(bTreePositiveTests, test8)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test8 started");

    std::vector<serializable_string> expected_result =
    {
        std::string("y"),
        std::string("l"),
        std::string("a"),
        std::string("g"),
        std::string("k"),
        std::string("b"),
        std::string("c"),
        std::string("h")
    };

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree8";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 4> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string(" ")});
    tree.insert({24, std::string("g")});
    tree.insert({-456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({-45, std::string("k")});
    tree.insert({-193, std::string("l")});
    tree.insert({534, std::string("m")});
    tree.insert({1000, std::string("y")});

    std::vector<serializable_string> actual_result =
    {
        tree.at(1000).value(),
        tree.at(-193).value(),
        tree.at(1).value(),
        tree.at(24).value(),
        tree.at(-45).value(),
        tree.at(2).value(),
        tree.at(15).value(),
        tree.at(-456).value()
    };

    EXPECT_TRUE(compare_obtain_results(expected_result, actual_result));

    logger->trace("bTreePositiveTests.test8 finished");
}

TEST(bTreeNegativeTests, test1)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreeNegativeTests.test1 started");

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree_neg_1";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 3> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});

    EXPECT_EQ(tree.erase(45), false);

    logger->trace("bTreeNegativeTests.test1 finished");
}

TEST(bTreeNegativeTests, test3)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreeNegativeTests.test3 started");

    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree_neg_3";
    std::filesystem::create_directories(path);

    B_tree_disk<serializable_int, serializable_string, std::less<serializable_int>, 4> tree(path);

    tree.insert({1, std::string("a")});
    tree.insert({2, std::string("b")});
    tree.insert({15, std::string("c")});
    tree.insert({3, std::string("d")});
    tree.insert({4, std::string("e")});
    tree.insert({100, std::string(" ")});
    tree.insert({24, std::string("g")});
    tree.insert({-456, std::string("h")});
    tree.insert({101, std::string("j")});
    tree.insert({-45, std::string("k")});
    tree.insert({-193, std::string("l")});
    tree.insert({534, std::string("m")});
    tree.insert({1000, std::string("y")});

    EXPECT_EQ(tree.erase(1001), false);

    logger->trace("bTreeNegativeTests.test3 finished");
}

int main(
    int argc,
    char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}