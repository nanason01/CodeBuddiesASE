### Entrypoints system tests
This folder contains system tests that test the entry/ functionality, written in Python, with use of the PyTest module.

# How to run manually
Start the server by typing (in the root directory of the project):
```
bazel run //entry:service
```
Open another window/tab in the terminal, and navigate to the systemtests folder. Assuming you are in the root directory:
```
cd systemtests
pytest
```

Please note: As discussed in demo, sometimes tests can fail if the external APIs this project calls act up, or rate-limit queries. Rerunning the tests solves the issue.