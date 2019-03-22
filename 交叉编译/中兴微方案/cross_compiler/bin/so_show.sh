#!/bin/sh

#######################################################################
# �ű�˵��
# *********************************************************************
### 1. �ű����ܣ���host������ȡĿ�����̬�����̬�������������
### 2. �ű����÷�ʽ
###    ./so_show.sh TARGET=/opt/zte/x86_gcc412_glibc250/  usrapp1 usrapp2 usr-dir1 usr-dir2"
###    ���У�TARGETΪ��������Ŀ¼
###          usrapp*Ϊ��̬�����.so(usrapp��������)
###          usr-dir*Ϊ��̬�����.so����Ŀ¼��usr-dir��������,ֻ����usr-dir*�µ�usrapp*�����ݹ鴦������Ŀ¼��
###          Ϊ�����Ч�ʣ�ͬһ��̬�����.so��Ҫ����usrapp*�У�����usr-dir*��ͬһ��̬�����.so���ڶ��ʵ�����ᱻ���ִ���������飬Ӱ��Ч�ʣ�
#######################################################################

##############################################################
##### ����������Ϣ��ʾ
##############################################################
ERR_INFO()
{

	echo ""
	echo "############################################################################################"
	echo "so_show���ڻ�ȡĿ�����̬�����̬�������������"
	echo "ʹ�÷���:"
	echo "./so_show.sh TARGET=/opt/zte/x86_gcc412_glibc250/  usrapp1 usrapp2 usr-dir1 usr-dir2"
	echo "/opt/zte/x86_gcc412_glibc250/bin/so_show.sh usrapp1 usrapp2 usr-dir1 usr-dir2"
	echo "       " 
	echo "TARGET             Ϊ��������Ŀ¼���ڹ�������binĿ¼��ִ��so_showʱ����ʡ�Ըò���"
	echo "usrapp*            Ϊ��̬�����.so"
	echo "usr-dir*           Ϊ��̬�����.so����Ŀ¼��ֻ����usr-dir*�µ�usrapp*�����ݹ鴦������Ŀ¼��"
	echo ""
	echo "Ϊ�����Ч�ʣ�ͬһ��̬�����.so��Ҫ����usrapp*�У�����usr-dir*��"
	echo "############################################################################################"
	echo ""
	if [ $ERR -eq 0 ]; then 
		exit 0
	else
		exit 1
	fi
}


#######################################################################
# ��ȡһ��libname
#######################################################################
get_libname()
{
  start_token=$@	
	while [ "${start_token}" != "" ]
	do
		start_token=${start_token#*[}                     
		token=${start_token%%]*}     
		start_token=${start_token#*]}	
		
		((index=0))
		lib_name=${libs_name[${index}]}
		while [ "$index" -lt "$lib_count" -a "${token}" != "$lib_name" ]
		do		
			lib_name=${libs_name[${index}]}
			((index++))
		done
		if [ "$index" == "$lib_count" -a "${token}" != "$lib_name" ];then
			libs_name[${lib_count}]=${token}
			((lib_count++))
		fi
	done
}

#######################################################################
# �ݹ���ã���ȡ���������Ŀ���
#######################################################################
get_need()
{	
#	lib_name=`find $TARGET -name $1`
	index=0
	while [ "$index" -lt "$so_num" ] 
	do
		if [ "${all_so_name[${index}]}" = "$1" ]; then
			lib_name=${all_so[${index}]}
			index=so_num
		fi
		((index++))
	done
	if [ "$lib_name" = "" ]; then
		echo "WALLING: ��$1δ�ҵ�����ȷ�����Ƿ��û�˽�п�"
	fi
	
	lib_name=`$READELF -d ${lib_name} | grep NEEDED`
	get_libname ${lib_name}
	
	((finish_index=finish_index+1))
	if [ "$finish_index" -lt "$lib_count" ]; then		
		get_need ${libs_name[${finish_index}]}
	fi		
}

parse_deps()
{
	deps_index=0
	while [ $deps_index -lt $input_num ] 
	do		
		file=${input_file[$deps_index]}
		string="`$READELF -d ${file} | grep NEEDED`"
		if [ "${string}" != "" ]; then	
		  exe=`file ${file} | grep executable`
			target_lib_type=`$READELF -s ${file} | grep __uClibc_main`
			if [ "${target_lib_type}" = "" ]; then
				target_lib_type=glibc
			else
				target_lib_type=uClibc
			fi
			if [ "${target_lib_type}" != "${lib_type}" -a "$exe" != "" ]; then
				echo "WALLING: c�����Ͳ�ƥ��: ${file}=${target_lib_type}"		
				echo "                        ��������=${lib_type}"	
			else					
				dlopen=`$READELF -s ${file} | grep dlopen`
				if [ "$dlopen" != "" ]; then
					echo "WALLING: ${file}��ʹ����dlopen��so��"	
					echo "         ����Դ�ļ���ȷ�ϴ򿪵�ʲô�⣬����ʹ�ýű���һ�����dlopen�򿪿��������"	
				fi
				
				target_arch=`$READELF -h ${file} | grep Machine`	
				case $target_arch in 
					*ARM*)
						target_arch=ARM
						target_data=`$READELF -h ${file} | grep "big endian"` 
						if [ "$target_data" != "" ]; then
						target_arch+=EB		
						fi
						;;
					*MIPS*)
						target_arch=MIPS
