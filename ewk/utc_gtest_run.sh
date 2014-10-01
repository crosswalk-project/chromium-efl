#!/bin/bash

##  @file   utc_gtest_run.sh
##  @author Jongsoo Yoon <join.yoon@samsung.com>
##  @date   2014-02-20
##  @brief  This shell-script file is made to test all unit test cases automatically for Chromium EFL API

##  Set target environment
UTC_EXEC_PATH=/opt/usr/utc_exec
UTC_RESULTS_ROOT_DIR=/opt/usr/utc_results
UTC_ROOT=/usr/bin

set $(date +%y%m%d_%H%M%S)
FILE_NAME_EXTENSION=$1

UTC_RESULT_DIR=$UTC_RESULTS_ROOT_DIR/unittest-result-$FILE_NAME_EXTENSION
UTC_LOG_DIR=$UTC_RESULT_DIR-logs

## Echo setting values
echo UTC_EXEC_PATH=$UTC_EXEC_PATH
echo UTC_ROOT=$UTC_ROOT
echo UTC_RESULT_DIR=$UTC_RESULT_DIR

## Mkdir UTC DIRECTORYS
if [ -d $UTC_RESULTS_ROOT_DIR ]
then
  echo $UTC_RESULTS_ROOT_DIR
else
  echo make $UTC_RESULTS_ROOT_DIR
  mkdir $UTC_RESULTS_ROOT_DIR
fi
mkdir $UTC_RESULT_DIR
mkdir $UTC_LOG_DIR

## Run Unit Test for All
utc_exec=ewk_unittests
echo ====shell script==== ::: start test ::: $utc_exec
cp $UTC_EXEC_PATH/$utc_exec $UTC_ROOT
# some TCs need to get focus on webview, lockscreen is blocking focus
killall -s QUIT lockscreen
$UTC_ROOT/$utc_exec --gtest_output="xml:$UTC_RESULT_DIR/$utc_exec.xml" # | tee $UTC_LOG_DIR/$utc_exec.log
rm $UTC_ROOT/$utc_exec

if [ ! -f "$UTC_RESULT_DIR/$utc_exec.xml" ]; then
  test_name=${utc_exec%"_func"}
  ## Generate fake xml file that tells about possible segfault.
  echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "<testsuites tests=\"1\" failures=\"1\" disabled=\"0\" errors=\"0\" time=\"0.1\" name=\"Launch\">" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "  <testsuite name=\"$test_name\" tests=\"1\" failures=\"1\" disabled=\"0\" errors=\"0\" time=\"0.1\">" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "    <testcase name=\"LaunchTC\" status=\"run\" time=\"0.1\" classname=\"$test_name\">" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "      <failure message=\"SIGSEGV\" type=\"\"></failure>" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "    </testcase>" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "  </testsuite>" >> $UTC_RESULT_DIR/$utc_exec.xml
  echo "</testsuites>" >> $UTC_RESULT_DIR/$utc_exec.xml
fi

echo ====shell script==== :::  end test  ::: $utc_exec

## Set testsuites initial values
_testAPIs=0
_failureAPIs=0
_successAPIs=0
_totalTests=0
_success=0
_failures=0
_disabled=0
_errors=0
_time=0
_isFails=0
_name=AllTests
utc_report_xml_file_path=$UTC_RESULT_DIR.xml

## Parsing & Merging All tests xml files
for result_xml in  $UTC_RESULT_DIR/*.xml; do
  echo -e "\033[1;33;3m$result_xml\033[0m"
  _testAPIs=`expr $_testAPIs + 1`
  _isFails=`expr $_failures + $_disabled + $_errors`
  _totalTests=`expr $_totalTests + $(xmllint --xpath "string(//testsuites/@tests)" $result_xml)`
  _failures=`expr $_failures + $(xmllint --xpath "string(//testsuites/@failures)" $result_xml)`
  _disabled=`expr $_disabled + $(xmllint --xpath "string(//testsuites/@disabled)" $result_xml)`
  _errors=`expr $_errors + $(xmllint --xpath "string(//testsuites/@errors)" $result_xml)`
  _time=`echo $_time $(xmllint --xpath "string(//testsuites/@time)" $result_xml) | awk '{printf "%.3f", $1 + $2}'`

  if [ `expr $_failures + $_disabled + $_errors` -gt $_isFails ];then
    _isFails=`expr $_failures + $_disabled + $_errors`
    _failureAPIs=`expr $_failureAPIs + 1`
  fi

  echo >> $UTC_RESULT_DIR/temp_report
  xmllint --xpath //testsuite $result_xml >> $UTC_RESULT_DIR/temp_report
done
_success=`expr $_totalTests - $_isFails`
_successAPIs=`expr $_testAPIs - $_failureAPIs`

## Generate report xml file
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > $utc_report_xml_file_path
echo "<testsuites testAPIs=\"$_testAPIs\" successAPIs=\"$_successAPIs\" failureAPIs=\"$_failureAPIs\" tests=\"$_totalTests\" success=\"$_success\" failures=\"$_failures\" disabled=\"$_disabled\" errors=\"$_errors\" time=\"$_time\" name=\"$_name\">" >> $utc_report_xml_file_path
cat $UTC_RESULT_DIR/temp_report >> $utc_report_xml_file_path
echo >> $utc_report_xml_file_path
echo "</testsuites>" >> $utc_report_xml_file_path

## End Unit Test
#rm $UTC_RESULT_DIR -rf

echo -e "\033[1;33;3mend unit test..\033[0m"
echo -e "\033[1;33;3mxml report file is located in \"\033[1;32;4m$utc_report_xml_file_path\033[1;33;3m\"\033[0m"
