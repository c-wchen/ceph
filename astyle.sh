#!/bin/bash

CURRENT_PATH=$(pwd)
SCAN_PATH=${CURRENT_PATH}/src
SCAN_SUFFIX=*.c,*.h,*.cpp,*.cc
ASTYLE_BIN=/usr/bin/astyle


# 内部功能
SCAN_SUFFIX_ARR=(${SCAN_SUFFIX//,/ })
SCAN_PATH_ARR=(${SCAN_PATH//,/ })
SCAN_SUFFIX_ARR_SIZE=${#SCAN_SUFFIX_ARR[@]}


for((i=0;i<${SCAN_SUFFIX_ARR_SIZE};i++))
do
    FIND_COND="${FIND_COND} -name ${SCAN_SUFFIX_ARR[$i]}"
    if [ $i -eq `expr $SCAN_SUFFIX_ARR_SIZE - 1` ]
    then
        continue
    else
        FIND_COND="$FIND_COND -or "
    fi
done

echo "$FIND_COND"

ASTYLE_CMD_OPTION="                   \
--style=k&r                           \
--indent=spaces=4                     \
--attach-extern-c                     \
--attach-closing-while                \
--indent-switches                     \
--min-conditional-indent=0            \
--pad-oper                            \
--pad-comma                           \
--pad-header                          \
--unpad-paren                         \
--align-pointer=name                  \
--break-one-line-headers              \
--add-braces                          \
--attach-return-type                  \
--attach-return-type-decl             \
--convert-tabs                        \
--max-code-length=120                 \
--break-after-logical                 \
--mode=c                              \
--suffix=none                         \
"

for path in ${SCAN_PATH_ARR[@]}
do
    found=$(find ${path} ${FIND_COND})
    ${ASTYLE_BIN} ${ASTYLE_CMD_OPTION} $found
done
