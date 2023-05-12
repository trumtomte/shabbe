##
# Shabbe (simple SHA-1 implementation)
#
# @version 0.1

shabbe: main.c
	$(CC) -o shabbe main.c -Wall -Wextra -pedantic -g3 -std=c11

.PHONY: clean

clean:
	rm shabbe

# end
