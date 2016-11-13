#!/usr/bin/python2
# -*- coding: utf-8 -*-

import subprocess
import os
import sys
import glob
import re

def shellExec(cmd):
	errCode = subprocess.call(cmd, shell=True)
	if errCode != 0:
		fatalError('failed executing: ' + cmd)

def fatalError(message):
	print('[ERROR] ' + message)
	sys.exit()

def removeIfExists(fileName):
	if os.path.exists(fileName):
		print('removing ' + fileName + '...')
		os.remove(fileName)

def removeFilesWildcard(pattern):
	files = glob.glob(pattern)
	for file in files:
		os.remove(file)

def getVersionName(filename):
	f = open(filename, 'r')
	for line in f:
		search = re.search('#define VERSION "(.*)"', line)
		if search:
			versionName = search.group(1)
			return versionName
	fatalError('version number was not found')
	return None;

# nazwy plików i folderów
guitarDBDir = 'guitarDB'
execName = 'SongBook.exe'
execDir = '../bin/'
configFileName = 'conf.ini'
versionFile = '../version.h'

# usuwanie starszych wersji
print('removing older versions...')
removeFilesWildcard('./SongBook-win-*.zip')


# wyciągnięcie numeru wersji
versionName = getVersionName(versionFile)
print('version number: %s' % versionName)

# pliki wyjściowe
outputArchive = 'SongBook-win-' + versionName + '.zip'

# aktualizacja najnowszej binarki
print('getting latest bin...')
shellExec('cp '+execDir+execName+' ' + execName)

# aktualizacja zmian z bazy źródłowej
print('updating ' + guitarDBDir + '...')
os.chdir(guitarDBDir)
shellExec('git pull origin release-en')
os.chdir('..')

# spakowanie
print('zipping...')
shellExec('zip -r '+outputArchive+' '+execName+' '+configFileName+' '+guitarDBDir+' -x *.git*')

print('done')
