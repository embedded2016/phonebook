reset
set ylabel 'time(sec)'
set style fill solid
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [:][:0.110]'output.txt' \
using 2:xtic(1) with histogram title 'original', \
'' using ($0-0.4):($2+0.002):2 with labels title '', \
'' using 3:xtic(1) with histogram title 'optimized'  , \
'' using ($0-0.06):($3+0.001):3 with labels title '', \
'' using 4:xtic(1) with histogram title 'hash1' , \
'' using ($0+0.06):($4+0.001):4 with labels title '', \
'' using 5:xtic(1) with histogram title 'hash2' , \
'' using ($0+0.2):($5+0.001):5 with labels title '', \
'' using 6:xtic(1) with histogram title 'thread1' , \
'' using ($0+0.40):($6+0.001):6 with labels title '', \
'' using 7:xtic(1) with histogram title 'thread2' , \
'' using ($0+0.55):($7+0.001):7 with labels title ''


reset
set ylabel 'time(sec)'
set style fill solid
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime1.png'

plot [:][:0.110]'output.txt' \
using 2:xtic(1) with histogram title 'original', \
'' using ($0-0.4):($2+0.002):2 with labels title '', \
'' using 3:xtic(1) with histogram title 'optimized' , \
'' using ($0-0.06):($3+0.001):3 with labels title '', \
'' using 4:xtic(1) with histogram title 'hash1' , \
'' using ($0+0.06):($4+0.001):4 with labels title '', \
'' using 6:xtic(1) with histogram title 'thread1' , \
'' using ($0+0.40):($6+0.001):6 with labels title ''


reset
set ylabel 'time(sec)'
set style fill solid
set title 'perfomance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime2.png'

plot [:][:0.110]'output.txt' \
using 2:xtic(1) with histogram title 'original', \
'' using ($0-0.4):($2+0.002):2 with labels title '', \
'' using 3:xtic(1) with histogram title 'optimized' , \
'' using ($0-0.06):($3+0.001):3 with labels title '', \
'' using 5:xtic(1) with histogram title 'hash2' , \
'' using ($0+0.2):($5+0.001):5 with labels title '', \
'' using 7:xtic(1) with histogram title 'thread2' , \
'' using ($0+0.55):($7+0.001):7 with labels title ''
