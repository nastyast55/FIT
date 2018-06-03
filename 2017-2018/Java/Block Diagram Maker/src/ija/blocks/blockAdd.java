package ija.blocks;
import ija.connection.connection;


public class blockAdd extends AbstractBlock{
    private connection input2;

    /* While user creates blocks */
    public blockAdd(String type, int connID){
        this.output = new connection(type, connID);
    }

    //-------With connID
    public blockAdd(String type, connection c1, connection c2, int connID)
    {
        this.input1=c1;
        this.input2=c2;
        if(type.equals("complex")){
        	this.output = new connection(type, "0.0+0.0i", connID);
        }
        else
        	this.output = new connection(type, 0.0, connID); 
        
    }

    public connection getInput2() {
        return input2;
    }
    public void setInput2(connection in2){
        this.input2 = in2;
    }

    public void execute() {
    	if(output.getType().equals("complex")){	
    	//	System.out.println("Its will be complex Add");
    		output.setReal(input1.getReal()+input2.getReal());
    		output.setImag(input1.getImag()+input2.getImag());
    		System.out.println("Output complex: "+output.complToString());
    		output.setComplex(output.complToString());
    	}
    	else
    		output.setValue(input1.getValue()+input2.getValue());
        //output.setValue(2.0);
    }
    
}
