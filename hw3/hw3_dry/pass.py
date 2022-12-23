hash = 0x939f103
password = ""
length = 6

for i in range(length):
    letter = (hash // (26**(length-1-i))) + ord('a')
    password += chr(letter)
    hash -= (letter - ord('a'))*(26**(length-1-i))

print(password)