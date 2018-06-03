package ija.connection;
import javafx.scene.shape.Line;

public class connection extends Line {
    protected int connectionID = -1;
    protected String type = "", nameBlock = "";
    /* Signed double */
    protected double value;
  
    /* Complex numbers */
    protected double real, imag;
    protected String[] realImag;
    protected String complexStr;

    /* While user creates schema */
    public connection(String type, int connID){
        this.type = type;
        this.connectionID = connID;
    }

    /* Signed double */
    public connection(String type, double value, int connID){
    	this.type = type;
    	this.value = value;
    	this.connectionID = connID;
    }
    
    /* Complex numbers */
    public connection(String type, String valComplex, int connID){
    	this.type = type;
    	this.connectionID = connID;
    	this.complexStr = valComplex;

    	//Parsovani vstupniho retezce s komplexnim cislem
    	realImag = valComplex.split("\\+");
	    if(realImag[0].equals(valComplex)){
	    	realImag = valComplex.split("\\-");
	    } 
	    this.real = Double.parseDouble(realImag[0]);
	    this.imag = Double.parseDouble(realImag[1].split("i")[0]);
	
    }
 	
    	
    /* Signed double */
    public void setValue(double value) {
        this.value = value;
    }
    /* Signed double */
    public double getValue() {
        return value;
    }
    /* Complex numbers */
    public double getReal(){
    	return this.real;
    }
    /* Complex numbers */
    public double getImag(){
    	return this.imag;
    }
    public void setReal(double real){
    	this.real = real;
    }
    public void setImag(double imag){
    	this.imag = imag;
    }
    /* Complex numbers - return String representation of complex number */
    public String complToString(){
		 String ret = "";

		 String realStr = String.valueOf(this.real);
		 String imagStr = String.valueOf(this.imag);
		  if(this.imag < 0){
			 ret = realStr + "-" + imagStr + "i";

		 }
		 else{
			 ret = realStr + "+" + imagStr + "i";
			 
		 }
		 return ret;
    }
    /* Complex numbers */
    public void setComplex(String complexStr){
    	this.complexStr = complexStr;
    }
    public String getComplex(){
    	return this.complexStr;
    }
    
    public int getConnID(){
    	return connectionID;
    }
    /* Pouziva se pri vytvarani schematu uzivatelem - vsechny bloky krome blockValue */
    public void setConnID(int id) {
        this.connectionID = id;
    }

    public String getType(){
    	return this.type;
    }

    public void setNameBlock(String name){
        this.nameBlock = name;
    }
    public String getNameBlock(){
        return this.nameBlock;
    }



}
