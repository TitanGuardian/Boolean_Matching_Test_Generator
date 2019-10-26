#!/usr/bin/env python3
import fileinput
import os
import re
import sys


def handle_node(mystr):
    global counter
    global dictionary
    if (mystr.find('in') != -1) | (mystr.find('out') != -1):
        return mystr
    else:
        key_val = dictionary.get(int(mystr[1:]))
        if key_val is None:
            dictionary[int(mystr[1:])] = counter
            counter += 1
            return 'n' + str(counter-1)
        else:
            return 'n' + str(key_val)


if __name__ == "__main__":
    file = fileinput.FileInput(sys.argv[1], inplace=True, backup='.bak')

    dictionary = {}
    counter = 0

    for line in file:
        line = line.rstrip()  # remove trailing (invisible) space

        if line.find('assign') != -1:
            res = re.findall(r'assign.*\|.*;', line)
            if res:
                # or
                res = re.findall(r'assign\s+(\w+)\s*=\s*(\w+)\s*\|\s*(\w+)\s*;', line)
                if res:
                    # a = b | c
                    print('  or (' + handle_node(res[0][0]) + ', ' + handle_node(res[0][1]) + ', '
                          + handle_node(res[0][2]) + ');')
                else:
                    res = re.findall(r'assign\s+(\w+)\s*=\s*~\s*(\w+)\s*\|\s*(\w+)\s*;', line)
                    if res:
                        # a = ~b | c
                        notb = 'n' + str(counter)
                        counter += 1
                        print('  not ( '+notb + ', ' + handle_node(res[0][1]) + ');')
                        print('  or ( ' + handle_node(res[0][0]) + ', ' + notb + ', ' + handle_node(res[0][2]) + ');')
                    else:
                        res = re.findall(r'assign\s+(\w+)\s*=\s*(\w+)\s*\|\s*~\s*(\w+)\s*;', line)
                        if res:
                            # a = b | ~c
                            notc = 'n' + str(counter)
                            counter += 1
                            print('  not ( ' + notc + ', ' + handle_node(res[0][2]) + ');')
                            print('  or ( ' + handle_node(res[0][0]) + ', ' + handle_node(res[0][1]) + ', ' + notc
                                  + ');')
                        else:
                            res = re.findall(r'assign\s+(\w+)\s*=\s*~\s*(\w+)\s*\|\s*~\s*(\w+)\s*;', line)
                            if res:
                                # a = ~b | ~c
                                notb = 'n' + str(counter)
                                counter += 1
                                notc = 'n' + str(counter)
                                counter += 1
                                print('  not ( ' + notb + ', ' + handle_node(res[0][1]) + ');')
                                print('  not ( ' + notc + ', ' + handle_node(res[0][2]) + ');')
                                print('  or ( ' + handle_node(res[0][0]) + ', ' + notb + ', ' + notc + ');')

            else:
                res = re.findall(r'assign.*&.*;', line)
                if res:
                    # and
                    res = re.findall(r'assign\s+(\w+)\s*=\s*(\w+)\s*&\s*(\w+)\s*;', line)
                    if res:
                        # a = b & c
                        print('  and (' + handle_node(res[0][0]) + ', ' + handle_node(res[0][1]) + ', ' + handle_node(
                            res[0][2]) + ');')
                    else:
                        res = re.findall(r'assign\s+(\w+)\s*=\s*~\s*(\w+)\s*&\s*(\w+)\s*;', line)
                        if res:
                            # a = ~b & c
                            notb = 'n' + str(counter)
                            counter += 1
                            print('  not ( ' + notb + ', ' + handle_node(res[0][1]) + ');')
                            print('  and ( ' + handle_node(res[0][0]) + ', ' + notb + ', ' + handle_node(res[0][2])
                                  + ');')
                        else:
                            res = re.findall(r'assign\s+(\w+)\s*=\s*(\w+)\s*&\s*~\s*(\w+)\s*;', line)
                            if res:
                                # a = b & ~c
                                notc = 'n' + str(counter)
                                counter += 1
                                print('  not ( ' + notc + ', ' + handle_node(res[0][2]) + ');')
                                print('  and ( ' + handle_node(res[0][0]) + ', ' + handle_node(
                                    res[0][1]) + ', ' + notc + ');')
                            else:
                                res = re.findall(r'assign\s+(\w+)\s*=\s*~\s*(\w+)\s*&\s*~\s*(\w+)\s*;', line)
                                if res:
                                    # a = ~b & ~c
                                    notb = 'n' + str(counter)
                                    counter += 1
                                    notc = 'n' + str(counter)
                                    counter += 1
                                    print('  not ( ' + notb + ', ' + handle_node(res[0][1]) + ');')
                                    print('  not ( ' + notc + ', ' + handle_node(res[0][2]) + ');')
                                    print('  and ( ' + handle_node(res[0][0]) + ', ' + notb + ', ' + notc + ');')
                else:
                    res = re.findall(r'assign\s*(\w+)\s*=\s*~\s*(\w+)\s*;', line)
                    if res :
                        # not
                        print('  not (' + handle_node(res[0][0]) + ', ' + handle_node(res[0][1]) + ');')
                    else:
                        # buf
                        print('  buf (' + handle_node(res[0][0]) + ', ' + handle_node(res[0][1]) + ');')
        else:
            print(line)
    # print('lol' if line == '  assign' else line)  # stdout is redirected to the file
    # os.unlink(sys.argv[1]+ '.bak') # remove the backup on success
    file.close()


    file = fileinput.FileInput(sys.argv[1], inplace=True)

    flag = False
    flag1 = True
    for line in file:
        if flag1:
            if re.findall(r'wire',line):
                i = 1
                flag1 = False
                strx='  wire n0'
                while i<counter:
                     stra = ', n' + str(i)
                     if strx.__len__()+stra.__len__() > 80:
                         print (strx)
                         strx = '     ' + stra
                     else:
                         strx += stra
                     i += 1
                print (strx+';')
                if re.findall(r'.*;',line):
                    flag = False
                    continue
                else:
                    flag = True


        if flag:
            if re.findall(r'.*;',line):
                flag = False
        else:
            print(line.rstrip())
