#!/usr/bin/env php    
<?php

mb_internal_encoding("UTF-8");
/*
**	IPP projekt c.1 : XTD
**	Autor: Stoika Anastasiia
*/


//KEYS
$INPT = "input";			//pro porovnani s klicem pri prechodu vstupnich parametru s foreach
$OUTPT = "output";
$HELP = "help";
$HEADER = "header";
$ETC = "etc";
$A = "a";
$B = "b";
$G = "g";

/**
*	printErr 
*	- Funkce printErr vytiskne na chybovy vystup chybove hlaseni a ukonci skript s navratovym kodem
*
*	@in - $typeErr - chybove hlaseni
*		- $errCode - navratovy kod chyby
*/
function printErr($typeErr, $errCode){
	fprintf(STDERR, "ERROR: ".$typeErr." -> return(".$errCode.")\n");
	exit($errCode);
	//$stderr = fopen('php://stderr', 'w');
}

//--------------------------------------------------Zpracovani parametru-------------------------------------------------------
$shortopts  = "a";
$shortopts .= "b"; // Эти параметры не принимают никаких значений

$longopts  = array(
    "help",    
    "input::", 
    "output::",
    "etc::",
    "header::"
);
$options = getopt($shortopts, $longopts);
$count = count($options);

	foreach ($options as $key => $value) {
	//	print "$key => $value\n"; 
		if(strcmp($key, $HELP) == 0 && $count === 1){					//je-li pocet argumentu 1 a zadan parametr --help
			echo "-------------------------------HELP---------------------------\n";
			echo "Skript na základě XML souboru s daty vytvoří sadu SQL příkazů\n";
			echo "generujících příslušnou strukturu tabulek v SQL databázi, ve \n";
			echo "které by se data obsažená ve vstupním souboru mohla nacházet.\n";
			echo "Autor: Anastasiia Stoika, xstoik00\n";
			echo "Parametry skriptu:\n";
			echo "--help --- napoveda\n";
			echo "--input=filename --- zadaný vstupní soubor ve formátu XML\n";
			echo "--output=filename --- zadaný výstupní soubor ve formátu definovaném výše\n";
			echo "--header='hlavička' --- na začátek výstupního souboru se vloží zakomentovaná hlavička\n";
			echo "--etc=n pro n ≥ 0 --- určuje maximální počet sloupců vzniklých ze stejnojmenných podelementů\n";
			echo "-a --- nebudou se generovat sloupce z atributů ve vstupním XML souboru\n";
			echo "-b --- pokud bude element obsahovat více podelementů stejného názvu, bude se uvažovat,\n";
			echo "		 jako by zde byl pouze jediný takový\n";
			echo "		 (tento parametr nesmí být kombinován s parametrem --etc=n)\n";
			return 0;
		}	//--input=filename
		elseif(strcmp($key, $INPT) == 0){
			if($value == NULL){						//pokud --input byl zadan bez vstupniho souboru --> nacitani z stdin
//				echo "Vstupni soubor nezadan\n";	//debugovani
				$input = fopen("php://stdin", "r");
				if(($input = fopen($options['input'], "r")) === false){
					printErr("Chyba pri otevreni souboru", 2);
				}
			}
			else{
				$input = fopen($value, "r");
				if(($input = fopen($options['input'], "r")) === false){
					printErr("Chyba pri otevreni souboru", 2);
				}				
			}
		}	//--output=filename
		elseif(strcmp($key, $OUTPT) == 0){
			if($value == NULL){						//pokud --output byl zadan bez vystupniho souboru --> vystup do stdout
				echo "Vystupni soubor nezadan\n";	//debugovani
				$output = fopen("php://stdout", "w");
				if(($output = fopen($options['output'], "w")) === false){
					printErr("Chyba při pokusu o otevření zadaného výstupního souboru pro zápis", 3);
				}
			}
			else{
				$output = fopen($value, "w");					//v promenne output bude ukazan vystupni soubor
				if(($output = fopen($options['output'], "w")) === false){
					printErr("Chyba při pokusu o otevření zadaného výstupního souboru pro zápis", 3);
				}
			}
		}	//--header='hlavicka'
		elseif(strcmp($key, $HEADER) == 0){
			if($value == NULL){
				printErr("Spatny format parametru --header='hlavicka'", 1);
			}
			else{
				$header = $value;						//do promenne $header bez uvozovek se zapise zadana hlavicka
			}
		}	//--etc=n, n>=0
		elseif(strcmp($key, $ETC) == 0){
			if(($value == NULL) || ($value < 0)){
				printErr("Spatny format parametru --etc=n, n>=0", 1);
			}
			else if(isset($options['b'])){
				printErr("Tento parametr nesmí být kombinován s parametrem -b", 1);
			}
			else{
				$etc = $value;								//do prom. $value bude zadan max. pocet sloupcu ze stejnojmennych atributu
			}
		}	//-a
		elseif(strcmp($key, $A) == 0){
			$a = true;
		}	//-b
		elseif(strcmp($key, $B) == 0){
			if(isset($options['etc'])){
				printErr("Tento parametr nesmí být kombinován s parametrem --etc=n", 1);
			}
			else{
				$b = true;
			}
		}	//-g
		elseif(strcmp($key, $G)){
			$g = true;
		}
		else{
			printErr("Chyba pri zadani parametru skriptu", 1);
		}
	}
