time ./cirTest -f testall_noO.do2 &> usr.ref #2>&1 
time ./ref/cirTest -f testall_noO.do2 &> ref.ref #2>&1
vimdiff usr.ref ref.ref