#						`$READELF -h ${file} | grep ELF64` && target_arch+=64
						target_class=`$READELF -h ${file} | grep ELF64` 
						if [ "$target_class" != "" ]; then
							target_arch+=64
						fi
						target_data=`$READELF -h ${file} | grep "little endian"` 
						if [ "$target_data" != "" ]; then
						target_arch+=EL		
						fi
						;;
					*PowerPC*)
						target_arch=Powerpc;;
					*386*)
						target_arch=X86;;
					*X86-64*) 
						target_arch=X86-64;;
					*)
						target_arch=unkown;;
				esac			
				if [ "${target_arch}" != "${lib_arch}" ]; then
					echo "WALLING: arch��ƥ��: ${file}=${target_arch}"		
					echo "                     ��������=${lib_arch}"				
				else
					target_obj+=" ${file##*/}"
					target_names[${target_num}]=${file}
					get_libname $string		
					((target_num++))
				fi
			fi			
		fi		
		((deps_index++))			
	done
}

#######################################################################
# ��ӡ������������
#######################################################################
print_libname()
{
	print_index=0
	if [ $lib_count -gt "0" ]; then
		echo 
		echo "===$target_obj ����so ==="
		while [ "$print_index" -le "$lib_count" ]
		do
			echo ${libs_name[${print_index}]}
			((print_index++))
		done	
	else
		echo 
		echo "=== ���������û����Ч��̬�����̬�� ==="
		echo
	fi
}

check_arch()
{
	index=0
	
	while [ "$index" -lt "$so_num" ] 
	do
		lib_name=${all_so_name[${index}]}
		if [ "${lib_name:0:7}" = "libc.so" ]; then
			lib_name=${all_so[${index}]}
			index=so_num
		fi
		((index++))
	done
	
	if [ "$lib_name" != "" ]; then
		lib_arch=`$READELF -h ${lib_name} | grep Machine`
		case $lib_arch in 
			*ARM*)
				lib_arch=ARM
				lib_data=`$READELF -h ${lib_name} | grep "big endian"` 
				if [ "$lib_data" != "" ]; then
					lib_arch+=EB		
				fi
				;;
			*MIPS*)
				lib_arch=MIPS
