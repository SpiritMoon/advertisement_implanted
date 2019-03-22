#!/bin/sh
set -e
#��Ϊkide��jar�ж�������������˶��壬�����ڽű���ʹ��ls���ֿ��·��������������⣬����ķ�������
#�ڽű��ڲ�ȡ����������Ķ���
unset LD_LIBRARY_PATH

CURPWD=`pwd`

##############################################################
##### ����������Ϣ��ʾ
##############################################################
ERR_INFO()
{
	echo "This Script extract libs of the toolchain to the dir specified by PREFIX."
	echo "USAGE:"
	echo "Example:GetSharelib TARGET=/opt/zte/ppc_gcc4.1.2_glibc2.5.0_multilib SUB_TARGET=powerpc-e500v2-linux-gnu PREFIX=/opt/zte/target_lib"
	echo "        GetSharelib PREFIX=/opt/zte/target_lib" SUB_TARGET=powerpc-e500v2-linux-gnu
	echo "        GetSharelib PREFIX=/opt/zte/target_lib" 
	echo
	echo "        TARGET��    TARGET is top dir of toolchain. It can be omitted, if you run this script in toolchain/bin."
	echo "        SUB_TARGET��SUB_TARGET is target of toolchain for multilib. "
	echo "                    SUB_TARGET options: [${SUPPORT_TARGET}]"
	echo "        PREFIX��    PREFIX is the place to put shared lib,it can be relative path."
	exit
}

##############################################################
##### ��ȡ�������
##############################################################
PROGRAM=$0
while [ $# -gt "0" ] 
do
	param=$1
	if [ "TARGET=" = "${param:0:7}" ];then
		TARGET_DIR=${param##*=}
	else
		if [ "PREFIX=" = "${param:0:7}" ];then
			PREFIX_DIR=${param##*=}		
		else
			if  [ "SUB_TARGET=" = "${param:0:11}" ]; then
				SUB_TARGET=${param##*=}							
			fi
		fi
	fi
	shift
done

if [ "${TARGET_DIR}_" = "_" ]; then
	TARGET_DIR=${PROGRAM%/*}/..
	TARGET_DIR=`cd ${TARGET_DIR}; pwd`
fi

##############################################################
##### ��֤PREFIX
##############################################################
if [ "${PREFIX_DIR}_" = "_" ] ; then
	echo "PREFIX is err,please check it"
	ERR_INFO
fi

##############################################################
##### ��֤TARGET
##############################################################
if [ ! -d ${TARGET_DIR} ] ; then
	echo "TARGET_DIR not exist,please check the value TARGET"
	ERR_INFO
fi

if [ ! -d ${TARGET_DIR}/bin ] ; then
	echo "fail to find toolchain,please check the value TARGET"
	ERR_INFO
fi

##############################################################
##### ��֤MULTILIB
##############################################################
case ${MACHTYPE} in
	*cygwin*)
		EXE=".exe";;
	*)
		EXE="";;
esac
CC=`find ${TARGET_DIR}/bin -name "*-gcc${EXE}"`
if [ "${CC}_" = "_" ]; then
	echo "fail to find toolchain,please check the value TARGET"
	ERR_INFO
fi
CC=`echo ${CC} | awk '{print $1}'` 

TARGET=`${CC} -dumpmachine`
LIB_LIST=`${CC} -print-multi-lib | awk -F ";" '{print $1}'`
LIB_LIST=`echo ${LIB_LIST}`
if [ ! "${LIB_LIST}_" = "._" ]; then
	MULTILIB="Y"
	SUPPORT_TARGET=`find ${TARGET_DIR}/bin -name "*-gcc${EXE}" | sed 's/.*\/bin\///g' | sed 's/-gcc.*//g'`
	SUPPORT_TARGET=`echo ${SUPPORT_TARGET}`
	if [ "${SUB_TARGET}_" = "_" ]; then
			echo "SUB_TARGET is err,please check it"
			ERR_INFO	
	fi	
	CC=`find ${TARGET_DIR}/bin -name "${SUB_TARGET}-gcc${EXE}"`
	if [ "${CC}_" = "_" ]; then		
		CC=`find ${TARGET_DIR}/bin -name "${SUB_TARGET}-gcc"`
		if [ "${CC}_" = "_" ]; then		
			echo "fail to find toolchain,please check the value TARGET"
			ERR_INFO
		fi
	fi	
fi	

##############################################################
##### ��ȡ��Ŀ¼����
##############################################################
LIB_NAME=`${CC} -print-multi-directory`

##############################################################
##### ��ȡ������·��
##############################################################
GCC_LIB_PATH=`${CC} -print-file-name=.`
GCC_LIB_PATH=`cd ${GCC_LIB_PATH}; pwd`
C_LIB_PATH=${TARGET_DIR}/${TARGET}/lib/${LIB_NAME}
C_LIB_PATH=`cd ${C_LIB_PATH}; pwd`

##############################################################
##### ��ȡ��̬��
##############################################################
#rm ${PREFIX_DIR}/* -rf

install -dv ${PREFIX_DIR}

set +e
if [ "${MULTILIB}_" = "Y_" ]; then
	if [ "${LIB_NAME}_" = "._" ]; then
		LIB_LIST=`echo ${LIB_LIST} | sed 's/\.//g'`
		for path in ${LIB_LIST}; 
		do
			EX_GCC_LIB_PATH+="-path ${GCC_LIB_PATH}/${path} -prune -o "
			EX_C_LIB_PATH+="-path ${C_LIB_PATH}/${path} -prune -o "
		done
	
		find ${GCC_LIB_PATH} ${EX_GCC_LIB_PATH} -name "*.so*" -exec cp {} -d ${PREFIX_DIR} \;
		find ${C_LIB_PATH} ${EX_C_LIB_PATH} -name "*.so*" -exec cp {} -d ${PREFIX_DIR} \;
	else
		find ${GCC_LIB_PATH} -name "*.so*" -exec cp {} -d ${PREFIX_DIR} \;
		find ${C_LIB_PATH} -name "*.so*" -exec cp {} -d ${PREFIX_DIR} \;
	fi
else
	#find ${TARGET_DIR} -name "*.so*" -exec cp {} -d ${PREFIX_DIR} \;
	find ${TARGET_DIR} -name "*.so*" -a ! -name "libz.so" -exec cp {} -d ${PREFIX_DIR} \;	
	
fi
set -e

cd ${PREFIX_DIR}
for file in *; do
	if [ -L ${file} ] ; then
		file1="`ls -l ${file}`"
		file2=${file1##*../}
		if [ "${file2}_" != "${file1}_" ] ; then
			file2=${file2##*/}
			rm -f ${file}
			ln -s ${file2} ${file}
		fi
	fi
done

#�����ӵ�����

for file in * ; do
	if [ -L ${file} ] ; then
		[ -e ${file} ] || rm -f ${file}
	fi
done
cd ${CURPWD}
cp ${TARGET_DIR}/version ${PREFIX_DIR}/
echo "=========================================================================="
cd ${CURPWD}
ls -lt ${PREFIX_DIR}
echo "GetSharelib is over!!!!!!!!!!!!!!!!"