//---------------------------------------------------------------------------------------------------------------------

if(!isset($options['input']))
	$input = fopen("php://stdin", "r");
elseif(($input = fopen($options['input'], "r")) === false)
    printErr("Nejde otevrit vstupni soubor", 2);

if(!isset($options['output'])){
	$output = fopen("php://stdout", "w");
}
elseif(($output = fopen($options['output'], "w")) === false)
    printErr("Nejde otevrit vystupni soubor", 3);

//Nacitani obsahu vstupniho souboru
$input_text = '';
do{
	$obsah = fgetc($input);
	$input_text .= $obsah;
}
while($obsah !== false);

//Vytvoreni objektu
$xml_file = simplexml_load_string($input_text);
//print_r($xml_file);

//Zavolani funkce pro zpracovani vstupniho souboru a rozparsovani xml objektu
$arr = array();
$tables = xml_parsing($xml_file, $arr, $options);
//print_r($tables);


if(!isset($options['g'])){
	create_output($tables, $options, $output);
}

//-----------------------------------------------------
$value_attr = "";
$arr_el = array();  		


/**
*	xml_parsing 
*	- Funkce xmk_parsing rozparsuje vytvoreny xml objekt a ulozi do pole data, ze kterych pak bude vytvoreny tabulky
*
*	@in - $xml_object - vytvoreny xml object
*		- $array_elements - vysledna datova struktura
*		- $options - vstupni parametry
*/
function xml_parsing($xml_object, $array_elements, $options){

	foreach($xml_object as $element=>$value) {
		$element = mb_strtolower($element);
		$max_sub = array();

    	if(!array_key_exists($element, $array_elements)) {										//default type for PK - INT
    		$array_elements[$element]['prk_'.$element.'_id']["data_type"] = 'INT';		//prvni sloupec v tabulke $key je PRK_nazevtab_ID
			$array_elements[$element]['prk_'.$element.'_id']["count"] = 1;
    	}

    	//---POKUD NASTAVEN PREPINAC -a --> NEGENEROVAT SLOUPCE Z ATRIBUTU
    	//---***treba pridat treti vstupni parametr do funkce - pole $options, aby fungovalo
    	if(!isset($options['a'])){
    		foreach ($value->attributes() as $attribute => $attr_value){
				$attribute = mb_strtolower($attribute);

    			//POKIAL UZ SLOUPEC S TAKOVYM ATRIBUTEM EXISTUJE, -> VYRESIT JAKY MA BYT VYSLEDNY DAT.TYP
    			if(isset($array_elements[$element][$attribute]["data_type"])){
    				$type1 = $array_elements[$element][$attribute]["data_type"];
    				$type2 = set_data_type($attr_value, true);
    				$array_elements[$element][$attribute]["data_type"] = conflict_dtype($type1, $type2);
    				$array_elements[$element][$attribute]["count"] = 1;
    			}
    			else{
    				//----POKIAL NEEXISTUJE --> ZAPIS DO ELEMENTU SLOUPCE S NAZVEM ATRIBUTU A STANOVENI TYPU
    				$array_elements[$element][$attribute]["data_type"] = set_data_type($attr_value, true);
    				$array_elements[$element][$attribute]["count"] = 1;
    			}	
    		}
    	}
	//----GENERUJE SLOUPEC 'VALUE' FROM ---> <element>value</element>
    	if(trim($value) != ""){
    		if(isset($array_elements[$element]["value"])){
    			$type1 = $array_elements[$element]["value"]["data_type"];
    			$type2 = set_data_type($value, false);
    			$array_elements[$element]["value"]["data_type"] = conflict_dtype($type1, $type2);
    			$array_elements[$element]["value"]["count"] = 1;
    		}
    		else{
	    		$array_elements[$element]["value"]["data_type"] = set_data_type($value, false);
	    		$array_elements[$element]["value"]["count"] = 1;
	    	}
    	}

	foreach ($value as $key2 => $value2){
		$key2 = mb_strtolower($key2);

//----NASTAVOVANI FOREIGN KEYS
			if(!isset($array_elements[$element][$key2.'_id'])){
				$array_elements[$element][$key2.'_id'] = array();
	    		$array_elements[$element][$key2.'_id']["data_type"] = 'INT';  //----> TYPE of FK will be always INT !!
	   			$array_elements[$element][$key2.'_id']["count"] = 0;
	   		}
	 
    	//Taky naplneni pole $max_sub cizimi klici a prirazeni pocatecni hodnoty - pocet podelementu
    	if(!isset($max_sub[$key2.'_id'])){
    		if(isset($options['b']))
				$max_sub[$key2.'_id'] = 1;
			else
				$max_sub[$key2.'_id'] = 0;
    	}

    	if(!isset($options['b']))
			$max_sub[$key2.'_id']++;

		if(isset($options['etc'])){
			if($max_sub[$key2.'_id'] > $options['etc']){
//				unset($array_elements[$element][$key2.'_id']); //vymazat FK v tabulke $element
//				unset($max_sub[$key2.'_id']);
				//prehodit polozky - do tabulky $key2 pridat cizi klic $element_id
					$array_elements[$key2][$element.'_id']["count"] = 1;
					$array_elements[$key2][$element.'_id']["data_type"] = 'INT';
			}
		}
		if(!isset($array_elements[$key2]['prk_'.$key2.'_id'])){
				$array_elements[$key2]['prk_'.$key2.'_id']["count"] = 1;
				$array_elements[$key2]['prk_'.$key2.'_id']["data_type"] = 'INT';
		}
	}//konec 3 cyklu foreach

		//$max_key == $key2
		foreach ($max_sub as $max_key => $max_val){

			if(isset($array_elements[$element][$max_key])){
				if($max_val > $array_elements[$element][$max_key]['count']){
					$array_elements[$element][$max_key]['count'] = $max_val; //echo "--".$array_elements[$element][$max_key]["count"];
				}
				if(isset($options['etc']) && ($max_val > $options['etc'])){
					unset($array_elements[$element][$max_key]);
					$max_val = 1;
				}

				if($max_val > 1){
					unset($array_elements[$element][$max_key]);	//ubirajem etot podelement i dobavlyaem jeho pronumerovannogo
					$cut_key = substr($max_key, 0, -3);
					for($i = 1; $i <= $max_val; $i++){
						$array_elements[$element][$cut_key.$i.'_id']["count"] = 1;
						$array_elements[$element][$cut_key.$i.'_id']["data_type"] = 'INT';
					}
				}

			}
			else{
				$array_elements[$element][$max_key]["count"] = $max_val;

				if(isset($options['etc']) && ($max_val > $options['etc'])){
					unset($array_elements[$element][$max_key]);
					$max_val = 1;
				}

				if($max_val > 1){
					unset($array_elements[$element][$max_key]);
					$cut_key = substr($max_key, 0, -3);
					for($i = 1; $i <= $max_val; $i++){
						$array_elements[$element][$cut_key.$i.'_ID']["count"] = 1;
						$array_elements[$element][$cut_key.$i.'_ID']["data_type"] = 'INT';
					}
				}
			}
		}
		$array_elements = xml_parsing($value, $array_elements, $options);	
	}
	return $array_elements;
}

