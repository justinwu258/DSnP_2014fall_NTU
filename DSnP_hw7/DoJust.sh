make
#./taskMgr -f doJ2 &> j2.txt
#./ref/taskMgr -f doJ2 &> h2.txt
#diff j2.txt h2.txt
#echo "----- J2 done -----"
./taskMgr -f doJust &> jJust.txt
#./ref/taskMgr -f doJust &> hJust.txt
#diff jJust.txt hJust.txt
vi jJust.txt
echo "----- Just done -----"
#./taskMgr -f doJ3 &> j3.txt
#./ref/taskMgr -f doJ3 &> h3.txt
#diff j3.txt h3.txt
#echo "----- J3 done -----"
#./taskMgr -f doDelMin &> jDelMin.txt
#./ref/taskMgr -f doJ3 &> h3.txt
#diff j3.txt h3.txt
#vi jDelMin.txt
#echo "----- delMin done -----"
