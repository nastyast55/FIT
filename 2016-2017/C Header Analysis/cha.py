#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#Author: Anastasiia Stoika, xstoik00
#Project: C Header Analysis


import sys, argparse, os, re, ntpath
import xml.dom.minidom
#------------------------------HLEDANI HLAVICKOVYCH SOUBORU -- REKURZIVNE----------------------------------------------
#-----Pak volani pomoci: os.path.walk(os.getcwd(), det_header, None)
header_list = []
header_open = []
functions_list = []
dir_list = []
dir_vypis = ""
inline_flag = 0
whitespace_flag = 0
varargs = "false"
file_output = ""
space = ['\f', '\n', '\r', '\t', '\v', ' ']
OUT = []
pretty_xml = ""
no_dupl_flag = 0

help_flag = 0
els_param_flag = 0
inpt_opt = 0
outpt_opt = 0
pretty_opt = 0
no_inl_opt = 0
max_par_opt = 0
no_dupl_opt = 0
rm_w_opt = 0

keywords = ["auto", "char", "const", "double", "extern",
			"float", "inline", "int", "long", "register", "restrict", "short", "signed",
			"unsigned", "void", "volatile", "_Bool", "_Complex",
			"_Imaginary", "bool", "complex", "imaginary", "static", "*", "**"]

def det_header(arg, adresar, jmena):
	global header_list
	global dir_list
	global header_open

	for name in jmena:
		if re.search(r"\.h$",name):
			header_list.append(name)
			header_open.append(os.path.join(arg, name))

#----------------------------------------------------------------------------------------------------------------------
def filename_from_path(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)

#--------------------------------ZPRACOVANI PARAMETROV PRIKAZOVE RADKY - ARGPARSE ---> DOPRACOVAT!!!!------------------------
#duplicita a kombinovani parametru s --help
for a in range(len(sys.argv)):
	if a != 0:
			opt = sys.argv[a] 
			if opt == "--help":
				if els_param_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				help_flag = 1
			elif opt.find("--input", 0) != -1:
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if inpt_opt > 0:
					#print("inpt_opt_err: {}".format(inpt_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					inpt_opt += 1
					#print("inpt_opt: {}".format(inpt_opt))
			elif opt.find("--output", 0) != -1:
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if outpt_opt > 0:
					#print("outpt_opt_err: {}".format(outpt_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					outpt_opt += 1
					#print("outpt_opt: {}".format(outpt_opt))
			elif opt.find("--pretty-xml", 0) != -1:
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if pretty_opt > 0:
					#print("pretty_opt_err: {}".format(pretty_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					pretty_opt += 1
					#print("pretty_opt: {}".format(pretty_opt))
			elif opt.find("--max-par", 0) != -1:
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				
				if max_par_opt > 0:
					#print("maxpar_opt_err: {}".format(max_par_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					max_par_opt += 1
					#print("maxpar_opt: {}".format(max_par_opt))
			elif opt == "--no-inline":
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if no_inl_opt > 0:
					#print("no_inl_opt_err: {}".format(no_inl_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					no_inl_opt += 1
					#print("no_inl_opt: {}".format(no_inl_opt))
			elif opt == "--no-duplicates":
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if no_dupl_opt > 0:
					#print("no_dupl_opt_err: {}".format(no_dupl_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					no_dupl_opt += 1
					#print("no_dupl_opt: {}".format(no_dupl_opt))
			elif opt == "--remove-whitespace":
				els_param_flag = 1
				if help_flag:
					sys.stderr.write("ERROR: can't combine options with --help\n")
					sys.exit(2)
				if rm_w_opt > 0:
					#print("rm_w_opt_err: {}".format(rm_w_opt))
					sys.stderr.write("ERROR: duplicite option\n")
					sys.exit(2)
				else:
					rm_w_opt += 1
					#print("rm_w_opt: {}".format(rm_w_opt))
			else:
				sys.stderr.write("ERROR: Unknown options.\n")
				sys.exit(2)
