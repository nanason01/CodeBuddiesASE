#Some Instructions

1.In order to make this excutable on your local, please change the `DB_FILENAME` inside `data.h` to:
```
constexpr auto DB_FILENAME = "/your/full/path/to/CodeBuddiesASE/data/db";

```

2.Before running the code, you need to create a local `db` file through calling:
```
./db_admin.sh create

```

3. Here goes some other script command that `db_admin.sh` provide.
```
./db_admin.sh clear  -> clears database (not exists ok)
./db_admin.sh create -> creates tables (exists ok)
./db_admin.sh dump   -> dumps database to console"

```

4. For running the `data_test.cc`, run `bazel run test` inside the directory or `bazel run //data:test`.
Before running the test, follow step 1 to change `TEST_DB_FILENAME` inside `data_test.cc` to the full path.
