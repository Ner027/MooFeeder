CREATE TABLE IF NOT EXISTS log_data
(
    log_id BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    parent_id BIGINT,
    CONSTRAINT fk_pid FOREIGN KEY (parent_id) REFERENCES calf_data(calf_id) ON DELETE CASCADE,
    log_timestamp BIGINT,
    consumed_volume FLOAT
);