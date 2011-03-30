#
# SConscript for cifparse-obj-v7.0
# Created: Aug 22, 2006 - Jdw
#
# Updated:  Mar 30, 2011 jdw - Clone environment
## 
import sys, os.path
Import('env')
env=env.Clone()
#
#if (len(env.subst('$MYDEBUG')) > 0):
#	dict = env.Dictionary()
#	for k,v in dict.items():
#		print  k, " = ", str(v)
#
#print "SConscript - in cifparse-obj-v7.0"
env1 = env.Clone()
myOs = sys.platform

env1.Replace(LEXFLAGS = '-Cfr -L -Pcifparser_')
env1.Replace(YACCFLAGS = '-d -v -l -p cifparser_')
s1 = env1.CFile(target = 'src/CifParser.c',  source='src/CifParser.y')
#
#print s1[0]
#print s1[1]
#  
dst1= os.path.join(str(env.subst('$MY_INCLUDE_INSTALL_PATH')),'CifParser.h')
if (myOs == "darwin"):
#    src1 = str(s1[0]) + '.h'	
    src1 = 'src/CifParser.H'
elif (myOs == "linux"):
#    src1 = 'src/CifParser.H'
    src1 = 'src/CifParser.h'
else:
    src1 = 'src/CifParser.h'
#
#print src1
#print dst1
#
mv1  = Move(File(dst1).abspath,File(src1).abspath)
env1.AddPostAction(s1, mv1)
#
s2 = env1.CFile(target = 'src/CifScanner.c', source='src/CifScanner.l')
#
#
env2 = env.Clone()
env2.Replace(LEXFLAGS = '-Cfr -L -Pdicparser_')
env2.Replace(YACCFLAGS = '-d -v -l -p dicparser_')
s3 = env2.CFile(target = 'src/DICParser.c',  source='src/DICParser.y')
dst3=  os.path.join(str(env.subst('$MY_INCLUDE_INSTALL_PATH')),'DICParser.h')
if (myOs == "darwin"):
#    src3 = str(s3[0]) + '.h'
    src3 = 'src/DICParser.H'
elif (myOs == "linux"):
#    src3 = 'src/DICParser.H'
    src3 = 'src/DICParser.h'
else:
    src3 = 'src/DICParser.h'
#
mv3  = Move(File(dst3).abspath,File(src3).abspath)
env2.AddPostAction(s3, mv3)
#
s4 = env2.CFile(target = 'src/DICScanner.c', source='src/DICScanner.l')
#
#
libName = 'cifparse-obj'
libSrcList =['src/CifFileReadDef.C',
			 'src/CifParser.c',
			 'src/CifScanner.c',
			 'src/CifScannerBase.C',
			 'src/CifParserBase.C',
			 'src/DICScanner.c',
			 'src/DICScannerBase.C',
			 'src/DICParser.c',
			 'src/DICParserBase.C']

#libObjList = [s.replace('.C','.o') for s in libSrcList]
libObjList = []
for s in libSrcList:
	if s.endswith('.c'):
		libObjList.append(s.replace('.c','.o'))
	elif s.endswith('.C'):
		libObjList.append(s.replace('.C','.o'))
	else:
		pass


libIncList =['include/CifFileReadDef.h',
			 'include/CifParserBase.h',
			 'include/CifParserInt.h',
			 'include/CifScannerBase.h',
			 'include/CifScannerInt.h',
			 'include/DICParserBase.h',
			 'include/DICParserInt.h',
			 'include/DICScannerBase.h',
			 'include/DICScannerInt.h']

myLib=env.Library(libName,libSrcList)
#
env.Install(env.subst('$MY_LIB_INSTALL_PATH'),myLib)
env.Alias('install-lib',env.subst('$MY_LIB_INSTALL_PATH'))
#
env.Install(env.subst('$MY_INCLUDE_INSTALL_PATH'),libIncList)
env.Alias('install-include',env.subst('$MY_INCLUDE_INSTALL_PATH'))
#
env.Install(env.subst('$MY_OBJ_INSTALL_PATH'),libObjList)
env.Alias('install-obj',env.subst('$MY_OBJ_INSTALL_PATH'))
#
env.Default('install-include','install-obj','install-lib')
#