#argparse
parser = argparse.ArgumentParser()
parser.add_argument("--no-duplicates", help="no duplicates", action="store_true")
parser.add_argument("--input", help="input file", nargs='?', default=sys.stdin)	
parser.add_argument("--output", help="output file", nargs='?', default=sys.stdout)
parser.add_argument("--pretty-xml", type=int, help="pretty-xml parametr", nargs='?',
		    const=4, dest="pretty") #pokud nebude zadane k, --> k = 4
parser.add_argument("--no-inline", help="no-inline parametr", action = "store_true")
parser.add_argument("--max-par", type=int, help="--max-par=n, n must be always defined", 
		    action="store", nargs=1)
parser.add_argument("--remove-whitespace", help="remove whitespace",
		    action="store_true", dest="rm_whitesp")


args = parser.parse_args()

if args.input:
	#nezadan-li parametr --input, prozkouma akt.adr. a jeho podadresare a najde soubory .h
	if args.input == sys.stdin:
		for root, dirs, files in os.walk(".",topdown=False):
			det_header(root, dirs, files)
		#os.walk(os.getcwd(), det_header, None)
		dir_vypis = "./"
	#pokud byla zadana cesta k adresari -> vsechny .h soubory v tomto adr a jeho podadr
	elif os.path.isdir(os.path.abspath(args.input)):
		cesta = args.input
		#os.walk(cesta, det_header, None)

		for root, dirs, files in os.walk(cesta,topdown=False):
			det_header(root, dirs, files)


		dir_list.append(args.input)
		if (args.input).endswith("/"):
			dir_vypis = args.input
		else:
			dir_vypis = args.input +"/"
	elif os.path.isfile(os.path.abspath(args.input)):
		filename = filename_from_path(args.input)
		header_list.append(filename)			#seznam s nazvy hlavickovych souboru
		header_open.append(args.input)			#seznam hlavickovych souboru pro otevreni
		dir_vypis = ""
	else:
		sys.stderr.write("ERROR: wrong argument for param --input\n")
		sys.exit(2)
if args.output:
	if args.output != sys.stdout:
		file_output = args.output
if args.pretty:
	for a in range(args.pretty):
		pretty_xml = pretty_xml + " "	
if args.no_inline:
	inline_flag = 1
if args.max_par:
	max_par = max(args.max_par)
	max_par = int(max_par)
if args.rm_whitesp:
	whitespace_flag = 1
if args.no_duplicates:
    no_dupl_flag = 1

#-----------------------------------------------------------------------------------------------------
#-----------------------------OVERENI NA SPRAVNOST NAJDENYCH HLAVICKOVYCH SOUBORU--------------------------
'''
print "Seznam hlavickovych souboru:"
print header_list
print "Seznam hlavickovych souboru s cesty pro otevreni:"
print header_open
print "Vysledna hodnota dir pro vypis:"
print dir_vypis
'''
#------------------------------------------- ZPRACOVANI DEKLARACE/DEFINICE FUNKCE -----------------------------------
def det_rett_and_name(string):
	state = "START"
	type_name = []
	type_name_dict = {}
	vysl = []
	str2 = ""
	star = ""
	name = ""
	ret_type = ""
	mezera = ""
	ret_type_flag = 0
	star_flag = 0
	#---------------Rozdeli nacteny retezec na slova a bile znaky a ulozi do listu-----
	for char in string:

		if char in space:
			#print "-----------"
			#print "char is whitespace"
			if str2 in keywords:
				if star != "":
					type_name.append(str2)
					type_name.append(star)
				else:
					type_name.append(str2)
			
				type_name.append(char)
			else:
				if star != "":
					type_name.append(star)
					type_name.append(str2)
				else:
					type_name.append(str2)

				type_name.append(char)
			#print("STR2-W: {}".format(str2))
			star = ""
			str2 = ""
			#print type_name
			#print "---------"
		elif char == "*":
			star = "*"
		else:
			str2 = str2 + char


	#pokud posledni symbol nebyl mezerou, ulozeni posledniho slova se provede zde
	if str2 != "":
		#print("STR2: {}".format(str2))
		type_name.append(str2)


	#print "TYPE NAME:"
	#print type_name
	#---------------------------------------------------------------------------------
	#-------Rozdeli navratovy typ od jmena funkce------
	for word in type_name:
		#if word != '' and (word not in space):
			#print("WORD: {}".format(word))
		
		if word in keywords:
			ret_type = ret_type + word
		
		#---Zapisovat whitespace nebo ne? --> whitespace_flag
		elif word in space:
			#print ("----------HERE")
			if whitespace_flag == 1:
				if ret_type[-1] not in space:
					ret_type = ret_type + " "
			else:
				#vsechny whitespaces maji byt prevedeny na mezery
				ret_type = ret_type + " "
		else:
			#name = word
			if word != '':
				#print("NAME: {}".format(word))
				name = word

	ret_type = ret_type.rstrip()									#vymazat mezeru na konci
	
	if whitespace_flag == 1:
		if ret_type.find('*') != -1 and ret_type[-2] == " ":
			ret_type = list(ret_type)
			del ret_type[-2]
			ret_type = ''.join(ret_type)

	type_name_dict['name'] = name
	type_name_dict['ret_type'] = ret_type
	
	#print("----- {}".format(type_name_dict))
	#print("---type_name_dict[NAME] = {}".format(type_name_dict['name']))
	#print("---type_name_dict[RET] = {}".format(type_name_dict['ret_type']))
	return type_name_dict		
