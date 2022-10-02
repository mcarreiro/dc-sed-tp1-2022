[top]
components : gate@gate
in : fromBarrier fromManager
out : out_port toBarrier
link : out@gate out_port
link : toBarrier@gate toBarrier
link : fromBarrier fromBarrier@gate
link : fromManager fromManager@gate