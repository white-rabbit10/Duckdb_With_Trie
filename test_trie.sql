-- Force DuckDB to use your Dictionary Compression
PRAGMA force_compression='dictionary';

CREATE TABLE test_trie (s VARCHAR);
-- Insert enough data to trigger compression
INSERT INTO test_trie SELECT 'abc' || (i % 100)::VARCHAR FROM range(10000000) t(i);
CHECKPOINT;

SELECT * FROM pragma_storage_info('test_trie'); -- verify dictionary compression

-- To avoid cold-cache noise, run the non-existing-query a couple of times:
SELECT count(*) FROM test_trie WHERE s = 'does_not_exist';
SELECT count(*) FROM test_trie WHERE s = 'does_not_exist';


PRAGMA enable_profiling = json;
PRAGMA profiling_output = '/Users/srishtisachan/Classes/CSCI 543 - ADS/Project 2/profiles/profile_trie_unmatched_path.json';
PRAGMA profiling_output = '/Users/srishtisachan/Classes/CSCI 543 - ADS/Project 2/profiles/profile_trie.json';

SELECT count(*) FROM test_trie WHERE s = 'abc50';