#!/bin/bash

#Jmeno studenta: Anastasiia Stoika
#Login: xstoik00

LC_ALL=POSIX
export LC_ALL

function help_f ()
{
	echo Jmeno: dirstat
	echo "Pouziti: dirstat [-i FILE_ERE] [DIR]"
	echo Popis: 
	echo "1.Pokud byl skriptu zadán adresář (DIR), bude předmětem zkoumání. Bez zadaného adresáře se prozkoumává aktuální adresář."
	echo "2.Přepínač -i způsobuje ignorování souborů a adresářů, jejichž název odpovídá rozšířenému regulárnímu výrazu FILE_ERE.  "
	echo "	FILE_ERE nesmí pokrývat název kořenového adresáře."
}	


#pocitani zadanych parametru z prikazove radky
counter=0 
for i in "$@"; do
	counter=`expr $counter + 1`
done



if [ "$counter" -eq 2 -o "$counter" -eq 3 ];then
	if [ "$counter" -eq 2 ];then
		if [ "$1" == "-i" -a -d $2 ];then
			echo Error. Wrong kombination of arguments.
			help_f
			exit 1
		else 
			reg_vyraz=$2
			dir=`pwd`
		fi
	elif [ "$counter" -eq 3 ];then
		if [ "$1" == "-i" -a -d $3 ];then
			reg_vyraz=$2
			dir=$3
		else
			echo Error. Wrong kombination of arguments.
			help_f
			exit 1
		fi
	fi
	#ROOT DIRECTORY -i ##################################################################################################
		echo "Root directory: $dir" 
	#DIRECTORIES -i ######################################################################################################
		pocet_dir=`find $dir -type d | grep -v $reg_vyraz | wc -l`
		if [ "$pocet_dir" -eq 0 ]; then
			pocet_dir=`expr $pocet_dir + 1`
			echo Directories: $pocet_dir
		else 
			echo "Directories: $pocet_dir" 
		fi
	#DEPTH	-i ###########################################################################################################
		max_depth=1		#pocatecni hodnota
		cd $dir			#prepneme se do zadaneho adresare, aby stal aktualnim


		for dname in *; do	#a abyjsme mohli pak nerekurzivne projit cez vsechy podadresare bez nasiho zadaneho
			
			depth=`find $dname -type d | grep -v $reg_vyraz | wc -l`
#			echo $depth $dname
	
			if [ "$depth" -gt "$max_depth" ]; then
				max_depth=$depth
			fi		 
		done
		max_depth=`expr $max_depth + 1`
		echo  "Max depth: $max_depth"
	#AVERAGE no. OF FILES -i ################################################################################################
		pocet_files=`find $dir -type f | grep -v $reg_vyraz | wc -l`
		#INT
		average_no_files=`expr $pocet_files / $pocet_dir` # I kdyz vysledek float -> int
		echo Average no. of files: $average_no_files
	
		echo All files: $pocet_files
	
	#FILES -i  ###############################################################################################################
	#Largest file -i  #########################################################################
		pomocna=`find $dir -type f | grep -v $reg_vyraz`
		sizes_of_files=`find $pomocna -type f -printf '%s\n' | sort -n`
		
		if [ "$pocet_files" -eq 0 ]; then
			echo "  Largest file: N/A"
		else
			largest_file=`find $pomocna -type f -printf '%s\n' | sort -n | tail -n 1` 
			echo "  Largest file: $largest_file"
		fi
	#Average file -i #########################################################################	
	#------------> = sum of all sizes / pocet of files
if [ "$pocet_files" -eq 0 ]; then
	echo "  Average file size: N/A"
else
	sum=0	
	for size in $sizes_of_files; do
		sum=`expr $sum + $size`
	done
	#INT
	average_file_size=`expr $sum / $pocet_files`  #bylo / $pocet_sizes
	echo "  Average file size: $average_file_size"
fi
	#FILE SIZE MEDIAN -i #####################################################################
	#1 - Overeni jestli je pocet_sizes sude cislo nebo liche
	#potrebuje 2 argumenty - pocet files a sizes of files
if [ "$pocet_files" -eq 0 ]; then
	echo "  File size median: N/A"