/**
*	create_output 
*	- Funkce create_output vytiskne na chybovy vystup chybove hlaseni a ukonci skript s navratovym kodem
*
*	@in - $tables - vytvorena datova struktura z funkce xml_parsing
*		- $options - vstupni parametry
*		- $output - vystup
*/
function create_output($tables, $options, $output){

	if(isset($options['header']) && $options['header'] != "")
		fprintf($output, "--%s\n\n", $options['header']);

	foreach ($tables as $table_name => $value) {
		fprintf($output, "CREATE TABLE %s(\n", $table_name);


			foreach ($value as $column_name => $value2) {		//$value2 - "count" a "data_type", potrebujeme jenom data_type
				
				if(substr_compare($column_name, "prk_", 0, 4) == 0)
					fprintf($output, "\t%s %s PRIMARY KEY", $column_name, $tables[$table_name][$column_name]["data_type"]);
				else
					fprintf($output, ",\n\t%s %s", $column_name, $tables[$table_name][$column_name]["data_type"]);
			}
			fprintf($output, "\n);\n\n");
	}
}

/**
*	set_data_type
*	- Funkce set_data_type urci datovy typ
*
*	@in - $value - value kteremu treba urcit datovy typ
*		- $attr_flag - je-li to atribut jiny typ nez bit, int a float --> nvarchar
*/
function set_data_type($value, $attr_flag){
	//!!!!!!!!! $value is case insensitive! --> strtolower
	if($value == '0' || $value == '1' || $value == 'true' || $value == 'false' || empty($value))
		return "BIT";
	elseif(is_int(filter_var($value, FILTER_VALIDATE_INT)))
		return "INT";
	elseif(is_float(filter_var($value, FILTER_VALIDATE_FLOAT)))
		return "FLOAT";
	elseif($attr_flag)						//je-li attribut jiny typ nez def. vyse, => jeho typ je NVARCHAR
		return "NVARCHAR";
	else
		return "NTEXT";						//je-li typ Elementu, ne atributu, jiny nez definovane vyse => jeho typ NTEXT

}

/**
*	conflict_dtype 
*	- Funkce conflict_dtype vyresi konfliky datovych typu
*
*	@in - $type1 - uz nastaveny datovy typ
*		- $type2 - novy urceny typ
*/
function conflict_dtype($type1, $type2){
	$types = array('BIT' => 1, 'INT' => 2, 'FLOAT' => 3, 'NVARCHAR' => 4, 'NTEXT' => 5);
	return (($types[$type1] > $types[$type2]) ? $type1 : $type2);
}




fclose($input);

?>
