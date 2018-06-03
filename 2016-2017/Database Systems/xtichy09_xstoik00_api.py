#!/usr/bin/python

#xtichy09:xstoik00

import os
import sys
import time
import cx_Oracle
import fileinput
from random import randint
from time import sleep


ip = 'gort.fit.vutbr.cz'
port = 1521
SID = 'dbgort'
dsn_tns = cx_Oracle.makedsn(ip, port, SID)

class col:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'



def delay():
	sleep(randint(1,5)/5)



if __name__ == "__main__":
  
	status = col.HEADER + 'LOG_IN' + col.ENDC
	authority = raw_input('[' + status + ']-[ NULL ] --- Please log in like a [saman|buh]: ')
	authority = authority.upper()
	
	if (authority == 'BUH') or (authority == 'SAMAN'):
		status = col.OKGREEN + 'LOG_OK' + col.ENDC
		name = raw_input('[' + status + ']-[' + authority + '] --- Login: ')
		passwd = raw_input('[' + status + ']-[' + authority + '] --- Passwd: ')
		#TODO KONTROLA

	else:
		status = col.FAIL + 'ERROR' + col.ENDC
		print('[' + status + ']-[' + authority + '] --- Unauthorized authority')
		exit(1)

	status = col.HEADER + 'CONNECTING...' + col.ENDC


	try:
		print('[' + status + '] --- Try to connect database')

		db = cx_Oracle.connect('XTICHY09', 'gnx0rcck', dsn_tns)
		cursor = db.cursor()

		status = col.OKBLUE + 'CONNECTED' + col.ENDC

		print('[' + status + '] --- OK!!!')

		status = col.HEADER + 'WAITING' + col.ENDC


	except cx_Oracle.DatabaseError as e:

		cursor.close()
		db.close()


		error, = e.args
		status = col.FAIL + 'ERROR' + col.END
		if error.code == 1017:
			print('[' + status + '] --- Please check your credentials.')
		else:
			print('[' + status + '] --- Database connection error: %s'.format(e))
		# Very important part!
		raise

	else:
		

		#Interface for Buh
		#Zrodit mamuta - add mamut
		#Zrodit lovce - add lovec
		#Zabit jednotku - kill unit
		#Vytvorit jaskynu - add cave 
		if authority == "BUH":
			print('[' + status + ']-[' + authority + '] --- Login as: ' + authority)

			while True: 

				status = col.HEADER + 'WAITING' + col.ENDC
				command = raw_input('[' + status + ']-[' + authority + '] --- Please make command: ')
				command = command.upper()

				#
				if command == "HELP":
					print('[' + status + ']-[' + authority + '] --- ' + col.WARNING + 'ADD MAMUT | ADD CAVE | INFO_BY_KMEN | SELECT_1 | SELECT_2 | SELECT_3 | SELECT_4 | SELECT_5 | SELECT_6 | SELECT_7 | EXIT' + col.ENDC)


				elif command == 'SELECT_5':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Zobraz jamu, jej polohu a v kolkych vypravach bola pouzita')
					delay()
					SQL = 'SELECT jama.ID_JAMA, jama.SUR_X AS X, jama.SUR_Y AS Y, count(jama.ID_JAMA) as pocet '
					SQL += 'FROM vyprava LEFT OUTER JOIN jama ON vyprava.FK_JAMA = jama.ID_JAMA '
					SQL += 'GROUP BY jama.ID_JAMA, jama.SUR_X, jama.SUR_Y '
					SQL += 'ORDER BY 1'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'SELECT_6':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Zobraz mamutov ktory boli uz nekdy spatreni')
					delay()
					SQL = 'SELECT m.JMENO AS MAMUT, m.ZNAKY, m.CHARAKTER, m.ZVYK, m.VAHA '
					SQL += 'FROM mamut m '
					SQL += 'WHERE exists (select * from spatren where spatren.FK_MAMUT = m.ID_MAMUT)'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'SELECT_7':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Vypise jaskyne v ktorych bydli neandertalci ktore su aktualne na vyprave')
					delay()
					SQL = 'SELECT j.NAZEV AS JESKYNE, j.SUR_X AS X, j.SUR_Y AS Y, j.KAPACITA  '
					SQL += 'FROM jeskyne j, neandrtalec n '
					SQL += 'WHERE n.fk_jeskyne = j.id_jeskyne and j.id_jeskyne IN (SELECT n.fk_jeskyne from neandrtalec n where n.fk_vyprava IS NOT NULL ) '
					SQL += 'GROUP BY j.nazev, j.SUR_X, j.SUR_Y, j.kapacita'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row


				elif command == 'ADD MAMUT':
					
					status = col.HEADER + 'MAKE_QUERY' + col.ENDC
					#INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Drtihlav ostrochvosty', 'jazvy', 'agresivny', 'utoci po vsem', NULL, 5800);
					SQL = 'INSERT INTO mamut VALUES (NULL, \'' + raw_input('[' + status + ']-[' + authority + '] --- [varchar(30)]-NAME: ') + '\', \'' 
					SQL += raw_input('[' + status + ']-[' + authority + '] --- [varchar(30)]-ZNAK: ') + '\', \'' + raw_input('[' + status + ']-[' + authority + '] --- [varchar(30)]-CHARAKTER: ')
					SQL +=  '\', \'' + raw_input('[' + status + ']-[' + authority + '] --- [varchar(30)]-ZVYK: ') + '\', ' + raw_input('[' + status + ']-[' + authority + '] --- [numeric(3,0)]-VEK: ')
					SQL += ', ' + raw_input('[' + status + ']-[' + authority + '] --- [numeric(5,0)]-VAHA: ') + ')'
					
					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					delay()

					cursor.execute(SQL)
					db.commit()

					print('[' + status + ']-[' + authority + '] --- OK !!!')
					delay()
					status = col.OKGREEN + 'TEST_OK' + col.ENDC
					SQL = 'SELECT * FROM MAMUT ORDER BY 1 DESC'
					cursor.execute(SQL)
					for row in cursor:
						print ('[' + status + ']-[' + authority + '] --- ' + str(row))
						break

				elif command == 'ADD CAVE':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC
					SQL = 'INSERT INTO jama VALUES (NULL, ' + raw_input('[' + status + ']-[' + authority + '] --- [numeric(3,0)]-X: ') + ', '
					SQL += raw_input('[' + status + ']-[' + authority + '] --- [numeric(3,0)]-Y: ') + ', ' + raw_input('[' + status + ']-[' + authority + '] --- [numeric(3,0)]-KAPACITA: ') + ', '

					SQL2 = 'SELECT S.id_stanoviste, S.nazev, S.sur_x, S.sur_y FROM stanoviste S ORDER BY 1'
					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL2)
					delay()
					cursor.execute(SQL2)
					for row in cursor:
						print row
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')


					status = col.HEADER + 'MAKE_QUERY' + col.ENDC
					SQL += raw_input('[' + status + ']-[' + authority + '] --- [numeric(5,0)]-FK_ID_STANOVISTE: ' ) + ')'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					db.commit()
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')

					delay()
					status = col.OKGREEN + 'TEST_OK' + col.ENDC
					SQL = 'SELECT * FROM JAMA ORDER BY 1 DESC'
					cursor.execute(SQL)
					for row in cursor:
						print ('[' + status + ']-[' + authority + '] --- ' + str(row))
						break

				elif command == 'SELECT_1':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Ktery neandrtalci jsou v kterem kmeni')
					delay()
					SQL = 'SELECT N.RC, N.JMENO, N.NAROZENI, N.ZDRAVI, K.NAZEV, K.AGRESIVITA '
					SQL += 'FROM neandrtalec N, kmen K '
					SQL += 'WHERE K.id_kmen = N.fk_kmen '
					SQL += 'ORDER BY 5'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row


				elif command == 'SELECT_2':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Ktere stanoviste ma ake jamy')
					delay()
					SQL = 'SELECT S.NAZEV, J.ID_JAMA, J.SUR_X, J.SUR_Y, J.KAPACITA '
					SQL += 'FROM stanoviste S, jama J '
					SQL += 'WHERE J.fk_stanoviste = S.id_stanoviste '
					SQL += 'ORDER BY 2'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'SELECT_3':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Zobrazi stanoviste, jeho suradnice, kapacitu, ke ktere jaskyni patri a kteremu kmenu')
					delay()
					SQL = 'SELECT S.NAZEV, S.SUR_X, S.SUR_Y, S.KAPACITA, J.NAZEV, KM.NAZEV '
					SQL += 'FROM kmen KM, jeskyne J, stanoviste S '
					SQL += 'WHERE J.fk_kmen=KM.id_kmen and S.fk_jeskyne=J.id_jeskyne '
					SQL += 'ORDER BY 1'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'SELECT_4':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Zobraz kmen, pozici a kolik ma neandrtalcov')
					delay()
					SQL = 'SELECT kmen.NAZEV, kmen.SUR_X, kmen.SUR_Y, count(kmen.NAZEV) AS pocet_neand '
					SQL += 'FROM neandrtalec LEFT OUTER JOIN kmen ON neandrtalec.FK_KMEN = kmen.ID_KMEN '
					SQL += 'GROUP BY kmen.NAZEV, kmen.SUR_X, kmen.SUR_Y '
					SQL += 'ORDER BY 1'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'INFO_BY_KMEN':

					status = col.HEADER + 'MAKE_QUERY' + col.ENDC

					print('[' + status + ']-[' + authority + '] --- ' + 'Zobraz kmen podle ID, pozici a kolik ma neandrtalcov')
					delay()
					SQL = 'SELECT kmen.NAZEV, kmen.SUR_X, kmen.SUR_Y, kmen.AGRESIVITA, count(kmen.NAZEV) AS pocet_neand '
					SQL += 'FROM neandrtalec LEFT OUTER JOIN kmen ON neandrtalec.FK_KMEN = kmen.ID_KMEN and kmen.ID_KMEN = ' + raw_input('[' + status + ']-[' + authority + '] --- [numeric(5,0)]-ID_KMEN: ' )
					SQL += 'GROUP BY kmen.NAZEV, kmen.SUR_X, kmen.SUR_Y '
					SQL += 'ORDER BY 1'

					status = col.HEADER + 'SEND_QUERY' + col.ENDC
					print('[' + status + ']-[' + authority + '] --- ' + SQL)
					cursor.execute(SQL)
					delay()
					print('[' + status + ']-[' + authority + '] --- OK !!!')
					for row in cursor:
						print row

				elif command == 'EXIT':
					cursor.close()
					db.close()
					exit(0)


		elif authority == "SAMAN":
			command = raw_input('[' + status + ']-[' + authority + '] --- Saman is not implemented sorry: ')
			cursor.close()
			db.close()
			exit(0)
