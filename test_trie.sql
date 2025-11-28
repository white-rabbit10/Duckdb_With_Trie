-- Force DuckDB to use your Dictionary Compression
PRAGMA force_compression='dictionary';

CREATE TABLE test_trie (s VARCHAR);
-- Insert enough data to trigger compression
INSERT INTO test_trie SELECT 'abc' || (i % 100)::VARCHAR FROM range(20000) t(i);
CHECKPOINT;

-- This query should trigger your StringFilter
SELECT count(*) FROM test_trie WHERE s = 'abc50';