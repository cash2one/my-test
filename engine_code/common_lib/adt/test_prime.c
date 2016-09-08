#include <stdlib.h>
#include <stdio.h>

static int is_prime(unsigned long int candidate)
{
	/* No even number and none less than 10 will be passed here.  */
	unsigned long int divn = 3;
	unsigned long int sq = divn * divn;

	while (sq < candidate && candidate % divn != 0) {
		++divn;
		sq += 4 * divn;
		++divn;
	}

	return candidate % divn != 0;
}

#define MIN_PRIME 53
unsigned long int next_prime(unsigned long int seed)
{
	if (seed < MIN_PRIME) {
		return MIN_PRIME;
	}

	// compute the length of seed
	unsigned long power_two = 0;
	unsigned long tmp_seed = seed;
	while (tmp_seed > 0) {
		tmp_seed >>= 1;
		power_two++;
	}

	power_two = (1 << power_two);

	unsigned long int new_seed = 0;

	do {
		/* Make it definitely odd.  */
		new_seed = ((power_two + (power_two >> 1)) >> 1) | 1;
		while (!is_prime(new_seed)) {
			new_seed += 2;
		}

		if (new_seed < seed) {
			power_two <<= 1;
		}
	} while (new_seed < seed);

	return new_seed;
}

int main(int argc, char **argv)
{
	unsigned long int ori = strtoul(argv[1], 0, 0);
	printf("ori: %d, dst: %d\n", ori, next_prime(ori));

	return 0;
}
