Scheme Interpreter (Python, Scheme)
==================

A Partner and I created a scheme language interpreter using python. The interpreter was able to read a subset of the scheme programming language. This project required an understanding of scheme, and included:
- reading scheme expressions
- primative procedure calls
- symbol evaluation and definition
- lambda expressions and procedure definition
- calling user-defined procedures
- evaluation of various special forms (if, and, or, cond, let, mu)

This project was also very test driven. We continually added tests to test.scm, including some recursive functions that should be interpreted correctly. We worked primarily in scheme.py, scheme_reader.py, and tests.scm files, which the other files as utility. 

- scheme.p: the scheme evaluator implementation
- scheme_reader.py: scheme parser
- tests.scm collection of test cases written in scheme for testing the interpreter. Contains simple unit tests and more ellaborate recursive functions in scheme.
- scheme_tokens.py: tokenizer for scheme
- scheme_primitives.py: primitive scheme procedures
- scheme_test.py: testing framework for scheme
- ucb.py: utility functions for the course


- Partner: Song King
