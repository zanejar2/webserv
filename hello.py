#!/usr/bin/env python

import random
import string

def generate_random_string(length):
    characters = string.ascii_letters + string.digits
    return ''.join(random.choice(characters) for _ in range(length))

def print_cgi_headers():
    print()

def main():
    print_cgi_headers()
    random_string = generate_random_string(10)  # Change the number inside the parentheses to set the desired length of the random string
    print(f"<h1>Random String:</h1><p>{random_string}</p>")

if __name__ == "__main__":
    main()
