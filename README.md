![Screenshot 2024-04-05 at 9.09.35 PM.png](..%2F..%2F..%2F..%2FDesktop%2FScreenshot%202024-04-05%20at%209.09.35%20PM.png)

Some tips on how to run this.

# how to run this program
make

# run all examples ... assuming your executable is in ./
# make a outputdir
mkdir outputdir
./runit.sh outputdir ./scheduler

# compare outputs with reference output
# make sure that the frefout is the first argument
# because we are putting the LOG.txt into the outputdir

./gradeit.sh refout2 outputdir

You might get something like this.

frankeh@NYU2 > ./gradeit.sh ./refout ./studx
in    F    L    S   R2   R5   P2   P5:3 E2:5 E4
00    1    1    0    1    1    1    1    1    1
01    1    1    1    1    1    1    1    1    1
02    1    1    1    1    1    1    1    1    1
03    1    1    1    1    1    1    1    1    1
04    1    1    1    1    1    1    1    1    1
05    1    1    1    1    1    1    1    1    1
06    1    1    1    1    1    1    1    1    1
07    1    1    1    1    0    1    0    0    1

SUM   8    8    7    8    7    8    7    7    8 
68 out of 72 correct

frankeh@NYU2 > diff ./refout/out_0_S ./studx/out_0_S
2,4c2,4
< 0000:    0  100   10   10 2 |   201   201   101     0
< 0001:  500  100   20   10 4 |   627   127    27     0
< SUM: 627 31.90 20.41 164.00 0.00 0.319
---
> 0000:    0  100   10   10 2 |   209   201   101     0
> 0001:  500  100   20   10 4 |   622   127    27     0
> SUM: 627 31.90 20.41 164.00 0.00 0.301