#--------------------------------KONEC FUNKCE PRO ZPRACOVANI NAVRAT.TYPU A NAZVU FUNKCE-------------------

def detect_params(str_args):
	global varargs
	#print("VARARGS: {}".format(varargs))
	params = []
	str2 = ""
	star = ""
	star_flag = 0
	name = ""
	type_param = ""
	param_dict = []
	state = "START"
	comma_flag = 0
	type_p = []

	#print ("STR_args: {}".format(str_args))
	for char in str_args:
		
		if char in space:
			if str2 in keywords:
				if star != "":
					params.append(str2)
					params.append(star)
				else:
					params.append(str2)
			
				params.append(char)
			else:
				if star != "":
					params.append(star)
					params.append(str2)
				else:
					params.append(str2)

				if comma_flag == 1:
					params.append(",")
					comma_flag = 0
				params.append(char)
			star = ""
			str2 = ""
		elif char == "*":				# dvojity odkaz? (**)
			if star_flag == 1:
				star = star + "*"
				star_flag = 0
			else:
				star = "*"
				star_flag = 1
		elif char == ",":
			comma_flag = 1
		else:
			str2 = str2 + char

	#pokud posledni symbol nebyl mezerou ulozeni posledniho slova se provede zde
	if str2 != "":
		params.append(str2)

	#print("PARAMS STRUCTURE: {}".format(params))
	#----------------ZPRACOVANI PARAMETRU-----------------
	for word2 in params:
		if state == "START":
	
			#if word2 != '' and (word2 not in space):
				#print("PARAM_WORD: {}".format(word2))

			if word2 in keywords:				# plati i pro *, a **
				type_param = type_param + word2
				state = "START"

			#---Zapisovat whitespace nebo ne? --> whitespace_flag
			elif word2 in space:
				if whitespace_flag == 1:
					if type_param != "":
						if type_param[-1] not in space:
							type_param = type_param + " "
	
				else:
					#vsechny whitespaces maji byt prevedeny na mezery
					type_param = type_param + " "
		
				state = "START"
				
			#je-li detekovana comma, zapsat do listu detekovana jmeno a typ	
			elif word2 == ",":
				type_param = type_param.strip()
				if whitespace_flag == 1:
					' '.join(type_param.split())
			
				param_dict.append(type_param)
				name = ""
				type_param = ""
				state = "START"
				comma_flag = 1
			elif word2 == '...':
				#print("HERE WORD MUST BE '...' and it is: {}".format(word2))
				varargs = "true"
				state = "START"
			else:
				if word2.endswith("[]"):
					word2 = word2.strip("[]")
					state = "START"
					name = word2
				else:
					name = word2
					state = "START"
		elif state == "IGNORE":
			word2 = word2.strip('[]')
			state = "START"

	if type_param != "": #and name != "":
		type_param = type_param.strip()									#vymazat mezeru na konci

		if whitespace_flag == 1:
			if type_param.find('*') != -1 and type_param[-2] == " ":
				type_param = list(type_param)
				del type_param[-2]
				type_param = ''.join(type_param)
		
		param_dict.append(type_param)
	#print("VARARGS: {}".format(varargs))
	#print("------------------------------------------------------------------")
	return param_dict

