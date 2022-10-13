```bazel quick start guide:

all targets are structured like:

// <- indicates the root directory, where WORKSPACE file is

//<path to file>:<target name>

simple commands:

bazel run <target>
run (and maybe build) a target
e.g. bazel run //entry:test

bazel build <target>
e.g. bazel build //entry:test
then:
bazel-bin/entry/test

bazel test <test target>
e.g. bazel test --test_output=all //entry/test:hello```
