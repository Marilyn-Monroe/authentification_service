CREATE TABLE IF NOT EXISTS accounts (
    id uuid DEFAULT gen_random_uuid(),
    username TEXT NOT NULL,
    email TEXT NOT NULL,
    pass_salt TEXT NOT NULL,
    pass_hash TEXT NOT NULL,
    PRIMARY KEY (id)
);