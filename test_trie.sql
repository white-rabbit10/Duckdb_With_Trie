-- Force DuckDB to use your Dictionary Compression
PRAGMA force_compression='dictionary';

CREATE TABLE test_trie (s VARCHAR);
-- Insert enough data to trigger compression
INSERT INTO test_trie SELECT 'abc' || (i % 100)::VARCHAR FROM range(10000000) t(i);
CHECKPOINT;

SELECT * FROM pragma_storage_info('test_trie'); -- verify dictionary compression

-- To avoid cold-cache noise, run the non-existing-query a couple of times:
SELECT count(*) FROM test_trie WHERE s = 'unmatched';


PRAGMA enable_profiling = json;
PRAGMA profiling_output = '/Users/srishtisachan/Classes/CSCI 543 - ADS/Project 2/profiles/profile_trie_unmatched_path.json';
PRAGMA profiling_output = '/Users/srishtisachan/Classes/CSCI 543 - ADS/Project 2/profiles/profile_trie.json';

SELECT count(*) FROM test_trie WHERE s = 'abc50';


 SELECT trie_stats_reset();
SELECT * FROM trie_stats();

/*
Because 'does_not_exist' is a constant string literal, DuckDB’s planner does something smart:

It consults the string statistics on the column before running any scan.

Since the segment-level statistics include:
	•	min string
	•	max string
	•	dictionary of unique strings
	•	zonemap for strings

DuckDB determines:

The value 'does_not_exist' cannot possibly be present → therefore return 0 immediately.

Meaning:

✔ No scan

✔ No vector loads

✔ No dictionary decode

✔ No column-level Filter() call

✔ Query answered entirely using statistics

Which is why your Trie metrics stay at 0.
*/