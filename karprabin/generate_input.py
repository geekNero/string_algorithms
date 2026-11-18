import random

s = ""

n = int(input("Enter string length: "))

for i in range(n):
    s += str(random.randint(0,9))

print(s)
