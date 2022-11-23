#!/bin/bash

bazel coverage //... --combined_report=lcov
genhtml bazel-out/_coverage/_coverage_report.dat
