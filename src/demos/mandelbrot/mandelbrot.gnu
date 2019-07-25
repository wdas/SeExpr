set title 'mandelbrot';
set key off;
set ylabel 'time (s)'
set boxwidth 0.5
set style fill solid
set style data histograms;
set terminal png size 1200, 800;
set output 'figure.png';
plot "plot.dat" using 1:3:xtic(2) with boxes