else
	sude_liche=`expr $pocet_files % 2` # +
	counter=0
	flag=0
	if [ "$sude_liche" -eq 0 ]; then		#je-li vysl. deleni dvema = 0 -> sude cislo
		for cislo in $sizes_of_files; do
			if [ "$flag" -eq 1 ]; then
				median2=$cislo
				break
			else
				counter=`expr $counter + 1`
				amount_s=`expr $pocet_files - $counter`
				if [ "$counter" -eq "$amount_s" ]; then
					median1=$cislo
					flag=1
				else continue
				fi
			fi
		done
		sum_med=`expr $median1 + $median2`
		median=`expr $sum_med / 2`			#median je sum/2 dvou cisel uprosted
		echo "  File size median: $median"
	else						# +  je-li vysl. deleni dvema cokoliv krome 0 -> liche cislo
		amount_s=`expr $pocet_files + 1`	# +	#pocet rozmeru na jedno vice, nez skutecne je
#overeni	echo Amount_s zvetsene o 1: $amount_s
		for cislo in $sizes_of_files; do
			counter=`expr $counter + 1`
			amount_s1=`expr $amount_s - $counter`
#overeni		echo Counter: $counter, amounts: $amount_s1 
			if [ "$counter" -eq "$amount_s1" ]; then	 #median je cislo uprostred
				median=$cislo
				break
			fi
		done
		echo "  File size median: $median"
	fi
fi
	#FILE EXTENSIONS -i  ###################################################################################################
	extensions=`find $dir -type f | grep -v $reg_vyraz | sed 's/^.*\.//g' | awk '!/\//' | sort | uniq ` #uniq -c -> kolik kterych pripon
	pocet_ext=`find $dir -type f | grep -v $reg_vyraz | sed 's/^.*\.//g' | awk '!/\//' | sort | uniq | wc -l`
if [ "$pocet_ext" -ne 0 ]; then
	pripony_vypis=`echo $extensions | sed 's/ /,/g'`
	echo File extensions: $pripony_vypis

	#ANALYSIS OF FILES WITH EXTENSIONS -i ##################################################################################
	for e in $extensions; do
		pocet_files_ext=`find $dir -name "*.$e" | wc -l`
		echo Files .$e: $pocet_files_ext
	# LARGEST FILE WITH EXT -i ##########################################################	
		largest_file=`find $dir -name "*.$e" -printf '%s\n' | sort -n | tail -n 1`
		echo "  Largest file .$e: $largest_file"
	# AVERAGE FILE WITH EXT -i #########################################################
		sum=0
		sizes_ext=`find $dir -name "*.$e" -printf '%s\n' | sort -n`  #promenna s rozmery souboru dane pripony
		
        	for size in $sizes_ext; do
                	sum=`expr $sum + $size`
        	done
        	#INT
        	average_file_size=`expr $sum / $pocet_files_ext`
        	echo "  Average file size .$e: $average_file_size"
	# MEDIAN OF FILES WITH EXT #######################################################
        sude_liche=`expr $pocet_files_ext % 2` # +
        counter=0
	eflag=0
        if [ "$sude_liche" -eq 0 ]; then                #je-li vysl. deleni dvema = 0 -> sude cislo
                for cislo in $sizes_ext; do
                        if [ "$eflag" -eq 1 ]; then
                                median2=$cislo
                                break
                        else
                                counter=`expr $counter + 1`
                                amount_s=`expr $pocet_files_ext - $counter`
                                if [ "$counter" -eq "$amount_s" ]; then
                                        median1=$cislo
                                        eflag=1
                                else continue
                                fi
                        fi
                done
                sum_med=`expr $median1 + $median2`
                median=`expr $sum_med / 2`                      #median je sum/2 dvou cisel uprosted
                echo "  File size median: $median"
        else                                            # +  je-li vysl. deleni dvema cokoliv krome 0 -> liche cislo
                amount_s=`expr $pocet_files_ext + 1`        # +     #pocet rozmeru na jedno vice, nez skutecne je
#overeni        echo Amount_s zvetsene o 1: $amount_s
                for cislo in $sizes_ext; do
                        counter=`expr $counter + 1`
                        amount_s1=`expr $amount_s - $counter`