#--------------------------------------------------------------------------------------------------------
#tyka se pouze: typedef type new_type;
def analyze_typedef(string):
	p_typedef = []

	p_typedef = string.split()
	if p_typedef[0] == "typedef":
		if p_typedef[1] in keywords:
			keywords.append(p_typedef[2])


#---------------------------------------------------------------------------------------------------------
#*********************************************************************************************************
#*                           HLAVNI BEH PROGRAMU ----> KONECNY AUTOMAT 								     *
#*********************************************************************************************************
#---------------------------------------------------------------------------------------------------------
for soubor in header_open:
	#-----------------------------
	#----Ziskani relativni adresy-
	file_vypis = os.path.relpath(soubor)
	#-----------------------------
	try:
		file_h = open(soubor, "r")
	except (OSError, IOError):
		sys.stderr.write("ERROR: Nepodarilo se otevrit soubor\n")
		sys.exit(2)
	
	state = "START"
	string = ""
	ret_type = ""
	name = ""
	str_args = ""
	func_flag = 0
	typedef_flag = 0
	new_type = ""
	func_dict = {}				#tu se zapisuji vysledky po volani funkce
	params_list = []
	flag_n = 0		
	flag_d = 0
	count = 0
	#------------------------------------FSM----------------------------------------
	for lino, line in enumerate(file_h, start=1):
		for c in line:
			if state == "START":
				if c == '/':
					state = "P_COMMENT"
				elif c in space:
					state = "START"
				elif c == "#":					#include, #define, #ifndef, ...
					state = "MRIZKA"
				elif c == "\"":
					state = "RETEZ_IGNORE"
				elif c == ";" or c == ")":
					state = "START"
				else:
					state = "P_FUNC"
					string = c
			elif state == "P_COMMENT":
				if c == '/':
					state = "STR_COMMENT"
				elif c == '*':					# --> /*
					state = "P_BLOCK_COMMENT"
			elif state == "STR_COMMENT":
				if c == '\n':
					state = "START"
			elif state == "P_BLOCK_COMMENT":	# --> /**
				if c == '*':
					state = "FINISH_BL_COM"
			elif state == "FINISH_BL_COM":
				if c == '/':					# --> /**/
					state = "START"
				elif c == '*':					# --> /***
					state = "FINISH_BL_COM"
				else:
					state = "P_BLOCK_COMMENT"
			elif state == "MRIZKA":
				if c == '\\':
					flag_n = 1
					state = "MRIZKA"
				elif c == '\n':
					if flag_n == 1:
						state = "MRIZKA"
						flag_n = 0
					else:
						state = "START"
				else:
					state = "MRIZKA"
