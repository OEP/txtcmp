#!/bin/sh

test_description='Checks valid outputs'
. ./sharness.sh

F="${SHARNESS_TEST_DIRECTORY}/f"

test_expect_success 'empty file' "
  echo '0 $F/empty.txt $F/empty.txt' >expected &&
  txtcmp $F/empty.txt $F/empty.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'empty file -n' "
  echo '0.000 $F/empty.txt $F/empty.txt' >expected &&
  txtcmp -n $F/empty.txt $F/empty.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'empty file and non-empty file' "
  echo '0 $F/one.txt $F/empty.txt' >expected &&
  txtcmp $F/one.txt $F/empty.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'empty file and non-empty file (reverse)' "
  echo '0 $F/empty.txt $F/one.txt' >expected &&
  txtcmp $F/empty.txt $F/one.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'one line and two line file' "
  echo '1 $F/one.txt $F/two.txt' >expected &&
  txtcmp $F/one.txt $F/two.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'one line and two line file -n' "
  echo '1.000 $F/one.txt $F/two.txt' >expected &&
  txtcmp -n $F/one.txt $F/two.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'multiple files' "
  cat <<EOF >expected &&
1 $F/one.txt $F/two.txt
1 $F/one.txt $F/three.txt
2 $F/two.txt $F/three.txt
EOF
  txtcmp $F/one.txt $F/two.txt $F/three.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'partial normalized' "
  cat <<EOF >expected &&
0.333 $F/animals1.txt $F/animals2.txt
EOF
  txtcmp -n $F/animals1.txt $F/animals2.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'spaces are normally honored' "
  cat <<EOF >expected &&
1 $F/numbers.txt $F/numbers_spaced.txt
EOF
  txtcmp $F/numbers.txt $F/numbers_spaced.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'spaces trimmed from ends with -t' "
  cat <<EOF >expected &&
2 $F/numbers.txt $F/numbers_spaced.txt
EOF
  txtcmp -t $F/numbers.txt $F/numbers_spaced.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'using -t with empty lines' "
  cat <<EOF >expected &&
4 $F/numbers.txt $F/numbers_blanks.txt
EOF
  txtcmp -t $F/numbers.txt $F/numbers_blanks.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'using -b ignores empty lines' "
  cat <<EOF >expected &&
4 $F/numbers_blanks.txt $F/numbers_blanks.txt
EOF
  txtcmp -b $F/numbers_blanks.txt $F/numbers_blanks.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'using -bt ignores empty lines' "
  cat <<EOF >expected &&
4 $F/numbers_blanks.txt $F/numbers_blanks.txt
EOF
  txtcmp -bt $F/numbers_blanks.txt $F/numbers_blanks.txt >actual &&
  test_cmp expected actual
"

test_expect_success 'using -b with dos newlines' "
  cat <<EOF >expected &&
4 $F/numbers_blanks_dos.txt $F/numbers_blanks_dos.txt
EOF
  txtcmp -b $F/numbers_blanks_dos.txt $F/numbers_blanks_dos.txt >actual &&
  test_cmp expected actual
"

test_done