#overeni                echo Counter: $counter, amounts: $amount_s1
                        if [ "$counter" -eq "$amount_s1" ]; then         #median je cislo uprostred
                                median=$cislo
                                break
                        fi
                done
                echo "  File size median: $median"
        fi
	done
  exit 0
fi
###################################### SCRIPT JEN S CESTOU NEBO BEZ CESTY #########################################################################

#je-l	i byl zadan pouze jeden argument, overeni je-li to adresar
elif [ ""$counter" -eq 1" -o ""$counter" -eq 0" ]; then
	if [ "$counter" -eq 1 ]; then
		dir=$1
	else
		dir=`pwd`
	fi
#	echo $dir_arg  ---> overeni, jestli se arg spravne zapsal
	
	#je-li to adresar, bude se prozkoumat
	if [ -d $dir ]; then 
	
	#ROOT DIRECTORY ###################################################################################################
		echo "Root directory: $dir" 
	#DIRECTORIES ######################################################################################################
#	cd $dir_arg
		pocet_dir=`find $dir -type d | wc -l`
		if [ "$pocet_dir" -eq 0 ]; then
			pocet_dir=`expr $pocet_dir + 1`
			echo Directories: $pocet_dir
		else
			echo "Directories: $pocet_dir" 
		fi
	#DEPTH	###########################################################################################################
		max_depth=1		#pocatecni hodnota
		cd $dir			#prepneme se do zadaneho adresare, aby stal aktualnim


		for dname in *; do	#a abyjsme mohli pak nerekurzivne projit cez vsechy podadresare bez nasiho zadaneho
			
			depth=`find $dname -type d | wc -l`
#			echo $depth $dname
	
			if [ "$depth" -gt "$max_depth" ]; then
				max_depth=$depth
			fi		 
		done
		max_depth=`expr $max_depth + 1`
		echo  "Max depth: $max_depth"

	#AVERAGE no. OF FILES ################################################################################################
		pocet_files=`find $dir -type f | wc -l`
		#INT
		average_no_files=`expr $pocet_files / $pocet_dir` # I kdyz vysledek float -> int
		echo Average no. of files: $average_no_files
		echo All files: $pocet_files

	#FILES ###############################################################################################################
	#Largest file #########################################################################

		sizes_of_files=`find $dir -type f -printf '%s\n' | sort -n`      #ubrala v sort -u
#     +		echo SIZES $sizes_of_files
#		pocet_sizes=`find $dir -type f -printf '%s\n' | sort -n | wc -l`  #pocet sizes == pocet_files

	#Ukaze posledni radek s nejvetsim rozmerem filu 
		largest_file=`find $dir -type f -printf '%s\n' | sort -n | tail -n 1` 
		if [ "$largest_file" -eq 0 ]; then
			echo "  Largest file: N/A"
		else
			echo "  Largest file: $largest_file"
		fi
	#Average file #########################################################################	
	#------------> = sum of all sizes / pocet of files
	if [ "$pocet_files" -eq 0 ]; then
		echo "  Average file size: N/A"
	else
		sum=0	
		for size in $sizes_of_files; do
			sum=`expr $sum + $size`
		done
		#INT
		average_file_size=`expr $sum / $pocet_files`  #bylo / $pocet_sizes
		echo "  Average file size: $average_file_size"
	fi
	#FILE SIZE MEDIAN #####################################################################
	#1 - Overeni jestli je pocet_sizes sude cislo nebo liche
	#potrebuje 2 argumenty - pocet files a sizes of files

if [ "$pocet_files" -eq 0 ]; then
	echo File size median: N/A
else
	sude_liche=`expr $pocet_files % 2` # +
	counter=0
	flag=0
	if [ "$sude_liche" -eq 0 ]; then		#je-li vysl. deleni dvema = 0 -> sude cislo
		for cislo in $sizes_of_files; do
			if [ "$flag" -eq 1 ]; then
				median2=$cislo
				break
			else
				counter=`expr $counter + 1`
				amount_s=`expr $pocet_files - $counter`
				if [ "$counter" -eq "$amount_s" ]; then
					median1=$cislo
					flag=1
				else continue
				fi
			fi
		done
		sum_med=`expr $median1 + $median2`
		median=`expr $sum_med / 2`			#median je sum/2 dvou cisel uprosted
		echo "  File size median: $median"
	else						# +  je-li vysl. deleni dvema cokoliv krome 0 -> liche cislo
		amount_s=`expr $pocet_files + 1`	# +	#pocet rozmeru na jedno vice, nez skutecne je
