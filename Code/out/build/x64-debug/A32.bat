:: -----------------------------------------------------------------------
:: COMPILERS COURSE - SCRIPT                                             -
:: SCRIPT A12 - CST8152 - Summer 2023                                    -
:: -----------------------------------------------------------------------

CLS
SET COMPILER=Corrosion.exe

SET FILE0=INPUT1
SET FILE1=INPUT2
SET FILE2=INPUT3
SET FILE3=INPUT4

SET ASSIGNMENT=A32
SET EXTENSION=cor
SET OUTPUT=out
SET ERROR=err

SET PARAM=P

:: -----------------------------------------------------------------------
:: Begin of Tests (A22 - S22)                                            -
:: -----------------------------------------------------------------------

@echo off

ren *.exe %COMPILER%

::
:: BASIC TESTS  ----------------------------------------------------------
::
:: Basic Tests (A12 - S22) - - - - - - - - - - - - - - - - - - - - - -

%COMPILER% %PARAM% %FILE0%.%EXTENSION%	> %FILE0%-%ASSIGNMENT%.%OUTPUT%	2> %FILE0%-%ASSIGNMENT%.%ERROR%
%COMPILER% %PARAM% %FILE1%.%EXTENSION%	> %FILE1%-%ASSIGNMENT%.%OUTPUT%	2> %FILE1%-%ASSIGNMENT%.%ERROR%
%COMPILER% %PARAM% %FILE2%.%EXTENSION%	> %FILE2%-%ASSIGNMENT%.%OUTPUT%	2> %FILE2%-%ASSIGNMENT%.%ERROR%
%COMPILER% %PARAM% %FILE3%.%EXTENSION%	> %FILE3%-%ASSIGNMENT%.%OUTPUT%	2> %FILE3%-%ASSIGNMENT%.%ERROR%

:: SHOW OUTPUTS - - - - - - - - - - - - - - - - - - - - - - - - - - -
DIR *.OUT
DIR *.ERR

:: -----------------------------------------------------------------------
:: End of Tests (A22 - S22)                                              -
:: -----------------------------------------------------------------------
