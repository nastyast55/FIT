package ija.blocks;

import ija.connection.connection;

public class blockSub extends AbstractBlock{
    public connection input2;

    /* While user creates blocks */
    public blockSub(String type, int connID){
        this.output = new connection(type, connID);
    }

    public blockSub (String type, connection c1, connection c2, int connID)
    {
    	super.input1=c1;
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
            output.setReal(input1.getReal()-input2.getReal());
            output.setImag(input1.getImag()-input2.getImag());
            System.out.println("Output complex: "+output.complToString());
            output.setComplex(output.complToString());
        }
        else
            output.setValue(input1.getValue() - input2.getValue());
    }
}