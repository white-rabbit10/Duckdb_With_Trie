#pragma once

#include "duckdb/common/types.hpp"
#include "duckdb/common/atomic.hpp"
#include "duckdb/function/built_in_functions.hpp"

namespace duckdb {

// ------------------------------------------------------------
// Global Trie Metrics
// ------------------------------------------------------------
struct TrieMetrics {
	atomic<uint64_t> filter_calls {0};
	atomic<uint64_t> fallback_calls {0};

	atomic<uint64_t> eq_positive_hits {0};
	atomic<uint64_t> eq_negative_hits {0};

	atomic<uint64_t> in_positive_hits {0};
	atomic<uint64_t> in_negative_hits {0};
};

extern TrieMetrics g_trie_metrics;

// Register table/pragma functions
void RegisterTrieStatsFunctions(BuiltinFunctions &set);

} // namespace duckdb