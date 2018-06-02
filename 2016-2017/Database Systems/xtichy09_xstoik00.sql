-- Lovci mamutov
-- Authors: 
-- xtichy09
-- xstoik00

drop table zabil;
drop table stopar;
drop table lovec;
drop table saman;
drop table neandrtalec;
drop table spatren;
drop table vyprava;
drop table jama;
drop table mamut;
drop table stanoviste;
drop table jeskyne;
drop table kmen;

--
-- Table structure for kmen
-- (2) IN REFERENCES
-- jeskyne 		- fk_kmen 		-> kmen
-- neandrtalec 	- fk_kmen 		-> kmen
--
CREATE TABLE kmen 
( id_kmen NUMERIC(5,0),
  nazev VARCHAR(30) NOT NULL,
  sur_x NUMERIC(3,0) NOT NULL,
  sur_y NUMERIC(3,0) NOT NULL,
  agresivita NUMERIC(3,0) NOT NULL,
  PRIMARY KEY (id_kmen)
);

drop sequence seq_id_kmen;
create sequence seq_id_kmen start with 1;

-- Dumping data for table kmen
INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Vysokohorsky bobry', 500, 100, 200);
INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Podzemne lasice', 999, 40, 10);
INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Tulave lisky', 800, 800, 100);
INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Agresivne koritnacky', 110, 750, 800);

--INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Navacho', 60, 45, 650);
--INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Gurony', 452, 150, 300);
-- END Table structure for kmen
--


-- *************************************
-- Table structure for jeskyne
-- (2) IN REFERENCES
-- stanoviste 	- fk_jeskyne 	-> jeskyne
-- neandrtalec  - fk_jeskyne 	-> jeskyne
-- (1) OUT REFERENCES
-- fk_kmen 		-> kmen 		- id_kmen
--
CREATE TABLE jeskyne (
  id_jeskyne numeric(5,0),
  nazev varchar(30) NOT NULL,
  sur_x numeric(3,0) NOT NULL,
  sur_y numeric(3,0) NOT NULL,
  kapacita numeric(3,0) NOT NULL,
  fk_kmen numeric(5,0) NOT NULL,
  PRIMARY KEY (id_jeskyne),
  FOREIGN KEY (fk_kmen) REFERENCES kmen(id_kmen) ON DELETE CASCADE
);

drop sequence seq_id_jeskyne;
create sequence seq_id_jeskyne start with 1;

-- Dumping data for table jeskyne
-- Vysokohorske bobry
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Chladna dira', 450, 110, 10, 1);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Levova jeskyne', 550, 70, 20, 1);
-- Podzemne lasice
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Lasicia nora', 950, 50, 70, 2);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Daleky obchvat', 970, 130, 15, 2);
-- Tulave lisky
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Tulava nora', 750, 820, 135, 3);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Lisci utulna', 780, 840, 50, 3);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Klbko stastia', 840, 820, 60, 3);
-- Agresivne koritnacky
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Kruniri lihen', 130, 755, 110, 4);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Koritnacia zbrojirna', 120, 765, 60, 4);
INSERT INTO jeskyne VALUES (seq_id_jeskyne.nextval, 'Koritnacia tlama', 100, 740, 20, 4);
-- END Table structure for jeskyne
-- *************************************


-- *************************************
-- Table structure for stanoviste
-- (3) IN REFERENCES
-- neandrtalec 	- fk_stanoviste 	-> stanoviste
-- jama 		- fk_stanoviste 	-> stanoviste
-- spatren 		- fk_stanoviste 	-> stanoviste
-- (1) OUT REFERENCES
-- fk_jeskyne 	-> jeskyne 			-> id_jeskyne
CREATE TABLE stanoviste (
  id_stanoviste numeric(5,0),
  nazev varchar(50) NOT NULL,
  sur_x numeric(3,0) NOT NULL,
  sur_y numeric(3,0) NOT NULL,
  kapacita numeric(2,0) NOT NULL,
  fk_jeskyne numeric(5,0) NOT NULL,
  PRIMARY KEY (id_stanoviste),
  FOREIGN KEY (fk_jeskyne) REFERENCES jeskyne(id_jeskyne) ON DELETE CASCADE
);

drop sequence seq_id_stanoviste;
create sequence seq_id_stanoviste start with 1;

-- Dumping data for table stanoviste
-- Vysokohorske bobry
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Severna vyhlidka', 445, 100, 10, 1);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Bobri zrak', 455, 115, 5, 1);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Levove oko', 555, 75, 5, 2);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Bobri prst', 560, 80, 10, 2);
-- Podzemne lasice
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lasicia bystrozrakost', 955, 55, 10, 3);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lasicia vyhlidka', 950, 52, 5, 3);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Vyhlidka delekeho obchvatu', 972, 139, 7, 4);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lasicia pozorovatelna', 978, 135, 15, 4);
-- Tulave lisky
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Tulava vyhlidka', 755, 825, 5, 5);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lisci zrak', 750, 823, 20, 5);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Stratena vyhlidka', 782, 843, 3, 6);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lisci bystrozrakost', 777, 839, 4, 6);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Stastny zrak', 840, 825, 10, 7);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Lisci veza', 839, 815, 13, 7);
-- Agresivne koritnacky
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Kruniri otvor', 135, 755, 11, 8);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Koritnaci spojka', 128, 753, 7, 8);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Zbrojarenska vez', 121, 770, 18, 9);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'AK-47', 120, 755, 10, 9);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Tlamy dech', 98, 735, 10, 10);
INSERT INTO stanoviste VALUES (seq_id_stanoviste.nextval, 'Agresivna vyhlidka', 97, 745, 15, 10);
-- END Table structure for stanoviste
-- *************************************


