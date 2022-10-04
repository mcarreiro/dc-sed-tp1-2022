[top]
components : router@router barrier@barrier manager@manager queue@queue transducer@transducer gate1@gate gate2@gate gate3@gate gate4@gate gate5@gate gate6@gate gate7@gate gate8@gate gate9@gate gate10@gate
out : out

link : out@router in@queue
link : outId@router arrived@transducer
link : out@queue in@barrier
link : throughput@transducer in@manager

link : done@barrier done@queue
link : done@barrier solved@transducer

link : out1@barrier fromBarrier@gate1
link : out2@barrier fromBarrier@gate2
link : out3@barrier fromBarrier@gate3
link : out4@barrier fromBarrier@gate4
link : out5@barrier fromBarrier@gate5
link : out6@barrier fromBarrier@gate6
link : out7@barrier fromBarrier@gate7
link : out8@barrier fromBarrier@gate8
link : out9@barrier fromBarrier@gate9
link : out10@barrier fromBarrier@gate10

link : toBarrier@gate1 done1@barrier
link : toBarrier@gate2 done2@barrier
link : toBarrier@gate3 done3@barrier
link : toBarrier@gate4 done4@barrier
link : toBarrier@gate5 done5@barrier
link : toBarrier@gate6 done6@barrier
link : toBarrier@gate7 done7@barrier
link : toBarrier@gate8 done8@barrier
link : toBarrier@gate9 done9@barrier
link : toBarrier@gate10 done10@barrier

link : out@gate1 out
link : out@gate2 out
link : out@gate3 out
link : out@gate4 out
link : out@gate5 out
link : out@gate6 out
link : out@gate7 out
link : out@gate8 out
link : out@gate9 out
link : out@gate10 out

link : out1@manager fromManager@gate1
link : out2@manager fromManager@gate2
link : out3@manager fromManager@gate3
link : out4@manager fromManager@gate4
link : out5@manager fromManager@gate5
link : out6@manager fromManager@gate6
link : out7@manager fromManager@gate7
link : out8@manager fromManager@gate8
link : out9@manager fromManager@gate9
link : out10@manager fromManager@gate10

[router]
meanPackets : 150

[queue]
preparation : 00:00:01:00

[transducer]
frecuence : 1:0:0:0
timeUnit : 1:0:0:0

[gate1]
startHour : 9 
endHour : 21
id: 1

[gate2]
startHour : 9 
endHour : 21 
id: 2

[gate3]
startHour : 9 
endHour : 21 
id: 3

[gate4]
startHour : 9 
endHour : 21 
id: 4

[gate5]
startHour : 12 
endHour : 18 
id: 5

[gate6]
startHour : 12 
endHour : 18
id: 6

[gate7]
startHour : 12 
endHour : 18
id: 7

[gate8]
startHour : 12 
endHour : 18 
id: 8

[gate9]
startHour : 12 
endHour : 18
id: 9

[gate10]
startHour : 12 
endHour : 18
id: 10
