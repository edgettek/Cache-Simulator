FILES := src/cache.c src/trace.c

cacheSim: $(FILES)
	gcc -o $@ $^ -O3

cacheSimDebug: $(FILES)
	gcc -o $@ $^ -g

clean:
	rm -rf cacheSimDebug.dSYM
	rm -f cacheSimDebug
	rm -f cacheSim

.PHONY : clean