-- *************************************
-- Table structure for mamut
-- (3) IN REFERENCES
-- vyprava 	- fk_mamut 		-> mamut
-- spatren  - fk_mamut 		-> mamut
-- zabil 	- fk_mamut 		-> mamut
CREATE TABLE mamut (
  id_mamut numeric(5,0),
  jmeno varchar(30) NOT NULL,
  znaky varchar(30) NOT NULL,
  charakter varchar(30) NOT NULL,
  zvyk varchar(30) NOT NULL,
  vek numeric(3,0),
  vaha numeric(5,0),
  PRIMARY KEY (id_mamut)
);

drop sequence seq_id_mamut;
create sequence seq_id_mamut start with 1;

-- Dumping data for table mamut
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Drtihlav ostrochvosty', 'jazvy', 'agresivny', 'utoci po vsem', NULL, 5800);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Sprinter senior', 'hubeny', 'mierny', 'rychle utika', 30, 4200);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Tankodrom tlusty', 'tlusy', 'mierny', 'vubec neutika', 55, 8500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Usihlav obrovsky', 'obrovske usi', 'mierumilovny', 'neboji se', 7, 1500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Smradoch dlhosrsty', 'smrdi', 'mierny', 'nekupe sa', 20, 6500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Krasnovlas pekny', 'pekna srst', 'mierny', 'kupe sa casto', 18, 6200);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Sprinter junior', 'hubeny', 'mierny', 'rychle utika', 16, 3500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Smrtos strasny', 'cervene oci', 'agresivny', 'utoci', NULL, 5300);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Tupohlav senior', 'ploska hlava', 'agresivny', 'utoci', 45, 7500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Tupohlav junior', 'ploska hlava', 'mierne agresivny', 'nekdy utoci', 25, 6500);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Zabijak legendarny', 'velke kly', 'velmi agresivny', 'vyhledava kontakt', 43, 8200);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Vlasatik tverdohlavy', 'Dlouhe vlasy', 'neagresivni', 'vyhledava kontakt', 21, 9200);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Velkouchy velikan', 'velke usi', 'mirne agresivny', 'rychle utika', 56, 9700);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Drzy prizivnik', 'Bile nohy', 'velmi agresivny', 'muze zabit', 36, 9800);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Velkonoh cerny', 'cerne vlasy', 'velmi agresivny', 'nekdy utoci', 47, 8200);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Mirolib bilooky', 'bile oci', 'mirne agresivni', 'vyhledava kontakt', 20, 6700);
INSERT INTO mamut VALUES (seq_id_mamut.nextval, 'Andrej Tichy', 'obrovsky prdel', 'mirne agresivni', 'kdyz te uvidi muze sezrat', 25, 9900);
-- END Table structure for mamut
-- *************************************

-- *************************************
-- Table structure for jama
-- (1) IN REFERENCES
-- vyprava 	- fk_jama 		-> jama
-- (1) OUT REFERENCES
-- fk_stanoviste 	-> stanoviste 	- id_stanoviste
CREATE TABLE jama (
	id_jama numeric(5,0),
	sur_x numeric(3,0) NOT NULL,
	sur_y numeric(3,0) NOT NULL,
	kapacita numeric(3,0) NOT NULL,
	fk_stanoviste numeric(5,0) NOT NULL,
  PRIMARY KEY (id_jama),
	FOREIGN KEY (fk_stanoviste) REFERENCES stanoviste(id_stanoviste) ON DELETE CASCADE
);

drop sequence seq_id_jama;
create sequence seq_id_jama start with 1;

