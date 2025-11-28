#pragma once

#include "duckdb/common/helper.hpp" 
#include "duckdb/common/types/string_type.hpp"
#include "duckdb/common/types.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace duckdb {

struct TrieNode {
	// Children keyed by character
	std::unordered_map<char, std::unique_ptr<TrieNode>> children;
	// Dictionary IDs (indexes into the dictionary) that end at this node
	std::vector<uint32_t> dict_ids;
	bool is_terminal = false;
};

class Trie {
public:
	Trie();

	//! Insert full string + associated dictionary ID
	void Insert(const string_t &str, uint32_t dict_id);

	//! Find exact string (returns true if found and sets dict_id to one matching ID)
	bool FindExact(const string_t &str, uint32_t &dict_id) const;

	//! Collect all dictionary IDs whose strings start with this prefix
	void CollectPrefix(const string_t &prefix, std::vector<uint32_t> &out_ids) const;

private:
	std::unique_ptr<TrieNode> root;

	const TrieNode *TraversePrefix(const string_t &prefix) const;
	void DFS(const TrieNode *node, std::vector<uint32_t> &out_ids) const;
};

} // namespace duckdb