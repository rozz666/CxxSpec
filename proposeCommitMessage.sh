git diff HEAD | grep +TEST | sed -e 's/\+TEST_*F*(\(.*\)Test *, *\(.*\)).*/\1: \2/'