-- Dumping data for table jama
-- Severna vyhlidka 445/100
INSERT INTO jama VALUES (seq_id_jama.nextval, 445, 102, 1, 1);
INSERT INTO jama VALUES (seq_id_jama.nextval, 443, 101, 2, 1);
-- Bobri zrak 445/115
INSERT INTO jama VALUES (seq_id_jama.nextval, 444, 115, 2, 2);
INSERT INTO jama VALUES (seq_id_jama.nextval, 444, 116, 2, 2);
-- Levove oko 555/75
INSERT INTO jama VALUES (seq_id_jama.nextval, 555, 77, 1, 3);
INSERT INTO jama VALUES (seq_id_jama.nextval, 556, 77, 1, 3);
-- Bobri prst 560/80
INSERT INTO jama VALUES (seq_id_jama.nextval, 559, 75, 1, 4);
INSERT INTO jama VALUES (seq_id_jama.nextval, 558, 74, 1, 4);
-- Lasicia bystrozrakost 955/55
INSERT INTO jama VALUES (seq_id_jama.nextval, 960, 57, 1, 5);
INSERT INTO jama VALUES (seq_id_jama.nextval, 963, 60, 2, 5);
-- Lasicia vyhlidka 950/52
INSERT INTO jama VALUES (seq_id_jama.nextval, 952, 53, 1, 6);
INSERT INTO jama VALUES (seq_id_jama.nextval, 952, 54, 2, 6);
-- Vyhlidka dalekeho obchvatu 972/139
INSERT INTO jama VALUES (seq_id_jama.nextval, 973, 140, 1, 7);
INSERT INTO jama VALUES (seq_id_jama.nextval, 970, 138, 2, 7);
-- Lasicia pozorovatelna 978/135
INSERT INTO jama VALUES (seq_id_jama.nextval, 980, 137, 1, 8);
INSERT INTO jama VALUES (seq_id_jama.nextval, 978, 137, 2, 8);
-- Tulava vyhlidka 755/825
INSERT INTO jama VALUES (seq_id_jama.nextval, 756, 826, 1, 9);
INSERT INTO jama VALUES (seq_id_jama.nextval, 757, 827, 2, 9);
-- Lisci zrak 750/823
INSERT INTO jama VALUES (seq_id_jama.nextval, 751, 823, 1, 10);
INSERT INTO jama VALUES (seq_id_jama.nextval, 752, 823, 2, 10);
-- Stratena vyhlidka 782/843 
INSERT INTO jama VALUES (seq_id_jama.nextval, 783, 844, 1, 11);
INSERT INTO jama VALUES (seq_id_jama.nextval, 783, 845, 2, 11);
-- Lisci bystrozrakost 777/839
INSERT INTO jama VALUES (seq_id_jama.nextval, 778, 840, 1, 12);
INSERT INTO jama VALUES (seq_id_jama.nextval, 779, 840, 2, 12);
-- Stastny zrak 840/825
INSERT INTO jama VALUES (seq_id_jama.nextval, 841, 825, 1, 13);
INSERT INTO jama VALUES (seq_id_jama.nextval, 842, 825, 2, 13);
-- Lisci veza 839/815
INSERT INTO jama VALUES (seq_id_jama.nextval, 838, 816, 1, 14);
INSERT INTO jama VALUES (seq_id_jama.nextval, 838, 813, 2, 14);
-- Kruniri otvor 135/755
INSERT INTO jama VALUES (seq_id_jama.nextval, 135, 760, 1, 15);
INSERT INTO jama VALUES (seq_id_jama.nextval, 136, 760, 2, 15);
-- Koritnaci spojka 128/753
INSERT INTO jama VALUES (seq_id_jama.nextval, 126, 750, 1, 16);
INSERT INTO jama VALUES (seq_id_jama.nextval, 126, 749, 2, 16);
-- Zbrojarenska vez 121/770
INSERT INTO jama VALUES (seq_id_jama.nextval, 120, 770, 2, 17);
INSERT INTO jama VALUES (seq_id_jama.nextval, 119, 770, 1, 17);
-- AK-47 120/755
INSERT INTO jama VALUES (seq_id_jama.nextval, 113, 775, 1, 18);
INSERT INTO jama VALUES (seq_id_jama.nextval, 112, 775, 2, 18);
-- Tlamy dech 98/735
INSERT INTO jama VALUES (seq_id_jama.nextval, 95, 735, 2, 19);
INSERT INTO jama VALUES (seq_id_jama.nextval, 96, 736, 3, 19);
-- Agresivna vyhlidka 97/745
INSERT INTO jama VALUES (seq_id_jama.nextval, 96, 741, 1, 20);
INSERT INTO jama VALUES (seq_id_jama.nextval, 96, 742, 2, 20);


-- END Table structure for jama
-- *************************************


-- *************************************
-- Table structure for vyprava
-- (2) IN REFERENCES
-- neandrtalec 	- fk_vyprava 	-> vyprava
-- zabil 		- fk_vyprava 	-> vyprava
-- (2) OUT REFERENCES
-- fk_jama 		-> jama 		- id_jama
-- fk_mamut 	-> mamut 		- id_mamut
CREATE TABLE vyprava (
	id_vyprava numeric(5,0),
	zacatek date NOT NULL,
	konec date NOT NULL,
	stav varchar(60) NOT NULL,
	uspesnost numeric(1,0) NOT NULL,
	fk_jama numeric(5,0) NOT NULL,
	fk_mamut numeric(5,0) NOT NULL,
  PRIMARY KEY (id_vyprava),
	FOREIGN KEY (fk_jama) REFERENCES jama(id_jama) ON DELETE CASCADE,
	FOREIGN KEY (fk_mamut) REFERENCES mamut(id_mamut) ON DELETE CASCADE
);

drop sequence seq_id_vyprava;
create sequence seq_id_vyprava start with 1;

-- Dumping data for table vyprava
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'drtihlav vyhral', 0, 2, 1);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 2, 14);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'utekl', 0, 3, 4);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 15, 10);

INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 15, 6);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 15, 5);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'utekl', 0, 15, 9);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 26, 13);

INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 17, 8);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 26, 15);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 38, 2);
INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 'uspesny lov', 1, 24, 12);

INSERT INTO vyprava VALUES (seq_id_vyprava.nextval, to_date('30.4.2016', 'dd.mm.yyyy'), to_date('2.5.2016', 'dd.mm.yyyy'), 'uspesny lov', 1, 26, 17);
-- END Table structure for vyprava
-- *************************************


-- *************************************
-- Table structure for spatren
-- (2) OUT REFERENCES
-- fk_stanoviste	-> stanoviste 	- id_stanoviste
-- fk_mamut 		-> mamut 		- id_mamut
CREATE TABLE spatren (
	id_spatren numeric(5,0),
	od date NOT NULL,
	do date NOT NULL,
	fk_stanoviste numeric(5,0) NOT NULL,
	fk_mamut numeric(5,0) NOT NULL,
	PRIMARY KEY (id_spatren),
  FOREIGN KEY (fk_stanoviste) REFERENCES stanoviste(id_stanoviste) ON DELETE CASCADE,
  FOREIGN KEY (fk_mamut) REFERENCES mamut(id_mamut) ON DELETE CASCADE
);

