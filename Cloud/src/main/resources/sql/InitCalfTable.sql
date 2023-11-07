CREATE TABLE IF NOT EXISTS calf_data
(
    calf_id BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    parent_id BIGINT,
    CONSTRAINT fk_pid FOREIGN KEY (parent_id) REFERENCES feeding_station_data(station_id) ON DELETE CASCADE,
    notes TEXT,
    phy_tag VARCHAR(128),
    max_consumption FLOAT
);