#				`$READELF -h ${lib_name} | grep ELF64` && lib_arch+=64
				lib_class=`$READELF -h ${lib_name} | grep ELF64` 
				if [ "$lib_class" != "" ]; then
					lib_arch+=64
				fi
				lib_data=`$READELF -h ${lib_name} | grep "little endian"` 
				if [ "$lib_data" != "" ]; then
					lib_arch+=EL		
				fi
				;;		
			*PowerPC*)
				lib_arch=Powerpc;;
			*386*)
				lib_arch=X86;;
			*X86-64*) 
				lib_arch=X86-64;;
			*)
				lib_arch=unkown;;
		esac
		lib_type=`$READELF -s ${lib_name} | grep __uClibc_main`
		if [ "$lib_type" = "" ]; then
			lib_type=glibc
		else
			lib_type=uClibc
		fi
	fi	
}

get_all_so()
{
	echo "��������.so  ......"
	temp=`find ${TARGET} -name "*.so.*"`
	temp=`echo ${temp}`

	so_num=0
	all_so[${so_num}]=${temp%% *}
	so_name=${all_so[${so_num}]}
	all_so_name[${so_num}]=${so_name##*/}
	((so_num++))
	
	while [ "${temp}" != "${temp#* }" ]
	do
		temp=${temp#* }                     
		all_so[${so_num}]=${temp%% *}		
		so_name=${all_so[${so_num}]}
		all_so_name[${so_num}]=${so_name##*/}
		((so_num++))			
	done	
}

#######################################################################
# �����
#######################################################################

export LANG=zh_CN.GBK

finish_index=0
lib_count=0

target_obj=
target_num=0
path_num=0
input_num=0

#�����������ǰֻ��1��0��0����������1������󡣺��������֧��ʱ�����Խ�ERR��Ϊ�����
ERR=0

#û���κβ���ʱ��ֱ�����������Ϣ
if [ $# -eq 0 ];then
	ERR=0
	ERR_INFO
fi
while [ $# -gt "0" ] 
do
	param=$1
	if [ "TARGET=" = "${param:0:7}" ];then
		TARGET=${param#*TARGET=}
		if [ -d ${TARGET} ]; then
			TARGET=`cd ${TARGET}; pwd`
		else
			echo "target·��������ȷ�Ϲ�����·��"
			ERR=1
			ERR_INFO	
		fi
	else
		if [ -d ${param} ];then
			param=`cd ${param}; pwd`
			param=${param%/}
			for file in ${param}/*
			do
				if [ -f ${file} ]; then
  				ELF_FILE=`file -b ${file} | grep ELF`
  				if ! [ "_${ELF_FILE}" = "_" ]; then
    				input_file[${input_num}]=${file}
    				((input_num++))
  				fi
				fi
			done
		else
			if [ -f "${param}" ];then
				ELF_FILE=`file -b ${param} | grep ELF`
			else
				echo "��ʹ����ȷ��ʽ����so_show"
				ERR=1
				ERR_INFO
			fi
  		if ! [ "_${ELF_FILE}" = "_" ]; then
    		input_file[${input_num}]=${param}
    		((input_num++))		
  		fi		
		fi
	fi
	shift
done

if [ "${TARGET}" = "" ]; then	
	program_name=$0
	bin_dir=${program_name%/*}
	if [ "$bin_dir" = "$program_name" ]; then
		program_patch=`pwd`
		bin_dir=${program_patch##*/}
	else
		program_patch=${program_name%/*}
		program_patch=`cd $program_patch; pwd`
		bin_dir=${program_patch##*/}
	fi
	
	exist_readelf=`ls ${program_patch} | grep readelf`
	if [ "$bin_dir" = "bin" -a "$exist_readelf" != "" ]; then
		TARGET=`cd ${program_patch}/.. ; pwd`
	else
		echo "��ָ��TARGET������ȷ���Ƿ��ڹ�������binĿ¼��ִ��so_show.sh"
		ERR=1
		ERR_INFO
	fi
fi

READELF=`find ${TARGET} -name "*-readelf*"`
if [ $READELF = "" ]; then
	READELF=readelf
fi

get_all_so

check_arch

parse_deps

if [ "${libs_name[${finish_index}]}" != "" ]; then
	get_need ${libs_name[${finish_index}]}
fi	

print_libname
