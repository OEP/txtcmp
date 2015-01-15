#!/bin/sh

test_description='Checks valid outputs'
. ./sharness.sh

F="${SHARNESS_TEST_DIRECTORY}/f"

test_expect_success 'empty file' "
  echo '0 $F/empty.txt $F/empty.txt' >expected &&
  txtcmp $F/empty.txt $F/empty.txt >actual &&
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

test_expect_success 'multiple files' "
  cat <<EOF >expected &&
1 $F/one.txt $F/two.txt
1 $F/one.txt $F/three.txt
2 $F/two.txt $F/three.txt
EOF
  txtcmp $F/one.txt $F/two.txt $F/three.txt >actual &&
  test_cmp expected actual
"

test_done
