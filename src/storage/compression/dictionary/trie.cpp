#include "duckdb/storage/compression/dictionary/trie.hpp"

#include <utility> // for std::move

namespace duckdb {

Trie::Trie() : root(make_uniq<TrieNode>()) {
}

void Trie::Insert(const string_t &str, uint32_t dict_id) {
	TrieNode *node = root.get();

	const char *data = str.GetData();
	const idx_t size = str.GetSize();

	for (idx_t i = 0; i < size; i++) {
		char c = data[i];
		auto it = node->children.find(c);
		if (it == node->children.end()) {
			auto child = make_uniq<TrieNode>();
			TrieNode *child_ptr = child.get();
			node->children.emplace(c, std::move(child));
			node = child_ptr;
		} else {
			node = it->second.get();
		}
	}

	node->is_terminal = true;
	node->dict_ids.push_back(dict_id);
}

bool Trie::FindExact(const string_t &str, uint32_t &dict_id) const {
	const TrieNode *node = root.get();

	const char *data = str.GetData();
	const idx_t size = str.GetSize();

	for (idx_t i = 0; i < size; i++) {
		char c = data[i];
		auto it = node->children.find(c);
		if (it == node->children.end()) {
			return false;
		}
		node = it->second.get();
	}

	if (node->is_terminal && !node->dict_ids.empty()) {
		dict_id = node->dict_ids[0];
		return true;
	}
	return false;
}

const TrieNode *Trie::TraversePrefix(const string_t &prefix) const {
	const TrieNode *node = root.get();

	const char *data = prefix.GetData();
	const idx_t size = prefix.GetSize();

	for (idx_t i = 0; i < size; i++) {
		char c = data[i];
		auto it = node->children.find(c);
		if (it == node->children.end()) {
			return nullptr;
		}
		node = it->second.get();
	}
	return node;
}

void Trie::DFS(const TrieNode *node, std::vector<uint32_t> &out_ids) const {
	if (!node) {
		return;
	}
	if (node->is_terminal) {
		out_ids.insert(out_ids.end(), node->dict_ids.begin(), node->dict_ids.end());
	}
	for (auto &kv : node->children) {
		DFS(kv.second.get(), out_ids);
	}
}

void Trie::CollectPrefix(const string_t &prefix, std::vector<uint32_t> &out_ids) const {
	const TrieNode *node = TraversePrefix(prefix);
	if (!node) {
		return; // no strings with this prefix
	}
	DFS(node, out_ids);
}

} // namespace duckdb