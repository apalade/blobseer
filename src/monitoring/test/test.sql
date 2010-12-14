DROP TABLE IF EXISTS "internal_write_127.0.0.1";
DROP TABLE IF EXISTS "internal_read_127.0.0.1";
DROP TABLE IF EXISTS "physical_127.0.0.1";
DROP TABLE IF EXISTS "created_blobs";

CREATE TABLE created_blobs
(
  "client_id" VARCHAR(255),
  "blob_id" BIGINT,
  "replication" INT,
  "page_size" BIGINT,
  "rectime" BIGINT
);

CREATE TABLE "internal_write_127.0.0.1" (
  "rectime" BIGINT,
  "blob_id" BIGINT,
  "pages" INT
);

CREATE TABLE "internal_read_127.0.0.1" (
  "rectime" BIGINT,
  "blob_id" BIGINT,
  "pages" INT
);

CREATE TABLE "physical_127.0.0.1" (
  "rectime" BIGINT,
  "load5" REAL,
  "free" REAL
);

DROP INDEX IF EXISTS idx1;
DROP INDEX IF EXISTS idx2;
DROP INDEX IF EXISTS idx3;
DROP INDEX IF EXISTS idx4;

INSERT INTO "internal_read_127.0.0.1" VALUES
  (1272903459, 12, 3),
  (1272903559, 1, 1),
  (1272903659, 1, 19);

INSERT INTO "internal_write_127.0.0.1" VALUES
  (1272903459, 12, 3),
  (1272903559, 12, 2),
  (1272903659, 1, 1);


INSERT INTO "physical_127.0.0.1" VALUES
  (1272903459, 1.12, 0.9),
  (1272903559, 0.12, 0.9);

INSERT INTO "created_blobs" VALUES
  ('127.0.0.1', 1, 5, 64, 1272903559),
  ('127.0.0.1', 12, 2, 128, 1272303559);
  
CREATE INDEX idx1 ON "internal_read_127.0.0.1"(rectime);
CREATE INDEX idx2 ON "internal_write_127.0.0.1"(rectime);
CREATE INDEX idx3 ON "physical_127.0.0.1"(rectime);
CREATE INDEX idx4 ON "created_blobs"(blob_id);