#overeni	echo Amount_s zvetsene o 1: $amount_s
		for cislo in $sizes_of_files; do
			counter=`expr $counter + 1`
			amount_s1=`expr $amount_s - $counter`
#overeni		echo Counter: $counter, amounts: $amount_s1 
			if [ "$counter" -eq "$amount_s1" ]; then	 #median je cislo uprostred
				median=$cislo
				break
			fi
		done
		echo "  File size median: $median"
	fi
fi	
	
	#FILE EXTENSIONS ###################################################################################################
	extensions=`find $dir -type f | sed 's/^.*\.//g' | awk '!/\//' | sort | uniq ` #uniq -c -> kolik kterych pripon
	pocet_ext=`find $dir -type f | sed 's/^.*\.//g' | awk '!/\//' | sort | uniq | wc -l`
if [ "$pocet_ext" -ne 0 ]; then
	pripony_vypis=`echo $extensions | sed 's/ /,/g'`
	echo File extensions: $pripony_vypis
	

#	first_table=`find $dir -type f | sed 's/^.*\.//g' | awk '!/\//' | sort | uniq -c | awk '{printf("Files .%s: %d\n", $2, $1);}'`	
	
	#ANALYSIS OF FILES WITH EXTENSIONS##################################################################################
	for e in $extensions; do
		pocet_files_ext=`find $dir -name "*.$e" | wc -l`
		echo Files .$e: $pocet_files_ext
	# LARGEST FILE WITH EXT ##########################################################	
		largest_file=`find $dir -name "*.$e" -printf '%s\n' | sort -n | tail -n 1`
		echo "  Largest file .$e: $largest_file"
	# AVERAGE FILE WITH EXT  #########################################################
		sum=0
		sizes_ext=`find $dir -name "*.$e" -printf '%s\n' | sort -n`  #promenna s rozmery souboru dane pripony
		
        	for size in $sizes_ext; do
                	sum=`expr $sum + $size`
        	done
        	#INT
        	average_file_size=`expr $sum / $pocet_files_ext`
        	echo "  Average file size .$e: $average_file_size"
	# MEDIAN OF FILES WITH EXT #######################################################
        sude_liche=`expr $pocet_files_ext % 2` # +
        counter=0
	eflag=0
        if [ "$sude_liche" -eq 0 ]; then                #je-li vysl. deleni dvema = 0 -> sude cislo
                for cislo in $sizes_ext; do
                        if [ "$eflag" -eq 1 ]; then
                                median2=$cislo
                                break
                        else
                                counter=`expr $counter + 1`
                                amount_s=`expr $pocet_files_ext - $counter`
                                if [ "$counter" -eq "$amount_s" ]; then
                                        median1=$cislo
                                        eflag=1
                                else continue
                                fi
                        fi
                done
                sum_med=`expr $median1 + $median2`
                median=`expr $sum_med / 2`                      #median je sum/2 dvou cisel uprosted
                echo "  File size median: $median"
        else                                            # +  je-li vysl. deleni dvema cokoliv krome 0 -> liche cislo
                amount_s=`expr $pocet_files_ext + 1`        # +     #pocet rozmeru na jedno vice, nez skutecne je
#overeni        echo Amount_s zvetsene o 1: $amount_s
                for cislo in $sizes_ext; do
                        counter=`expr $counter + 1`
                        amount_s1=`expr $amount_s - $counter`
#overeni                echo Counter: $counter, amounts: $amount_s1
                        if [ "$counter" -eq "$amount_s1" ]; then         #median je cislo uprostred
                                median=$cislo
                                break
                        fi
                done
                echo "  File size median: $median"
        fi
	done
 exit 0
fi		

else
	echo Error. Wrong arguments or such directory does not exists. Try again.
	help_f
	exit 1
fi
fi

