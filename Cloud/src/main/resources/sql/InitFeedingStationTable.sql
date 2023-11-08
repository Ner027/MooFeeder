CREATE TABLE IF NOT EXISTS station_data
(
    station_id BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    parent_id BIGINT,
    CONSTRAINT fk_pid FOREIGN KEY (parent_id) REFERENCES box_data(box_id) ON DELETE CASCADE,
    nof_calves INT,
    name VARCHAR(64),
    hw_id VARCHAR(32),
    description TEXT
);