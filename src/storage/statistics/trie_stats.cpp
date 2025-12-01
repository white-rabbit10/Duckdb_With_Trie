#include "duckdb/storage/statistics/trie_stats.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/common/types.hpp"
#include "duckdb/main/client_context.hpp"

namespace duckdb {

TrieMetrics g_trie_metrics;
struct TrieStatsGlobalState : public GlobalTableFunctionState {
	bool finished = false;
};

static unique_ptr<FunctionData> TrieStatsBind(ClientContext &context, TableFunctionBindInput &input,
                                              vector<LogicalType> &return_types, vector<string> &names) {
	names.emplace_back("filter_calls");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("fallback_calls");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("eq_positive_hits");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("eq_negative_hits");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("in_positive_hits");
	return_types.emplace_back(LogicalType::BIGINT);

	names.emplace_back("in_negative_hits");
	return_types.emplace_back(LogicalType::BIGINT);

	return nullptr;
}

static unique_ptr<GlobalTableFunctionState> TrieStatsInit(ClientContext &context, TableFunctionInitInput &input) {
	return make_uniq<TrieStatsGlobalState>();
}

static void TrieStatsFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &state = data_p.global_state->Cast<TrieStatsGlobalState>();

	if (state.finished) {
		// Already returned the single row
		return;
	}

	// Return a single row with all 6 metrics
	idx_t row_idx = 0;
	idx_t col_idx = 0;

	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.filter_calls.load())));
	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.fallback_calls.load())));
	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.eq_positive_hits.load())));
	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.eq_negative_hits.load())));
	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.in_positive_hits.load())));
	output.SetValue(col_idx++, row_idx, Value::BIGINT(static_cast<int64_t>(g_trie_metrics.in_negative_hits.load())));

	output.SetCardinality(1);
	state.finished = true;
}

static void TrieStatsResetFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	// Reset all counters to zero
	g_trie_metrics.filter_calls = 0;
	g_trie_metrics.fallback_calls = 0;
	g_trie_metrics.eq_positive_hits = 0;
	g_trie_metrics.eq_negative_hits = 0;
	g_trie_metrics.in_positive_hits = 0;
	g_trie_metrics.in_negative_hits = 0;

	// Return "OK"
	result.SetValue(0, Value("OK"));
}

void RegisterTrieStatsFunctions(BuiltinFunctions &set) {
	// Table function: trie_stats()
	TableFunction trie_stats_func("trie_stats", {}, TrieStatsFunction, TrieStatsBind, TrieStatsInit);
	set.AddFunction(trie_stats_func);

	// Scalar function: trie_stats_reset()
	ScalarFunction reset_func("trie_stats_reset", {}, LogicalType::VARCHAR, TrieStatsResetFunction);
	set.AddFunction(reset_func);
}

} // namespace duckdb