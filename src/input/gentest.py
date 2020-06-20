from random import randint as R
import random
import pickle
import re
import collections
import os
import pprint
import sys


def getallkw():
    if os.path.exists('kw.pickle'):
        kw = pickle.load(open('kw.pickle', 'rb'))
        return kw

    kw = collections.defaultdict(list)
    for entry in os.scandir('../test_data'):
        print(entry.path)
        with open(entry.path) as file:
            for i, line in enumerate(file):
                line = line.strip('\n').lower()
                if i == 0:
                    kw['from'].append(line.split(': ')[1])
                elif i == 4:
                    kw['to'].append(line.split(': ')[1])
                elif i == 3 or i == 6:
                    if i == 3:
                        content = line.split(': ')[1]
                    else:
                        content = line
                        while True:
                            try:
                                content += next(file).lower().strip('\n')
                            except StopIteration:
                                break
                    for m in re.finditer(r'[0-9a-zA-Z]+', content):
                        kw['content'].append(m.group(0).lower())
    pickle.dump(kw, open('kw.pickle', 'wb'))
    return kw


def genexpr(kw, endprob, probgrad):
    res = ''
    if R(0, 1):
        res += '!'

    if random.random() < endprob:
        res += random.choice(kw['content'])
        return res

    par = R(0, 1)
    if par:
        res += '('

    res += genexpr(kw, endprob*probgrad, probgrad)
    res += random.choice(['&', '|'])
    res += genexpr(kw, endprob*probgrad, probgrad)

    if par:
        res += ')'
    return res


def gentest(kw, numtest, endprob, probgrad):
    cmds = ['add', 'remove', 'longest', 'query']
    cmdw = [800, 200, 400, 1000]
    mails = []

    for i in range(numtest):
        cmd = random.choices(cmds, weights=cmdw, k=1)[0]
        if cmd == 'add':
            mailid = R(1, 10000)
            mails.append(mailid)
            print(cmd, 'test_data/mail' + str(mailid))
        elif cmd == 'remove':
            mailid = random.choice(mails)
            print(cmd, mailid)
        elif cmd == 'query':
            qs = ''
            if not R(0, 1):
                datefrom = [R(2000, 2020), R(1, 12),
                            R(1, 31), R(0, 23), R(0, 59)]
                dateto = [R(2000, 2020), R(1, 12), R(
                    1, 31), R(0, 23), R(0, 59)]
                if datefrom > dateto:
                    datefrom, dateto = dateto, datefrom
                qs += ' -d'
                if not R(0, 10):
                    qs += ''.join(map(str, datefrom))
                qs += '~'
                if not R(0, 10):
                    qs += ''.join(map(str, dateto))
            if R(0, 1):
                from_ = random.choice(kw['from'])
                assert len(from_) < 50
                qs += ' -f"{}"'.format(from_)
            if R(0, 1):
                to = random.choice(kw['to'])
                assert len(to) < 50
                qs += ' -t"{}"'.format(to)
            expr = genexpr(kw, endprob, probgrad)
            qs += ' ' + expr
            print('query'+qs)
        elif cmd == 'longest':
            print('longest')
        else:
            print(f'{cmd=}')


if len(sys.argv) < 4:
    print('usage: gentest.py #command initprob=0.1 probgrad=1.4')
    exit(1)

kw = getallkw()
gentest(kw, int(sys.argv[1]), float(sys.argv[2]), float(sys.argv[3]))
