[top]
components : barrier@barrier queue@queue transducer@transducer
in : in_port done1_port done2_port done3_port done4_port done5_port done6_port done7_port done8_port done9_port done10_port 
out : out1_port out2_port out3_port out4_port out5_port out6_port out7_port out8_port out9_port out10_port
link : out1@barrier out1_port
link : out2@barrier out2_port
link : out3@barrier out3_port
link : out4@barrier out4_port
link : out5@barrier out5_port
link : out6@barrier out6_port
link : out7@barrier out7_port
link : out8@barrier out8_port
link : out9@barrier out9_port
link : out10@barrier out10_port
link : done@barrier done@queue
link : in_port in@queue
link : in_port arrived@transducer
link : out@queue in@barrier
link : done1_port done1@barrier
link : done2_port done2@barrier
link : done3_port done3@barrier
link : done4_port done4@barrier
link : done5_port done5@barrier
link : done6_port done6@barrier
link : done7_port done7@barrier
link : done8_port done8@barrier
link : done9_port done9@barrier
link : done10_port done10@barrier

[queue]
preparation : 00:00:01:00

[transducer]
frecuence : 0:1:0:0
timeUnit : 0:1:0:0
