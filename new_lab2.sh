#!/bin/bash

#echo "Написать программу, объединяющую два введенных по запросу файла в один. Переслать содержимое полученного файла через межпроцессный канал в параллельный процесс, где получить информацию о количестве строк, слов и символов этого файла. Предусмотреть возможность прерывания от клавиатуры. При поступлении первого прерывания вывести информацию о количестве строк, слов и символов первого файла, при поступлении второго прерывания вывести те же самые атрибуты второго файла"

counter=0
flg1=0
flg1_s=0
flg1_e=0
flg2=0
flg2_e=0

trap myHandler 2
myHandler() 
{
counter=`expr $counter + 1`
if [ $flg1 -eq 1 -a $flg1_e -eq 0 ]; then
	flg1_e=1
	echo "File1:"
	cat $fileName1 | wc
elif [ $flg1 -eq 0 ]; then
	flg1_s=1
fi

if [ $flg2 -eq 1 -a $flg1 -eq 1 -a $flg1_e -eq 0 ]; then
	flg1_e=1
	echo "File1:"
	cat $fileName1 | wc
fi
	
if [ $counter -ge 2 -a $flg2 -eq 1 -a $flg2_e -eq 0 ]; then	
	flg2_e=1
	echo "File2: "
	cat $fileName2 | wc
fi
}


while [ 0 ]
do
echo "Enter first file:"
read fileName1
echo "TEST: $fileName1 end"
if test -r $fileName1 #-a -f $fileName1 
then
	flg1=1
	break
else
	continue
fi
done


echo "Enter second file:"
while [ 0 ]
do		
read fileName2
if [ -f $fileName2 -a -w $fileName2 ]
then
	flg2=0
	break
else
	echo "Enter second file:"
	continue
fi
done

if [ $flg1_s -eq 1 -a $flg1_e -eq 0 ]; then
	flg1_e=1
	echo "File1:"
	cat $fileName1 | wc
fi
if [ $counter -ge 2 -a $flg1_e -eq 0 -a $flg2_e -eq 0 ] 
then
	$flg1_e=1
	$flg2_e=1
	echo "File1:"
	cat $fileName1 | wc
	echo "File2: "
	cat $fileName2 | wc
fi

#sleep 1
echo "File1 + File2: "
cat $fileName1 $fileName2 | wc
sleep 1
sleep 1
sleep 1	
