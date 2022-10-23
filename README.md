To run service:
```bazel run //entry
bazel build //entry # to just build the binary, then the binary will exist in bazel-bin/
```

To modify database ddl:\
(eventually this will be done via an admin-only api endpoint)

```
TODO
```

To run style checker:
```# only need to run install once
pip install cpplint
cpplint */*  # to run for entire repo
cpplint <path_to_file> # to run for single file
```

To run tests:
```bazel test --test_output=all //... # run every test
bazel test --test_output=all //<target> # run single test suite
```
