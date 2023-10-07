CREATE TABLE IF NOT EXISTS control_box_data
(
    id              BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    nof_connections INT DEFAULT 0,
    session_token   VARCHAR(32),
    name            VARCHAR(64),
    password_salt   VARCHAR(64),
    username        TEXT,
    password        TEXT
)


