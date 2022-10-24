To run service:

```
bazel run //entry
bazel build //entry # to just build the binary, then the binary will exist in bazel-bin/
```

To modify database ddl:\
(eventually this will be done via an admin-only api endpoint)

```
./data/db_admin.sh {create, clear, dump} # any other input will print a help message
```

To run style checker:

```
pip install cpplint # only need to run install once
cpplint */*  # to run for entire repo
cpplint <path_to_file> # to run for single file
```

To run tests:

```
bazel test --test_output=all //... # run every test
bazel test --test_output=all //<target> # run single test suite
```
Endpoints : 

| Endpoint url  |   | Description | Input | Output |
| ------------- | - | ----------- | ----- | ------ |
| /getcredentials  | GET  | Generate credentials. Called the first time client uses the api | None | Json data with keys _client_id_, _api_key_ and _refresh_token_ |
| Content Cell  | Content Cell  |