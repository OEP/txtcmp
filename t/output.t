#!/bin/sh

test_description='Checks valid outputs'
. ./sharness.sh

F="${SHARNESS_TEST_DIRECTORY}/f"

test_expect_success 'empty file' "
  echo '0 $F/empty.txt $F/empty.txt' >expected &&
  txtcmp $F/empty.txt $F/empty.txt >actual &&
  test_cmp expected actual
"

test_done
