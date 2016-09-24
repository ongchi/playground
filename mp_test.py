#!/usr/bin/env python
'''
testing for multiprocessing tasks
'''
import multiprocessing as mp
import numpy as np
from itertools import product, imap


def fib(n):
    if n == 0: return 0
    elif n == 1: return 1
    else: return fib(n - 2) + fib(n - 1)


def f((n, x, y)):
    arr[n] = fib(x) * fib(y)


if __name__ == '__main__':
    l = range(22, 34)
    l2 = range(10, 20)
    arr_len = len(l) * len(l2)
    it = imap(lambda n, (x, y): (n, x, y), xrange(arr_len), product(l, l2))

    arr = mp.Array('d', arr_len)

    p = mp.Pool(processes=mp.cpu_count())
    p.map(f, it)
    p.close()
    p.join()

    a = np.array(arr).reshape(len(l), len(l2)).T
    print(a)
