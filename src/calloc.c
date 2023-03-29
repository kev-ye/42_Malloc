#include "malloc.h"


void	*calloc(size_t count, size_t size) {
	void	*ptr = NULL;

	if (count == 0 || size <= 0) {
        count = 1;
        size = 1;
    }

    if (DEBUG) {
		ft_putstr_fd("\n-- "S_CYAN"Calloc()"S_NONE" called: ", STDOUT_FILENO);
		ft_putstr_fd("["S_GREEN, STDOUT_FILENO);
		ft_putnbr_fd(count, STDOUT_FILENO, 0);
        ft_putstr_fd("* "S_GREEN, STDOUT_FILENO);
        ft_putnbr_fd(size, STDOUT_FILENO, 0);
		ft_putstr_fd(S_NONE" bytes]", STDOUT_FILENO);
		ft_putstr_fd("\n", STDOUT_FILENO);
	}

	if ((ptr = malloc(count * size)))
		ft_memset(ptr, 0, count * size);

    if (DEBUG) {
		ft_putstr_fd("  -> "S_CYAN"Calloc()"S_NONE" returned:", STDOUT_FILENO);
		ft_putstr_fd("["S_GREEN, STDOUT_FILENO);
		ft_putaddr_fd(ptr, STDOUT_FILENO);
		ft_putstr_fd(S_NONE"]\n\n", STDOUT_FILENO);
	}
	return (ptr);
}
