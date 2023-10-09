CREATE TABLE IF NOT EXISTS feeding_station_data
(
    id BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    parent_id BIGINT,
    nof_calves INT,
    name VARCHAR(64),
    hw_id VARCHAR(32),
    description TEXT
)