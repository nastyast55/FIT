package ija.files;

import java.io.*;
import java.util.Scanner;
import ija.blocks.*;


public class readFile {
	static int blockID = 1, dataType = 2, input1 = 3, input2 = 4;		//constanty
	public static int rows = 0, cols = 0;

	/**
	 * Function processes input file, parse it, create objects from description in file
	 * and add it to matrix of objects of type AbstractBlock
	 *
	 * @param filePath - event to control what to do on mouse release
	 * */
	public static AbstractBlock[] createMatrixFromFile(String filePath) throws Exception{
		String str = "", complexStr = "";
		String[] splitStr;
		double inpt1;
		AbstractBlock[] matrix = null;
		int i = 0, j = 0, index = 0, in1, in2;

		File file = new File(filePath);
		Scanner sc = new Scanner(file);

		str = sc.nextLine();		//First line - size of matrix
		splitStr = str.split(" ");

		/* Create matrix */
		rows = Integer.parseInt(splitStr[index]);
		cols = Integer.parseInt(splitStr[index + 1]);
		matrix = new AbstractBlock[rows * cols];

		for(i = 0; i < cols; i++){
			for(j = 0; j < rows; j++){
				str = sc.nextLine();
				splitStr = str.split(" ");

				switch(splitStr[0]){
					case "val":

						try {
							Double.parseDouble(splitStr[input1]);		//je cislo double
						} catch (NumberFormatException e) {
							complexStr = splitStr[input1];				//je complexne cislo

						}
						if(complexStr.length() > 0){
							matrix[i * rows + j] = new blockValue(splitStr[dataType], complexStr, i * rows + j);
							System.out.println("Complex on input: "+complexStr);
						}
						else{
							inpt1 = Double.parseDouble(splitStr[input1]);
							matrix[i * rows + j] = new blockValue(splitStr[dataType], inpt1, i * rows + j);
							System.out.println("Double on input: "+inpt1);
						}


						/* Set name of block */
						matrix[i*rows + j].getOutput().setNameBlock("blockValue");
						matrix[i*rows + j].setNameBlock("blockValue");				//predelat a smazat tuto metodu v Abstract block
						break;
					case "blockAdd":
						in1 = Integer.parseInt(splitStr[input1]);
						in2 = Integer.parseInt(splitStr[input2]);

						matrix[i*rows + j] = new blockAdd(splitStr[dataType], matrix[in1].getOutput(), matrix[in2].getOutput(), i*rows + j);

						System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						/* Set name of block */
						matrix[i*rows + j].setNameBlock("blockAdd");
						matrix[i*rows + j].getOutput().setNameBlock("blockAdd");

						System.out.println("Here block type: "+matrix[i*rows + j].getNameBlock());
						break;
					case "blockDiv":
						in1 = Integer.parseInt(splitStr[input1]);
						in2 = Integer.parseInt(splitStr[input2]);

						matrix[i*rows + j] = new blockDiv(splitStr[dataType], matrix[in1].getOutput(), matrix[in2].getOutput(), i*rows + j);

						/* Set name of block */
						matrix[i*rows + j].setNameBlock("blockDiv");
						matrix[i*rows + j].getOutput().setNameBlock("blockDiv");

						System.out.println("Here block type DIV");
						break;
					case "blockMul":
						in1 = Integer.parseInt(splitStr[input1]);
						in2 = Integer.parseInt(splitStr[input2]);

						matrix[i*rows + j] = new blockMul(splitStr[dataType], matrix[in1].getOutput(), matrix[in2].getOutput(), i*rows + j);

						System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						/* Set name of block */
						matrix[i*rows + j].setNameBlock("blockMul");
						matrix[i*rows + j].getOutput().setNameBlock("blockMul");

						System.out.println("Here block type MUL");
						break;
					case "blockPow":
						in1 = Integer.parseInt(splitStr[input1]);
						in2 = Integer.parseInt(splitStr[input2]);

						matrix[i*rows + j] = new blockPow(splitStr[dataType], matrix[in1].getOutput(), matrix[in2].getOutput(), i*rows + j);

						System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						/* Set name of block */
						matrix[i*rows + j].setNameBlock("blockPow");
						matrix[i*rows + j].getOutput().setNameBlock("blockPow");

						System.out.println("Here block type POW");
						break;
					case "blockSub":
						in1 = Integer.parseInt(splitStr[input1]);
						in2 = Integer.parseInt(splitStr[input2]);

						matrix[i*rows + j] = new blockSub(splitStr[dataType], matrix[in1].getOutput(), matrix[in2].getOutput(), i*rows + j);

						System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						/* Set name of block */
						matrix[i*rows + j].setNameBlock("blockSub");
						matrix[i*rows + j].getOutput().setNameBlock("blockSub");

						System.out.println("Here block type SUB");
						break;
					case "blockInc":
						System.out.println("Here type INC");
						if(splitStr[input1].indexOf(".") > 0)
							System.out.println("Here is double on input -- TODO");
						else{
							System.out.println("Here is Connection ID on input");
							in1 = Integer.parseInt(splitStr[input1]);
							matrix[i*rows + j] = new blockInc(splitStr[dataType], matrix[in1].getOutput(), i*rows + j);

							/* Set name of block */
							matrix[i*rows + j].setNameBlock("blockInc");
							matrix[i*rows + j].getOutput().setNameBlock("blockInc");
							//  System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						}
						break;
					case "blockDec":
						System.out.println("Here type DEC");
						if(splitStr[input1].indexOf(".") > 0)			//byl li zadan na vstupu id objektu, jine situace nemuzou nastat
							System.out.println("Here is double on input -- TODO");
						else{
							System.out.println("Here is Connection ID on input");
							in1 = Integer.parseInt(splitStr[input1]);
							matrix[i*rows + j] = new blockDec(splitStr[dataType], matrix[in1].getOutput(), i*rows + j);

							/* Set name of block */
							matrix[i*rows + j].setNameBlock("blockDec");
							matrix[i*rows + j].getOutput().setNameBlock("blockDec");
							//  System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						}
						break;
					case "blockNot":
						System.out.println("Here type NOT");
						if(splitStr[input1].indexOf(".") > 0)
							System.out.println("Here is double on input -- TODO");
						else{
							System.out.println("Here is Connection ID on input");
							in1 = Integer.parseInt(splitStr[input1]);			//ID vstupniho objektu
							matrix[i*rows + j] = new blockNot(splitStr[dataType], matrix[in1].getOutput(), i*rows + j);

							/* Set name of block */
							matrix[i*rows + j].setNameBlock("blockNot");
							matrix[i*rows + j].getOutput().setNameBlock("blockNot");
							//  System.out.println("ConnectionID: "+matrix[i*rows + j].getOutput().getConnID());
						}
						break;
					case "none":
						System.out.println("Here is NOTHING");
						break;
				}
			}
		}
		return matrix;
	}
	/**
	 * Function get matrix with objects, run their execute() methods and return string with results, that will be shown in output
	 *
	 * @param matrix - event to control what to do on mouse release
	 * */
	public static String runExecution(AbstractBlock[] matrix){
		System.out.println("In runExecution().");
		int step = 1;
		String output = "", name = "";
		/* Execution */
		for(int i = 0; i < cols; i++){
			for(int j = 0; j < rows; j++){
				if(matrix[i*rows + j] instanceof AbstractBlock)
				{
					//matrix[i*rows + j].execute();
					if(matrix[i*rows + j].getOutput().getType().equals("type1")) {

						if(matrix[i * rows + j].getOutput() != null) {
							System.out.println("Execution: " + matrix[i * rows + j].getOutput().getValue());
							//output = output + "Execution: " + matrix[i * rows + j].getOutput().getValue() + "\n";

							name = matrix[i * rows + j].getNameBlock();
							output = output + "Execution[" + step + "]: \n[blockName]: " + name;

							if (name.equals("blockValue") || name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
								output += "\n[INBlock]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue]: " + matrix[i * rows + j].getInput1().getValue();
							}else {
								output += "\n[INBlock_1]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue_1]: " + matrix[i * rows + j].getInput1().getValue();

								if (name.equals("blockAdd")) {
									output += "\n[INBlock_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockDiv")) {
									output += "\n[INBlock_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockMul")) {
									output += "\n[INBlock_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockPow")) {
									output += "\n[INBlock_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockSub")) {
									output += "\n[INBlock_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getValue();
								}
							}

							output = output + "\n[OUT]: " + matrix[i * rows + j].getOutput().getValue() + "\n\n";
							step++;


						}
					}
					else if(matrix[i*rows + j].getOutput().getType().equals("complex")) {
						System.out.println("Block: " + matrix[i * rows + j].getNameBlock());

						if (!matrix[i * rows + j].getOutput().equals(null)) {


							System.out.println("Execution[" + step + "]: " + matrix[i * rows + j].getOutput().getComplex());

							name = matrix[i * rows + j].getNameBlock();
							output = output + "Execution[" + step + "]: \n[blockName]: " + matrix[i*rows + j].getOutput().getNameBlock();

							if (name.equals("blockValue") || name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
								output += "\n[INBlock]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue]: " + matrix[i * rows + j].getInput1().getComplex();
							} else {
								output += "\n[INBlock_1]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue_1]: " + matrix[i * rows + j].getInput1().getComplex();

								if (name.equals("blockAdd")) {
									output += "\n[INBlock_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockDiv")) {
									output += "\n[INBlock_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockMul")) {
									output += "\n[INBlock_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockPow")) {
									output += "\n[INBlock_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockSub")) {
									output += "\n[INBlock_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getComplex();
								}
							}

