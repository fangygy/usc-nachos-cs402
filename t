if [ -n "$1" ]
then
  nachos -P2 -d c -rs $1 | egrep "DEBUGINFO|Number of Group" | less
else 
  nachos -P2 -d c -rs 10 | egrep "DEBUGINFO|Number of Group" | less
fi
