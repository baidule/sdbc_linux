w=`date +%w`
grep "TIMEVAL=" <$LOG/scli$w.log |./statas