							output = output + "\n[OUT]: " + matrix[i * rows + j].getOutput().getComplex() + "\n\n";
							step++;
						}
					}
				}
			}
		}
		return output;
	}

	//-------------------------------------------------------------------------------
	public static String runExecution(AbstractBlock[] matrix, int cols, int rows){
		System.out.println("In runExecution().");
		int step = 1;
		String output = "", name = "";
		/* Execution */
		for(int i = 0; i < cols; i++){
			for(int j = 0; j < rows; j++){
				if(matrix[i*rows + j] instanceof AbstractBlock)
				{
					matrix[i*rows + j].execute();
					if(matrix[i*rows + j].getOutput().getType().equals("type1")) {

						if(matrix[i * rows + j].getOutput() != null) {
							System.out.println("Execution: " + matrix[i * rows + j].getOutput().getValue());
							//output = output + "Execution: " + matrix[i * rows + j].getOutput().getValue() + "\n";

							name = matrix[i * rows + j].getNameBlock();
							output = output + "Execution[" + step + "]: \n[blockName]: " + name;

							if (name.equals("blockValue") || name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
								output += "\n[INBlock]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue]: " + matrix[i * rows + j].getInput1().getValue();
							}else {
								output += "\n[INBlock_1]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue_1]: " + matrix[i * rows + j].getInput1().getValue();

								if (name.equals("blockAdd")) {
									output += "\n[INBlock_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockDiv")) {
									output += "\n[INBlock_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockMul")) {
									output += "\n[INBlock_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockPow")) {
									output += "\n[INBlock_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getValue();
								} else if (name.equals("blockSub")) {
									output += "\n[INBlock_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getValue();
								}
							}

							output = output + "\n[OUT]: " + matrix[i * rows + j].getOutput().getValue() + "\n\n";
							step++;


						}
					}
					else if(matrix[i*rows + j].getOutput().getType().equals("complex")) {
						System.out.println("Block: " + matrix[i * rows + j].getNameBlock());

						if (!matrix[i * rows + j].getOutput().equals(null)) {


							System.out.println("Execution[" + step + "]: " + matrix[i * rows + j].getOutput().getComplex());

							name = matrix[i * rows + j].getNameBlock();
							output = output + "Execution[" + step + "]: \n[blockName]: " + matrix[i*rows + j].getOutput().getNameBlock();

							if (name.equals("blockValue") || name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
								output += "\n[INBlock]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue]: " + matrix[i * rows + j].getInput1().getComplex();
							} else {
								output += "\n[INBlock_1]: " + matrix[i * rows + j].getInput1().getNameBlock() + "\n[INValue_1]: " + matrix[i * rows + j].getInput1().getComplex();

								if (name.equals("blockAdd")) {
									output += "\n[INBlock_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockAdd) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockDiv")) {
									output += "\n[INBlock_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockDiv) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockMul")) {
									output += "\n[INBlock_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockMul) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockPow")) {
									output += "\n[INBlock_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockPow) matrix[i * rows + j]).getInput2().getComplex();
								} else if (name.equals("blockSub")) {
									output += "\n[INBlock_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getNameBlock();
									output += "\n[INValue_2]: " + ((blockSub) matrix[i * rows + j]).getInput2().getComplex();
								}
							}

							output = output + "\n[OUT]: " + matrix[i * rows + j].getOutput().getComplex() + "\n\n";
							step++;
						}
					}
				}
			}
		}
		return output;
	}

	public static void main(String[] args) throws Exception {

		AbstractBlock[] matrix = createMatrixFromFile("/home/xstoik00/workspace/schema/New/schema4_Double.txt");
		//	runExecution(matrix);
		System.out.println("\n-----------\n"+readFile.runExecution(matrix));

	}
}
