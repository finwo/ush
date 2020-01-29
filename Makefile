ush: linenoise.c ush.c
	clang -Os $^ -o $@

clean:
	rm ush