drop sequence seq_id_spatren;
create sequence seq_id_spatren start with 1;

-- Dumping data for table spatren
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'), 1, 14);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('14.5.1991', 'dd.mm.yyyy'), to_date('15.5.1991', 'dd.mm.yyyy'), 8, 10);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('18.6.1991', 'dd.mm.yyyy'), to_date('18.6.1991', 'dd.mm.yyyy'), 8, 6);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('24.7.1991', 'dd.mm.yyyy'), to_date('25.7.1991', 'dd.mm.yyyy'), 8, 5);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('19.1.1992', 'dd.mm.yyyy'), to_date('20.1.1992', 'dd.mm.yyyy'), 13, 13);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('24.4.1992', 'dd.mm.yyyy'), to_date('24.4.1992', 'dd.mm.yyyy'), 9, 8);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('24.5.1992', 'dd.mm.yyyy'), to_date('28.5.1992', 'dd.mm.yyyy'), 13, 15);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('16.9.1993', 'dd.mm.yyyy'), to_date('16.9.1993', 'dd.mm.yyyy'), 19, 2);
INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('18.2.1994', 'dd.mm.yyyy'), to_date('19.2.1994', 'dd.mm.yyyy'), 12, 12);

INSERT INTO spatren VALUES (seq_id_spatren.nextval, to_date('30.4.2016', 'dd.mm.yyyy'), to_date('2.5.2016', 'dd.mm.yyyy'), 10, 17);



-- END Table structure for spatren
-- *************************************


-- *************************************
-- Table structure for neandrtalec
-- IS-A specialization to [saman, lovec, stopar]
-- (4) IN REFERENCES
-- zabil 		- fk_neandrtalec 	-> neandrtalec
-- saman 		- fk_neandrtalec 	-> neandrtalec 
-- lovec 		- fk_neandrtalec 	-> neandrtalec 
-- stopar 		- fk_neandrtalec 	-> neandrtalec 
-- (4) OUT REFERENCES
-- fk_kmen 			-> kmen 		- id_kmen
-- fk_jeskyne 		-> jeskyne 		- id_jeskyne
-- fk_stanoviste 	-> stanoviste 	- id_stanoviste
-- fk_vyprava 		-> vyprava 		- id_vyprava
CREATE TABLE neandrtalec (
	RC numeric(10,0) NOT NULL,
	jmeno varchar(30) NOT NULL,
	narozeni date NOT NULL,
	umrti date,
	pohlavi varchar(30),
	zdravi numeric(3,0) NOT NULL,
	specializace numeric(1,0) NOT NULL,
	fk_kmen numeric(5,0) NOT NULL,
	fk_jeskyne numeric(5,0),
	fk_stanoviste numeric(5,0),
	fk_vyprava numeric(5,0),
	PRIMARY KEY (RC),
  FOREIGN KEY (fk_kmen) REFERENCES kmen(id_kmen) ON DELETE CASCADE,
  FOREIGN KEY (fk_jeskyne) REFERENCES jeskyne(id_jeskyne) ON DELETE CASCADE,
  FOREIGN KEY (fk_stanoviste) REFERENCES stanoviste(id_stanoviste) ON DELETE CASCADE,
  FOREIGN KEY (fk_vyprava) REFERENCES vyprava(id_vyprava) ON DELETE CASCADE
);

-- Dumping data for table neandrtalec
INSERT INTO neandrtalec VALUES (9104247994, 'Smradoch Dlhosrsty', to_date('24.4.1965', 'dd.mm.yyyy'), NULL, 'muz', 100, 1, 1, 1, NULL, NULL);
INSERT INTO neandrtalec VALUES (9104247995, 'Smradoska Dlhosrstova',to_date('29.6.1967', 'dd.mm.yyyy'), NULL, 'zena', 99, 1, 1, 1, NULL, NULL);
INSERT INTO neandrtalec VALUES (9104247996, 'Smradoch Mladsi',to_date('21.12.1993', 'dd.mm.yyyy'), NULL, 'muz', 999, 2, 1, 2, NULL, 4);

INSERT INTO neandrtalec VALUES (8104247995, 'Ragnar Lothbrok',to_date('17.2.1972', 'dd.mm.yyyy'), NULL, 'muz', 500, 1, 2, 3, NULL, NULL);
INSERT INTO neandrtalec VALUES (8103247994, 'Montesuma Lothbrok',to_date('06.05.1971', 'dd.mm.yyyy'), NULL, 'zena', 550, 1, 2, 3, NULL, NULL);
INSERT INTO neandrtalec VALUES (8104247996, 'Bjorn Lothbrok',to_date('18.09.1986', 'dd.mm.yyyy'), NULL, 'muz', 800, 2, 2, NULL, NULL, NULL);
INSERT INTO neandrtalec VALUES (8104247997, 'Annie Lothbrok',to_date('02.07.1992', 'dd.mm.yyyy'), NULL, 'zena', 900, 2, 2, 4, 4, 3);

