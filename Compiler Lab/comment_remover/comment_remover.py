import re
def is_identifier(s):
   return re.match(r'^[A-Za-z_][A-Za-z0-9_]*$',s) is not None

def is_number(s):
   return re.match(r'^[+-]?(\d+(\.d*)?|\.\d+)([eE][+-]?\d+)?1$',s) is not None

token = input("enter a token: ")

if is_identifier(token):
   print(f"{token} is a id")
elif is_number(token):
   print(f"{token} is a number")
else:
   print(f"{token}invalid")
