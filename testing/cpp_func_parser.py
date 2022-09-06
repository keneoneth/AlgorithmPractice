from argparse import ONE_OR_MORE, ZERO_OR_MORE
from pyparsing import *


# survey = '''GPS,PN1,LA52.125133215643,LN21.031048525561,EL116.898812 GPS,PN1,LA52.125133215643,LN21.031048525561,EL116.898812'''

# number = Word(nums+'.').setParseAction(lambda t: float(t[0]))
# separator = Suppress(',')
# latitude = Suppress('LA') + number
# longitude = Suppress('LN') + number
# elevation = Suppress('EL') + number

# line = (Suppress('GPS,PN1,')
#         + latitude
#         + separator
#         + longitude
#         + separator
#         + elevation)
# ret = line.scanString(survey)
# for elem in ret:
#     print(elem)

varname = Word(alphas, alphanums+"_")
param_elem = Word(alphanums+"_") + Optional(OneOrMore(Word("*")))
params = ZeroOrMore( OneOrMore(OneOrMore(param_elem) + Optional(",")) ).set_results_name("param")

openbracket = ("(")

closebracket = (")")

delimiter = Suppress(";")

cppfunction = (varname.set_results_name("func_type")+varname.set_results_name("func_name")+openbracket+params+closebracket+delimiter)

sample1 = '''void myfunc();'''

print(cppfunction.parseString(sample1))

sample2 = '''void myfunc(int a);'''

print(cppfunction.parseString(sample2))

sample3 = '''void myfunc(int a, int b);'''

print(cppfunction.parseString(sample3))

sample4 = '''void myfunc(int a, int b, float c);'''

print(cppfunction.parseString(sample4).as_dict())

sample5 = '''void myfunc(int *a, int** b, float * c, __RESTRICT__ double ** d);'''

print(cppfunction.parseString(sample5).as_dict())