INSERT INTO neandrtalec VALUES (4103247993, 'Pontiak Dlouhohlavy',to_date('14.4.1984', 'dd.mm.yyyy'), NULL, 'muz', 600, 2, 3, 5, NULL, 3);
INSERT INTO neandrtalec VALUES (4103247994, 'Raduska Dlouhohlava',to_date('15.1.1984', 'dd.mm.yyyy'), NULL, 'zena', 650, 1, 3, 5, NULL, NULL);
INSERT INTO neandrtalec VALUES (4103247995, 'Lakmej Dlouhohlavy',to_date('5.10.1985', 'dd.mm.yyyy'), NULL, 'muz', 600, 2, 3, 6, NULL, 2);
INSERT INTO neandrtalec VALUES (4103247996, 'Ragmir Dlouhohlavy',to_date('22.8.1987', 'dd.mm.yyyy'), NULL, 'muz', 400, 1, 3, 7, NULL, NULL);

INSERT INTO neandrtalec VALUES (5103247993, 'Joui Beladonna',to_date('15.9.1954', 'dd.mm.yyyy'), to_date('11.6.1998', 'dd.mm.yyyy'), 'muz', 99, 3, 4, 8, NULL, NULL);
INSERT INTO neandrtalec VALUES (5103247994, 'Armuta Beladonna',to_date('4.11.1958', 'dd.mm.yyyy'), NULL, 'zena', 99, 1, 4, 8, NULL, NULL);
INSERT INTO neandrtalec VALUES (5103247995, 'Marie Beladonna',to_date('19.8.1967', 'dd.mm.yyyy'), NULL, 'zena', 200, 1, 4, 9, NULL, NULL);
INSERT INTO neandrtalec VALUES (5103247996, 'Lanni Beladonna',to_date('2.1.1970', 'dd.mm.yyyy'), NULL, 'muz', 300, 2, 4, 9, NULL, 1);
INSERT INTO neandrtalec VALUES (5103247997, 'Josef Beladonna',to_date('12.2.1973', 'dd.mm.yyyy'), NULL, 'muz', 500, 3, 4, 10, 2, NULL);
INSERT INTO neandrtalec VALUES (5103247998, 'Nastasia Stoika',to_date('25.4.1995', 'dd.mm.yyyy'), NULL, 'zena', 999, 2, 4, 10, NULL, 13);





-- END Table structure for neandrtalec
-- *************************************

CREATE TABLE saman (
	fk_RC numeric(10,0),
	dovednost varchar(30),
	zvoleni date NOT NULL,
	konec date NOT NULL,
	PRIMARY KEY (fk_RC),
  FOREIGN KEY (fk_RC) REFERENCES neandrtalec(RC) ON DELETE CASCADE
);