#---------------------------------------------ANALYZA FUNKCE------------------------------------------------------
			elif state == "P_FUNC":
				if c == '(':
					#FUNKCE NA DETEKCI NAVRAT.TYPU a JMENA
					func_flag = 1
					func_dict = det_rett_and_name(string)
					#print("func_dict: {}".format(func_dict))
					
					if dir_vypis != "":
						func_dict["file"] = file_vypis
					else:
						func_dict["file"] = args.input
					state = "LIST_ARG"

				elif c == '{':
					if func_flag == 1:
						state = "IGNORE"
					else:
						typedef_flag = 1;
						state = "IGNORE"  #--->potential typedef
				elif c == ";":		
					if typedef_flag == 1:
						keywords.append(new_type)
						typedef_flag = 0
						new_type = ""
					else:
						analyze_typedef(string)
					
					func_flag = 0
					str_args = ""
					state = "START"
				elif c == "\"":
					state = "RETEZ_IGNORE"
				else:
					if typedef_flag != 1:
						string = string + c
						state = "P_FUNC"
					else:
						new_type = new_type + c
			
			elif state == "LIST_ARG":
				if c == ')':
					if str_args == "":
						params_list = []
						func_dict["varargs"] = "no"

						OUT.append(func_dict)
						OUT.append(params_list)

						#print "Prazdny seznam parametru funkce"
						state = "P_FUNC"
					else:

						#print("KA str_args buffer: {}".format(str_args))
						params_list = detect_params(str_args)
						str_args = ""
						count += 1;
						#odstranit prazdne polozky z listu
						params_list = list(filter(None, params_list))
						
						if varargs == "true":
							func_dict["varargs"] = "yes"
							#varargs = "false"
						else:
							func_dict["varargs"] = "no"
						varargs = "false"	

						if args.no_duplicates:
							for obj1 in OUT:
									if isinstance(obj1, dict):
										#print(obj1)
										if func_dict["name"] == obj1["name"]:
											#print("YES: {} -- {}".format(func_dict["name"], obj1["name"]))
											flag_d = 1

						OUT.append(func_dict)
						OUT.append(params_list)

						if (args.max_par and len(params_list) > max_par) or (args.no_inline and func_dict["ret_type"].find("inline") != -1) or flag_d == 1:
							del OUT[-1]
							del OUT[-1]
							if flag_d == 1:
								flag_d = 0
								
						state = "P_FUNC"
				else:
					str_args = str_args + c
					#print("KA str_args buffer: {}".format(str_args))
					state = "LIST_ARG"
			elif state == "IGNORE":
				if c == "}":
					if typedef_flag == 1:
						state = "P_FUNC"
					else:
						func_flag = 0
						state = "START"
				else:
					state = "IGNORE"
			elif state == "RETEZ_IGNORE":
				if c == "\"":
					state = "START"
				else:
					state = "RETEZ_IGNORE"
			
#---------------------------------------------VYPIS DO XML-----------------------------------------------------
#print(OUT)

dom = xml.dom.getDOMImplementation()
tree = dom.createDocument(None, "functions", None)
root = tree.documentElement

root.setAttribute("dir", dir_vypis)
i = 1
#element = tree.createElement("function")
for obj in OUT:
#	element = tree.createElement("function")
	#atr = tree.createElement("param")
	if isinstance(obj, dict):
		element = tree.createElement("function")
		element.setAttribute("file", obj["file"])
		element.setAttribute("name", obj["name"])
		element.setAttribute("varargs", obj["varargs"])
		element.setAttribute("rettype", obj["ret_type"])
	elif isinstance(obj, list):
		if obj:								#neni-li seznam parametru funkce prazdny -->vytvorit do xml element 'attributes'
			#atr = tree.createElement("param")
			obj = list(filter(None, obj))
			#if len(obj) <= max_par
			i = 1
			for attribute in obj:
				atr = tree.createElement("param")
				if attribute != "":
					atr.setAttribute("number", str(i))
					atr.setAttribute("type", attribute)
					i = i + 1
		
				element.appendChild(atr)
	root.appendChild(element)
	#root.appendChild(element)
#root.appendChild(element)

if pretty_xml == "":		
	if file_output != "":			
		fout = open(file_output, "w", encoding="utf-8")
		tree.writexml(fout, encoding="utf-8")
	else:
		tree.writexml(sys.stdout, encoding="utf-8")
else:	
	pretty_x = '<?xml version="1.0" encoding="utf-8"?>\n'
	pretty_x = pretty_x + root.toprettyxml(indent=pretty_xml, newl="\n")
	if file_output != "":			
		fout = open(file_output, "w", encoding="utf-8")
		fout.write(pretty_x)
	else:
		print(pretty_x)
























