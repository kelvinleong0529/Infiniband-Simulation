# -*- coding: utf-8 -*-
import sys
import os
import numpy as np

vecfile = []
scafile = []


def formate_filename(filename: str, deep: int = 0) -> str:
    tab = ''
    d = 0
    while d < deep:
        tab += '  '
        d += 1
    return tab + os.path.basename(filename)


def list_dir(dirname: str, deep: int = 0) -> None:
    if not os.path.exists(dirname):
        print(dirname, 'is not existed')
        sys.exit(1)

    if os.path.isfile(dirname):
        if "vec" in dirname:
            vecfile.append(dirname)
        if "sca" in dirname:
            scafile.append(dirname)
        # print dirname
        # print formate_filename(dirname, deep)

    if os.path.isdir(dirname):
        # print formate_filename(dirname, deep) + ":"
        # 列出目录的所有文件和子目录
        filenames = os.listdir(dirname)
        for filename in filenames:
            list_dir(dirname + os.sep + filename, deep + 1)


def averagenum(num: int) -> int:
    nsum = 0
    for i in range(len(num)):
        nsum += num[i]
    return nsum / len(num)


def parse_vector(filename: str) -> None:
    print('----------------------------------------------------------')
    linenum = 0
    print(filename)
    latency = []
    smallmessage = []
    latencyl = []
    largemessage = []
    with open(filename) as f:
        for line in f.readlines():
            lines = []
            lines = line.split('\t')
            if "Latency" in line:
                lines = line.split(' ')
                smallmessage.append(lines[1])
            if "Large" and "latency" in line:
                lines = line.split(' ')
                largemessage.append(lines[1])
            lines = line.split('\t')
            if len(lines) == 4 and lines[0] in smallmessage:
                latency.append(float(lines[3]))
            if len(lines) == 4 and lines[0] in largemessage:
                latencyl.append(float(lines[3]))
            linenum = linenum + 1
    # latency.sort()
    if len(latency) > 0:
        print('number of SMALL messages: ' + str(len(latency)))
    print('number of LARGE messages: ' + str(len(latencyl)))
    '''
	if len(latency) % 2 == 0:
		print 'average of latency: ' + str((latency[len(latency)/2]+latency[len(latency)/2-1])/2.0)
	else:
		print 'average of latency: ' + str(latency[len(latency)/2])
	'''
    if len(latency) > 0:
        print('average latency of SMALL messages: ' + str(averagenum(latency)))
    print('average latency of LARGE messages: ' + str(averagenum(latencyl)))
    # print 'max latency of SMALL messages: ' + str(max(latency))
    # print 'min latency of SMALL messages: ' + str(min(latency))
    if len(latency) > 0:
        print(np.percentile(latency, 99.9))  # 95%分位数
    print(np.percentile(latencyl, 99.9))


def parse_vector2(filename: str) -> None:
    print('----------------------------------------------------------')
    linenum = 0
    print(filename)
    flag = 0
    latencysum = 0
    latency = []
    largemessage = []
    with open(filename) as f:
        for line in f.readlines():
            lines = []
            lines = line.split('\t')
            if "Large" in line:
                lines = line.split(' ')
                largemessage.append(lines[1])
            lines = line.split('\t')
            if len(lines) == 4 and lines[0] in largemessage:
                latency.append(float(lines[3]))
            linenum = linenum + 1
    # latency.sort()
    print('number of LARGE messages: ' + str(len(latency)))
    print('average latency of LARGE message: ' + str(averagenum(latency)))
    print('max latency: ' + str(max(latency)))
    print('min latency: ' + str(min(latency)))
    print(np.percentile(latency, 99.9))


def parse_scala(filename: str) -> None:
    print('----------------------------------------------------------')
    print(filename)
    linenum = 0
    throughput = []
    utilization = []
    fraction = []
    with open(filename) as f:
        for line in f.readlines():
            lines = []
            lines = line.split(' ')
            # print lines
            # throughput data
            if "throughput" in line and "scalar" in line:
                throughput.append(float(lines[len(lines)-1]))
            # utilization data
            if "utilization" in line and "scalar" in line:
                utilization.append(float(lines[len(lines)-1]))
            if "fraction" in line and "scalar" in line:
                fraction.append(float(lines[len(lines)-1]))
            linenum = linenum + 1
    throughput.sort()
    print('number of senders: ' + str(len(throughput)))
    '''
	if len(throughput) % 2 == 0:
		print 'average of throughput: ' + str((throughput[len(throughput)/2]+throughput[len(throughput)/2-1])/2.0)
	else:
		print 'average of throughput: ' + str(throughput[len(throughput)/2])
	'''
    print('average of throughput: ' + str(averagenum(throughput)))
    print('max throughput: ' + str(max(throughput)))
    print('min throughput: ' + str(min(throughput)))
    utilization.sort()
    print('number of receivers: ' + str(len(utilization)))
    '''
	if len(utilization) % 2 == 0:
		print 'average of utilization: ' + str((utilization[len(utilization)/2]+utilization[len(utilization)/2-1])/2.0)
	else:
		print 'average of utilization: ' + str(utilization[len(utilization)/2])
	'''
    print('average of utilization: ' + str(averagenum(utilization)))
    print('max utilization: ' + str(max(utilization)))
    print('min utilization: ' + str(min(utilization)))
    if len(fraction) > 0:
        print('average of cnp fraction: ' + str(averagenum(fraction)))
        print('max cnp fraction: ' + str(max(fraction)))
        print('min cnp fraction: ' + str(min(fraction)))


if len(sys.argv) < 2:
    print('you should input the dirname')
    sys.exit(1)

del sys.argv[0]
for dirname in sys.argv:
    list_dir(dirname)
    print

# parse vec file
for v in vecfile:
    parse_vector(v)

for s in scafile:
    parse_scala(s)