-- Dumping data for table saman
INSERT INTO saman VALUES (9104247994, 'kuzelnik', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (9104247995, 'carodejnice', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (8104247995, 'vladce', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (8103247994, 'kuzelnik', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (4103247994, 'dobyvatel', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (5103247994, 'dobyvatel', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));
INSERT INTO saman VALUES (5103247995, 'dobyvatel', to_date('24.4.1991', 'dd.mm.yyyy'), to_date('24.4.1991', 'dd.mm.yyyy'));

CREATE TABLE lovec (
	fk_RC numeric(10,0),
	utok numeric(3,0) NOT NULL,
	lvl numeric(3,0) NOT NULL,
	zbran varchar(30) NOT NULL,
	PRIMARY KEY (fk_RC),
  FOREIGN KEY (fk_RC) REFERENCES neandrtalec(RC) ON DELETE CASCADE
);

-- Dumping data for table lovec
INSERT INTO lovec VALUES (8104247996, 100, 80, 'mec');
INSERT INTO lovec VALUES (8104247997, 5, 4, 'sekera');

CREATE TABLE stopar (
	fk_RC numeric(10,0),
	rychlost numeric(3,0) NOT NULL,
	smena numeric(4,0) NOT NULL,
	PRIMARY KEY (fk_RC),
  FOREIGN KEY (fk_RC) REFERENCES neandrtalec(RC) ON DELETE CASCADE
);

-- Dumping data for table stopar
INSERT INTO stopar VALUES (5103247997, 70, 480);

-- Zabil 
-- (3) REFERENCES
-- fk_RC 		-> neandrtalec 	- RC
-- fk_vyprava 	-> vyprava 		- id_vyprava
-- fk_mamut 	-> mamut 		- id_mamut 
CREATE TABLE zabil (
	fk_RC numeric(10,0),
	fk_vyprava numeric(5,0) NOT NULL,
	fk_mamut numeric(5,0) NOT NULL,
	okolnosti varchar(30),
	datum date NOT NULL,
	PRIMARY KEY (fk_RC, fk_vyprava, fk_mamut),
  FOREIGN KEY (fk_vyprava) REFERENCES vyprava(id_vyprava) ON DELETE CASCADE,
  FOREIGN KEY (fk_RC) REFERENCES neandrtalec(RC) ON DELETE CASCADE,
  FOREIGN KEY (fk_mamut) REFERENCES mamut(id_mamut) ON DELETE CASCADE
);


-- Dumping data for table zabil
INSERT INTO zabil VALUES (8104247996, 1, 1, 'nahoda', to_date('24.4.1991', 'dd.mm.yyyy'));

commit;

-- Select scripts

-- Spojeni dvou tabulek
-- Ktery neandrtalci jsou v kterem kmeni
SELECT N.RC, N.JMENO, N.NAROZENI, N.ZDRAVI, K.NAZEV AS KMEN, K.AGRESIVITA
FROM neandrtalec N, kmen K
WHERE K.id_kmen = N.fk_kmen
ORDER BY 5;

-- Spojeni dvou tabulek
-- Ktere stanoviste ma ake jamy
SELECT S.NAZEV AS STANOVISTE, J.ID_JAMA AS JAMA, J.SUR_X AS X, J.SUR_Y AS Y, J.KAPACITA
FROM stanoviste S, jama J
WHERE J.fk_stanoviste = S.id_stanoviste
ORDER BY 2;

-- Spojeni tri tabulek
-- Zobrazi stanoviste, jeho suradnice, kapacitu, ke ktere jaskyni patri a kteremu kmenu
SELECT S.NAZEV AS STANOVISTE, S.SUR_X AS X, S.SUR_Y AS Y, S.KAPACITA, J.NAZEV AS JESKYNE, KM.NAZEV AS KMEN
FROM kmen KM, jeskyne J, stanoviste S
WHERE J.fk_kmen=KM.id_kmen and S.fk_jeskyne=J.id_jeskyne
ORDER BY 1;

-- Group by a agregacne funkce count
-- Zobraz kmen, pozici a kolik má neandrtálcov
SELECT kmen.NAZEV AS KMEN, kmen.SUR_X AS X, kmen.SUR_Y AS Y, count(kmen.NAZEV) AS pocet_neand
FROM neandrtalec
LEFT OUTER JOIN kmen ON neandrtalec.FK_KMEN = kmen.ID_KMEN
GROUP BY kmen.NAZEV, kmen.SUR_X, kmen.SUR_Y
ORDER BY 1;

-- Group by a agregacne funkce count
-- Zobraz jamu, jej polohu a v kolkych vypravach bola pouzita
SELECT jama.ID_JAMA, jama.SUR_X AS X, jama.SUR_Y AS Y, count(jama.ID_JAMA) as pocet
FROM vyprava
LEFT OUTER JOIN jama ON vyprava.FK_JAMA = jama.ID_JAMA
GROUP BY jama.ID_JAMA, jama.SUR_X, jama.SUR_Y
ORDER BY 1;

-- Klauzule exists
-- Zobraz mamutov ktory boli uz nekdy spatreni
SELECT m.JMENO AS MAMUT, m.ZNAKY, m.CHARAKTER, m.ZVYK, m.VAHA  
FROM mamut m
WHERE exists (select * from spatren where spatren.FK_MAMUT = m.ID_MAMUT);

-- Klauzule IN
-- Vypise jaskyne v ktorych bydli neandertalci ktore su aktualne na vyprave
SELECT j.NAZEV AS JESKYNE, j.SUR_X AS X, j.SUR_Y AS Y, j.KAPACITA 
FROM jeskyne j, neandrtalec n 
WHERE n.fk_jeskyne = j.id_jeskyne and j.id_jeskyne IN (SELECT n.fk_jeskyne from neandrtalec n where n.fk_vyprava IS NOT NULL )
GROUP BY j.nazev, j.SUR_X, j.SUR_Y, j.kapacita;
----------------------------------------------------------------------------------
    ---------------------------4 CAST PROJEKTU-------------------------------
----------------------------------------------------------------------------------

-----------------------------------TRIGGERY---------------------------------------
DROP TRIGGER kmen_trig;
CREATE OR REPLACE TRIGGER kmen_trig
BEFORE INSERT OR UPDATE ON Kmen
FOR EACH row
BEGIN
  :new.id_kmen:=seq_id_kmen.nextval;
end;
/

-- Show Error Trigger kmen_trig;
-- INSERT INTO kmen VALUES (NULL, 'Alpaco', 1, 1, 50);
-- INSERT INTO kmen VALUES (NULL, 'Nastiny otroci', 10, 890, 800);

DROP TRIGGER mamut_delete;
CREATE OR REPLACE TRIGGER mamut_delete
AFTER DELETE
 ON mamut
 FOR EACH ROW
BEGIN
   DELETE FROM spatren
   WHERE spatren.fk_mamut=:old.id_mamut;
   DELETE FROM vyprava
   WHERE vyprava.fk_mamut=:old.id_mamut;
   DELETE FROM zabil
   WHERE zabil.fk_mamut=:old.id_mamut;
END;
/
--Show Error Trigger mamut_delete;
-- delete from mamut
-- where mamut.jmeno = 'Drtihlav ostrochvosty';


DROP TRIGGER mamut_insert;
CREATE OR REPLACE TRIGGER mamut_insert
BEFORE INSERT OR UPDATE ON Mamut
FOR EACH row
BEGIN
  :new.id_mamut:=seq_id_mamut.nextval;
end;
/

DROP TRIGGER jama_insert;
CREATE OR REPLACE TRIGGER jama_insert
BEFORE INSERT OR UPDATE ON Jama
FOR EACH row
BEGIN
  :new.id_jama:=seq_id_jama.nextval;
end;
/


-------------------------------END OF TRIGGERS------------------------------------
----------------------------------------------------------------------------------

--------------------------------- Procedure 1-------------------------------------
--VIEW1 pro dalsi prvni proceduru
--Zobrazi jake neandertalce zlovili mamuta na vyprave, na ktere se AKTUALNE nachazi

DROP VIEW aktual_lov_info;
DROP PROCEDURE get_lov;

CREATE OR REPLACE VIEW aktual_lov_info AS
SELECT ne.rc, ne.jmeno AS lovec, v.id_vyprava, ma.id_mamut, ma.jmeno AS MAMUT, ma.znaky, ma.charakter, ma.zvyk, ma.vek, ma.vaha
FROM mamut ma, vyprava v, neandrtalec ne
WHERE v.fk_mamut=ma.id_mamut AND ne.fk_vyprava=v.id_vyprava AND ne.fk_vyprava IS NOT NULL AND v.uspesnost=1
ORDER BY 1;
--jaky neandertalec na aktualne vyprave zlovil ktereho mamuta
--> zadavame rc - a kdyz je v tabulke aktual_lov_info, vypise nam ktereho mamuta zlovil

CREATE OR REPLACE PROCEDURE get_lov(rodne_c IN NUMERIC)
AS
CURSOR cursor1 (zadane_rc AKTUAL_LOV_INFO.rc%TYPE) IS
   SELECT lovec, id_vyprava, id_mamut, mamut, znaky
   FROM aktual_lov_info
   WHERE rc=zadane_rc;
  v_gt cursor1%ROWTYPE;
BEGIN
  OPEN cursor1(rodne_c);
    DBMS_OUTPUT.enable;
    
    IF(cursor1%ISOPEN) THEN
      DBMS_OUTPUT.put_line('Cursor cursor1 is open now!');		
    ELSE
      DBMS_OUTPUT.put_line('Cursor cursor1 is close now!');		
    END IF;
     --fetch selected row
    FETCH cursor1 INTO v_gt;
     --Podminka na overeni, jestli zadane uzivatelem rodne cislo je v tabulce AKTUAL_LOV_INFO
    IF(cursor1%FOUND) THEN
      DBMS_OUTPUT.put_line('Row is Found!');
      DBMS_OUTPUT.put_line('GET DATA:'||' '||'Lovec:'||' '|| TO_CHAR(v_gt.LOVEC)||' '||'ID_Vyprava:'||' '||v_gt.ID_VYPRAVA||' '
		||'ID_Mamut:'||' '||v_gt.ID_MAMUT||' '||'Mamut:'||' '||v_gt.MAMUT||' '||'Znaky:'||' '||v_gt.ZNAKY);  
  	ELSE
    	DBMS_OUTPUT.put_line('Row is not Found! Lovec s takovym rodnym cislem nezlovil mamuta na aktualni vyprave.');
  	END IF;
  CLOSE cursor1;
END;
/

-------------------------------Overeni procedury 1
/*
DECLARE
  RODNE_C NUMBER;
BEGIN
  RODNE_C := 5103247998;
  --nebo RODNE_C := 5103247995 treba, ktere neni v teto tabulce,-> vypise chybu zadaneho rc
  GET_LOV(
    RODNE_C => RODNE_C
  );
--rollback; 
END;
*/
-----------------------------------KONEC 1 PROCEDURY-----------------------------
---------------------------------------------------------------------------------

-----------------------------------PROCEDURE 2-----------------------------------
DROP VIEW saman_info;
DROP PROCEDURE vypis_samany;

----Zobrazi neandertalce, ktere jsou samany
CREATE OR REPLACE VIEW saman_info AS
  SELECT ne.rc, ne.jmeno, ne.narozeni, ne.fk_jeskyne AS jeskyne
  FROM neandrtalec ne
  WHERE exists (select * from saman sam where sam.fk_rc = ne.rc);


--Procedura 2 je pro ukazku prace s parametry procedury typu IN OUT a OUT
--Na zacatku pouzije 1 cursor, ktery pri zadani uzivatelem rodneho cisla samana - zapise jmeno samana do "info"
--Pak pouzije druhy cursor, ktery ma stejny vstupni parametr, ktery vyhleda Jeskyni, kde bydli saman. 
--->Cislo jeskyne se zapise do parametru "cislo" typu IN OUT, tim samym zmeni obsah tohoto parametru
create or replace PROCEDURE vypis_samany (cislo IN OUT NUMBER, info OUT VARCHAR2)  
AS
CURSOR get_jmeno_saman (inpt_cislo SAMAN_INFO.rc%TYPE) IS
  SELECT jmeno FROM saman_info --into info -> fetch get_info_saman into info
  WHERE rc = inpt_cislo;
CURSOR get_jeskyne (inp_cislo SAMAN_INFO.rc%TYPE) IS
  SELECT jeskyne FROM saman_info --fetch get_jeskyne into cislo
  WHERE rc = inp_cislo;
BEGIN
  OPEN get_jmeno_saman(cislo);
    DBMS_OUTPUT.enable;
    
    IF(get_jmeno_saman%ISOPEN) THEN
      DBMS_OUTPUT.put_line('Cursor get_jmeno_saman is open now!');		
    ELSE
      DBMS_OUTPUT.put_line('Cursor is close now!');		
    END IF; 
    
    FETCH get_jmeno_saman INTO info;
    IF(get_jmeno_saman%FOUND) THEN
    	DBMS_OUTPUT.put_line('Row is Found!');
      DBMS_OUTPUT.put_line('Jmeno samana: '||TO_CHAR(info));	
      DBMS_OUTPUT.put_line('Byl pouzit parametr procedury "cislo" jako vstupni, a "info" jako vystupni.');	
  	ELSE
    	DBMS_OUTPUT.put_line('Row is not Found!');
    END IF;
  CLOSE get_jmeno_saman;
    
  OPEN get_jeskyne(cislo);
    IF(get_jmeno_saman%ISOPEN) THEN
      DBMS_OUTPUT.put_line('Cursor get_jeskyne is open now!');		
    ELSE
      DBMS_OUTPUT.put_line('Cursor is close now!');		
    END IF; 
    --menime parametr cislo
    FETCH get_jeskyne INTO cislo;
    IF(get_jeskyne%FOUND) THEN
    	DBMS_OUTPUT.put_line('Row is Found!');
      DBMS_OUTPUT.put_line('Jeskyne, v ktere bydli saman: '||TO_CHAR(cislo));
      DBMS_OUTPUT.put_line('Byl pouzit parametr procedury "cislo" jako vstupni, a pak hodnota tohoto parametru byla prepsana na cislo jeskyne.');	
      DBMS_OUTPUT.put_line('Priklad pouzit pro ukazku prace s parametrem typu IN OUT.');	
    ELSE
      DBMS_OUTPUT.put_line('Row is not Found!');
    END IF;
  CLOSE get_jeskyne;
END vypis_samany;
/

-----------------------------------------Overeni procedury 2
--(Smazat dvojite -- na zacatku, nebo overit proceduru z polozky Procedures)
-- DECLARE
--   CISLO NUMBER;
--   INFO VARCHAR2(200);
-- BEGIN
--   CISLO := 5103247994;
--         ---nebo libovol jine cislo
--   VYPIS_SAMANY(
--     CISLO => CISLO,
--     INFO => INFO
--   );
    /* Legacy output: 
  DBMS_OUTPUT.PUT_LINE('CISLO = ' || CISLO);
  */ 
--   :CISLO := CISLO;
  /* Legacy output: 
  DBMS_OUTPUT.PUT_LINE('INFO = ' || INFO);
  */ 
--   :INFO := INFO;
       ---rollback; 
--END; 

-----------------------------------END OF PROCEDURE 2-----------------------------
----------------------------------------------------------------------------------

---------------------------------INDEXY A EXPLAIN PLAN----------------------------

DROP INDEX I_FK_J;

EXPLAIN PLAN FOR
SELECT jama.ID_JAMA, jama.SUR_X AS X, jama.SUR_Y AS Y, count(jama.ID_JAMA) as pocet
FROM vyprava
LEFT OUTER JOIN jama ON vyprava.FK_JAMA = jama.ID_JAMA
GROUP BY jama.ID_JAMA, jama.SUR_X, jama.SUR_Y
ORDER BY 1;

SELECT * FROM TABLE(DBMS_XPLAN.display);

CREATE INDEX i_fk_j ON vyprava(FK_JAMA);

EXPLAIN PLAN FOR
SELECT jama.ID_JAMA, jama.SUR_X AS X, jama.SUR_Y AS Y, count(jama.ID_JAMA) as pocet
FROM vyprava
LEFT OUTER JOIN jama ON vyprava.FK_JAMA = jama.ID_JAMA
GROUP BY jama.ID_JAMA, jama.SUR_X, jama.SUR_Y
ORDER BY 1;

SELECT * FROM TABLE(DBMS_XPLAN.display);

-----------------------------END OF INDEXES AND EXPLAIN PLAN----------------------
----------------------------------------------------------------------------------

-----------------------------------------PRAVA -----------------------------------
-- Andrej - XTICHY09 - dava prava na svoje tabulky

GRANT ALL ON kmen TO xstoik00;
GRANT ALL ON jeskyne TO xstoik00;
GRANT ALL ON stanoviste TO xstoik00;
GRANT ALL ON spatren TO xstoik00;
GRANT ALL ON mamut TO xstoik00;
GRANT ALL ON neandrtalec TO xstoik00;
GRANT ALL ON vyprava TO xstoik00;
GRANT ALL ON jama TO xstoik00;
GRANT ALL ON saman TO xstoik00;
GRANT ALL ON lovec TO xstoik00;
GRANT ALL ON stopar TO xstoik00;
GRANT ALL ON zabil TO xstoik00; 
--------------------------------------END OF PRAVA--------------------------------
----------------------------------------------------------------------------------

------------------------------------MATERIALIZED VIEW-----------------------------
-- Ted' druhy clen tymu - XSTOIK00 - dela materializovany pohled (KDYZ OD JEHO JMENA, KOMENTARE DAT PRYC,
-- A NAHRADIT KOMENTAREMI CAST PRVNIHO CLENU TYMU)

/*
DROP MATERIALIZED VIEW LOG ON kmen;
DROP MATERIALIZED VIEW mv1;

CREATE MATERIALIZED VIEW LOG ON kmen WITH PRIMARY KEY, ROWID
INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW mv1
CACHE
BUILD IMMEDIATE
REFRESH FAST ON COMMIT
ENABLE QUERY REWRITE
AS
  SELECT id_kmen, nazev
  FROM kmen
  
GRANT ALL ON mv1 TO xtichy09; 
*/

-----> XTICHY09 SI ZKUSI ZOBRAZIT POHLED:
SELECT * FROM xstoik00.mv1;

-----> XSTOIK00 prida jeden radek do tabulky pro overeni aktualizaci pohledu
/*
INSERT INTO kmen VALUES (seq_id_kmen.nextval, 'Navacho', 60, 45, 650);
COMMIT;  
*/

-----> XTICHY SI TO ZAS VYZKOUSI ZOBRAZIT
SELECT * FROM xstoik00.mv1;


commit;



