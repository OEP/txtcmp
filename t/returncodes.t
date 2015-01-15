#!/bin/sh

test_description='Makes sure basic return codes are correct'
. ./sharness.sh

test_expect_success '-h exits normally' '
  txtcmp -h
'

test_expect_success '-v exits normally' '
  txtcmp -v
'

test_expect_success 'garbage flags exit abnormally' '
  test_must_fail txtcmp -k
'

test_expect_success 'zero positional args fails' '
  test_must_fail txtcmp
'

test_expect_success 'one positional arg fails' '
  test_must_fail txtcmp foo
'

test_done
