lazy: lazy.o rule.o itemset.o cache.o test.o cost.o score.o training.o core.o unlabeled.o evaluation.o
	g++ -Wall -O3 -o lazy lazy.o rule.o itemset.o cost.o cache.o test.o score.o training.o core.o unlabeled.o evaluation.o -lm

lazy.o: lazy.c rule.c itemset.c criteria.h limits.h
	g++ -Wall -O3 -c lazy.c

rule.o: rule.c rule.h criteria.h limits.h
	g++ -Wall -O3 -c rule.c

cost.o: cost.c cost.h limits.h
	g++ -Wall -O3 -c cost.c

itemset.o: itemset.c itemset.h limits.h
	g++ -Wall -O3 -c itemset.c

cache.o: cache.c cache.h limits.h
	g++ -Wall -O3 -c cache.c

test.o: test.c test.h limits.h
	g++ -Wall -O3 -c test.c

score.o: score.c score.h limits.h
	g++ -Wall -O3 -c score.c

training.o: training.c training.h limits.h
	g++ -Wall -O3 -c training.c

core.o: core.c core.h limits.h
	g++ -Wall -O3 -c core.c

evaluation.o: evaluation.c evaluation.h limits.h
	g++ -Wall -O3 -c evaluation.c

unlabeled.o: unlabeled.c unlabeled.h
	g++ -Wall -O3 -c unlabeled.c

clean:
	rm -f *.o lazy
