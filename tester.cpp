#include <iostream>

#include "rbtree.h"

int main() {
	RedBlackTree<std::string, int> tree;
	for (auto& str : {"foo", "bar", "baz", "foo"}) {
		auto found = tree.find(str);
		if (found != tree.end()) {
			found->second++;
		} else {
			tree.insert(str, 1);
		}
	}
	for (auto pair : tree) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
	return 0;
}

