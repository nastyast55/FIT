//0.0 00.0 000.0 000.000 0.1 1.0 1.1 12.0 1123.24124214124 00.1 00000000.0893098
//0e6 00e5 00e0 
//000e68 1e0 3e000 12e0000 09864e16 0478e30 9832759823614e387 
//0E6 00E5 00E0 000E689 1E0 3E000 12E0000 09864E16 0478E30 9832759823614E387
//0e+6 00e+5 00e+0 000e+689 1e+0 3e+000 12e+0000 09864e+16 0478e+30 9832759823614e+387
//0e-6 00e-5 00e-0 000e-689 1e-0 3e-000 12e-0000 09864e-16 0478e-30 9832759823614e-387
//1.0e6 234.00e5 0000234.00e0634 1.0E6 234.00E5098 0000234.00E0898
//1.0e+6 234.00e+5 0000234.00e+0634 1.0E+0 234.00E+5098 0000234.00E+0898
//1.0e-6 234.00e-5 0000234.00e-0634 1.0E-6 234.00E-5098 0000234.00E-0898

//000234e6.00e-0634 	//---> 234e6 a chyba
//0.45e  		// --> chyba
//00.213eSlk 		//--> chyba
//0.213e+jegb 		//---> chyba 

//23.456e434Yew		//-->double + simple_id 
//34.875e+40$		//-->double + simple_id
//313.0957E+09+		//-->double + op'+'

//0e0.0 00e0.0 000e0.0 	//-->chyba, exp nemuze byt v cele casti 
//000e0000.000 0e0.1 1e1.0 
//1e9.1 12e098.0 	// --> rozpozna celou cast s exp jako double,"." - error
//1123e432098.24124214124 
// 00e9.1 00000000e8.0893098
// 0E0.0 00E0.0 000E0.0 000E0000.000 0E0.1 1E1.0 1E9.1 12E098.0 1123E432098.24124214124 00E9.1 00000000E8.0893098
// 0e+0.0 00e+0.0 000e+0.0 000e+0000.000 0e+0.1 1e+1.0 1e+9.1 12e+098.0 1123e+432098.24124214124 00e+9.1 00000000e+8.0893098
// 0e-0.0 00e-0.0 000e-0.0 000e-0000.000 0e-0.1 1e-1.0 1e-9.1 12e-098.0 1123e-432098.24124214124 00e-9.1 00000000e-8.0893098
// 0E+0.0 00E+0.0 000E+0.0 000E+0000.000 0E+0.1 1E+1.0 1E+9.1 12E+098.0 1123E+432098.24124214124 00E+9.1 00000000E+8.0893098
// 0E-0.0 00E-0.0 000E-0.0 000E-0000.000 0E-0.1 1E-1.0 1E-9.1 12E-098.0 1123E-432098.24124214124 00E-9.1 00000000E-8.0893098












