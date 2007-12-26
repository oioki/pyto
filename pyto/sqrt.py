a = 62.0
x = 1.0
d = 1.0
while ( d > 0.001 ):
	x = (x+a/x)/2
	d = (x*x-a)/2/a
	if d<0: d=-d
