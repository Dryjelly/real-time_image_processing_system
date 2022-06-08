# coding=utf-8
"""Performs face detection in realtime.

Based on code from https://github.com/shanren7/real_time_face_recognition
"""
# MIT License
#
# Copyright (c) 2017 François Gervais
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
from socket import *
import _pickle as pickle

import argparse
import sys
import time

import face

face_recog = face.Recognition()
print("activate testing...")

serverSock = socket(AF_INET, SOCK_STREAM)
serverSock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
serverSock.bind(('', 9090))
serverSock.listen(1)

connectionSock, addr = serverSock.accept()

print(str(addr),'에서 접속이 확인되었습니다.')


while 1:
	data_feat = connectionSock.recv(1024)
	data_feat = pickle.loads(data_feat)
	if data_feat == -999: break
	
	w = data_feat[0]
	h = data_feat[1]
	NoP = data_feat[2]
	print(w,h,NoP)

	temp2 = b''

	#while 1:
	#	temp1 = connectionSock.recv(2048)
	#	temp2 = temp2 + temp1
	#	if temp1[-1] == 46: break;
	#data = pickle.loads(temp2)
	connectionSock.settimeout(0.1)
	while 1:
		try:
			temp1 = connectionSock.recv(32768)
			temp2 = temp2 + temp1
		except:
			break
	connectionSock.settimeout(None)
	print(temp1[-1])
	print('데이터 수신 완료')
	data = pickle.loads(temp2)
	print(len(data[0]))

	#print('Length of data :',data[0][-1])
	#print(w, h, NoP)

	faces = face_recog.identify_psh_edit(data, w, h, NoP)
	result = -1

	for i, face in enumerate(faces):
		if face.name == "anchor":
			result = i
			break

	connectionSock.send(pickle.dumps(result))

serverSock.close()
print('soket closed')
