package ija.blocks;
import ija.connection.*;


public class blockValue extends AbstractBlock {
    protected String type = "", val = "";

    public blockValue(){

    }

    
    //--- Aktualne --- s CONNECTION_ID
    public blockValue(String type, double value, int connID)
    {
        if(type.equals("type1")) {
        	this.input1 = new connection("type1", value, connID);
        	this.output = new connection("type1", value, connID);
        } else if(type.equals("type2")) {
        	this.input1 = new connection("type2", value, connID);
        	this.output = new connection("type2", value, connID);
        } else if(type.equals("type3")) {
        	this.input1 = new connection("type3", value, connID);
        	this.output = new connection("type3", value, connID);
        }
    }
    
    public blockValue(String type, String valComplex, int connID)
    {  
       if(type.equals("complex")) {
        	this.input1 = new connection("comlex", valComplex, connID);
        	this.output = new connection("comlex", valComplex, connID);

        	this.type = type;
        	this.val = valComplex;
        }
    }
    
    
    public void execute() {
    	if(this.type.equals("complex")){
    		this.output.setComplex(this.input1.complToString());
    	}
    	else
    		this.output.setValue(this.input1.getValue());
    }
}