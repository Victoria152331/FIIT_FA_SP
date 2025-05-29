//
// Created by Des Caldnd on 2/28/2025.
//

#include <b_tree_disk.hpp>
#include <iostream>
#include <filesystem>

int main() {
    // Создадим папку для хранения файлов дерева
    const std::string path = "../../../../../../associative_container/search_tree/indexing_tree/b_tree_disk/tests/test_tree";
    std::filesystem::create_directories(path);

    // Создаём дерево с минимальным t = 2 (по умолчанию)
    B_tree_disk<serializable_int, serializable_string> tree(path);

    // Вставляем несколько элементов
    tree.insert({ serializable_int(10), serializable_string("ten") });
    tree.insert({ serializable_int(5),  serializable_string("five") });
    tree.insert({ serializable_int(20), serializable_string("twenty") });
    tree.insert({ serializable_int(15), serializable_string("fifteen") });
    tree.insert({ serializable_int(30), serializable_string("thirty") });
    tree.insert({ serializable_int(25), serializable_string("twenty-five") });

    std::cout << "insert:\n";
    for (int key : {5, 10, 15, 20, 25, 30}) {
        auto val = tree.at(serializable_int(key));
        if (val) {
            std::cout << "  key " << key << " -> " << val->value << "\n";
        } else {
            std::cout << "  key " << key << " fot found\n";
        }
    }

    std::cout << "iterator:\n";

    auto e = tree.end();
    for (auto it = tree.begin(); it != e; it++) {
        std::cout << "  key " << (*it).first.value << " -> " << (*it).second.value << "\n";
    }

    // Удаляем пару
    bool erased = tree.erase(serializable_int(10));
    std::cout << "\nerase 10: " << (erased ? "success" : "fail") << "\n";

    // Проверяем снова
    std::cout << "\nafter erase:\n";
    for (int key : {5, 10, 15, 20, 25, 30}) {
        auto val = tree.at(serializable_int(key));
        if (val) {
            std::cout << "  key " << key << " -> " << val->value << "\n";
        } else {
            std::cout << "  key " << key << " not found\n";
        }
    }

    return 0;
}
