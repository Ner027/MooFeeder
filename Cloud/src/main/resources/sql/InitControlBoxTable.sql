CREATE TABLE IF NOT EXISTS box_data
(
    box_id              BIGINT PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    session_token   VARCHAR(32),
    name            VARCHAR(64),
    password_salt   VARCHAR(64),
    username        TEXT,
    password        TEXT
);
