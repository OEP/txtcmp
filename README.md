txtcmp
======

[![Build Status](https://travis-ci.org/OEP/txtcmp.svg?branch=master)](https://travis-ci.org/OEP/txtcmp)

`txtcmp` is tool for finding similar text files. It is meant for the case where
you have many files and a few may be similar. It works by computing the longest
common subsequence (LCS) of all provided files, which is what is at work in
utilities like `diff`. It is not as useful in the case where you have just two
files. You should use `diff` for that.

Examples
--------

Compare three files

    txtcmp file1.txt file2.txt file3.txt

Find the most similar files in a directory

    find path/to/dir -type f | xargs txtcmp | sort -n | tail
