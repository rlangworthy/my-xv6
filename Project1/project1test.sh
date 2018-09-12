echo Project 1 Tests
echo Simple Sequential Commands Test
echo First;echo Second;echo Third;echo Fourth;echo Fifth
echo 
echo Simple Parens Test
echo Second;(echo First);echo Third;echo Fourth;echo Fifth
echo 
echo List Inside Parens Test
echo Third;(echo First;echo Second);echo Fourth;echo Fifth
echo 
echo Nested Parens Tests
echo Test 1
echo Third;(echo Second;(echo First));echo Fourth;echo Fifth
echo Test 2
echo Third;((echo First);echo Second);echo Fourth;echo Fifth
echo Test 3
echo Fourth;(echo Second;(echo First);echo Third);echo Fifth
echo 
echo Sequential Parens Test
echo Test 1
echo Fourth;(echo Third;(echo First);(echo Second));echo Fifth
echo Test 2
echo Third;(echo First);(echo Second);echo Fourth;echo Fifth
